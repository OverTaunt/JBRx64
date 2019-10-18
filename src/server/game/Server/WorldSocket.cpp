/*
* Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WorldSocket.h"
#include "BattlenetAccountMgr.h"
#include "BigNumber.h"
#include "DatabaseEnv.h"
#include "IPLocation.h"
#include "Opcodes.h"
#include "PacketLog.h"
#include "QueryCallback.h"
#include "Random.h"
#include "RBAC.h"
#include "Realm.h"
#include "ScriptMgr.h"
#include "SHA1.h"
#include "World.h"
#include <memory>

class EncryptablePacket : public WorldPacket
{
public:
    EncryptablePacket(WorldPacket const& packet, bool encrypt) : WorldPacket(packet), _encrypt(encrypt) { }

    bool NeedsEncryption() const { return _encrypt; }

private:
    bool _encrypt;
};

using boost::asio::ip::tcp;

std::string const WorldSocket::ServerConnectionInitialize("WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT");

std::string const WorldSocket::ClientConnectionInitialize("WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER");


WorldSocket::WorldSocket(tcp::socket&& socket)
    : Socket(std::move(socket)), _authSeed(rand32()), _OverSpeedPings(0), _worldSession(nullptr),  _sendBufferSize(4096), _initialized(false)
{
    _headerBuffer.Resize(2);
}

void WorldSocket::Start()
{
    std::string ip_address = GetRemoteIpAddress().to_string();
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_IP_INFO);
    stmt->setString(0, ip_address);

    _queryProcessor.AddQuery(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&WorldSocket::CheckIpCallback, this, std::placeholders::_1)));
}

void WorldSocket::CheckIpCallback(PreparedQueryResult result)
{
    if (result)
    {
        bool banned = false;
        do
        {
            Field* fields = result->Fetch();
            if (fields[0].GetUInt64() != 0)
                banned = true;

        } while (result->NextRow());

        if (banned)
        {
            SendAuthResponseError(AUTH_REJECT);
            TC_LOG_ERROR("network", "WorldSocket::CheckIpCallback: Sent Auth Response (IP %s banned).", GetRemoteIpAddress().to_string().c_str());
            DelayedCloseSocket();
            return;
        }
    }

    AsyncRead();

    MessageBuffer initializer;
    ServerPktHeader header(ServerConnectionInitialize.size(), 0);
    initializer.Write(header.header, header.getHeaderLength() - 2);
    initializer.Write(ServerConnectionInitialize.c_str(), ServerConnectionInitialize.length());

    // - io_service.run thread, safe.
    QueuePacket(std::move(initializer));
}

bool WorldSocket::Update()
{
    EncryptablePacket* queued;
    MessageBuffer buffer(_sendBufferSize);
    while (_bufferQueue.Dequeue(queued))
    {
        ServerPktHeader header(queued->size() + 2, queued->GetOpcode());
        if (queued->NeedsEncryption())
            _authCrypt.EncryptSend(header.header, header.getHeaderLength());

        if (buffer.GetRemainingSpace() < queued->size() + header.getHeaderLength())
        {
            QueuePacket(std::move(buffer));
            buffer.Resize(_sendBufferSize);
        }

        if (buffer.GetRemainingSpace() >= queued->size() + header.getHeaderLength())
        {
            buffer.Write(header.header, header.getHeaderLength());
            if (!queued->empty())
                buffer.Write(queued->contents(), queued->size());
        }
        else    // single packet larger than 4096 bytes
        {
            MessageBuffer packetBuffer(queued->size() + header.getHeaderLength());
            packetBuffer.Write(header.header, header.getHeaderLength());
            if (!queued->empty())
                packetBuffer.Write(queued->contents(), queued->size());

            QueuePacket(std::move(packetBuffer));
        }

        delete queued;
    }

    if (buffer.GetActiveSize() > 0)
        QueuePacket(std::move(buffer));

    if (!BaseSocket::Update())
        return false;

    _queryProcessor.ProcessReadyQueries();

    return true;
}

void WorldSocket::HandleSendAuthSession()
{
    WorldPacket packet(SMSG_AUTH_CHALLENGE, 37);
    BigNumber seed1;
    seed1.SetRand(16 * 8);
    packet.append(seed1.AsByteArray(16).get(), 16);               // new encryption seeds

    BigNumber seed2;
    seed2.SetRand(16 * 8);
    packet.append(seed2.AsByteArray(16).get(), 16);               // new encryption seeds

    packet << uint32(_authSeed);
    packet << uint8(1);
    SendPacketAndLogOpcode(packet);
}

void WorldSocket::OnClose()
{
    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);
        _worldSession = nullptr;
    }
}

void WorldSocket::ReadHandler()
{
    if (!IsOpen())
        return;

    MessageBuffer& packet = GetReadBuffer();
    while (packet.GetActiveSize() > 0)
    {
        if (_headerBuffer.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (_headerBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }

            // We just received nice new header
            if (!ReadHeaderHandler())
            {
                CloseSocket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetRemainingSpace() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
            _packetBuffer.Write(packet.GetReadPointer(), readDataSize);
            packet.ReadCompleted(readDataSize);

            if (_packetBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }
        }

        // just received fresh new payload
        ReadDataHandlerResult result = ReadDataHandler();
        _headerBuffer.Reset();
        if (result != ReadDataHandlerResult::Ok)
        {
            if (result != ReadDataHandlerResult::WaitingForQuery)
                CloseSocket();

            return;
        }
    }

    AsyncRead();
}

bool WorldSocket::ReadHeaderHandler()
{
    ASSERT(_headerBuffer.GetActiveSize() == (_initialized ? sizeof(ClientPktHeader) : 2));

    _authCrypt.DecryptRecv(_headerBuffer.GetReadPointer(), _headerBuffer.GetActiveSize());

    ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
    EndianConvertReverse(header->size);

    if (_initialized)
        EndianConvert(header->cmd);

    if (!header->IsValidSize() || (_initialized && !header->IsValidOpcode()))
    {
        TC_LOG_ERROR("network", "WorldSocket::ReadHeaderHandler(): client %s sent malformed packet (size: %hu, cmd: %u)",
            GetRemoteIpAddress().to_string().c_str(), header->size, header->cmd);
        return false;
    }

    if (_initialized)
        header->size -= sizeof(header->cmd);

    _packetBuffer.Resize(header->size);
    return true;
}

struct AuthSession
{
    uint8 LoginServerType = 0;
    uint32 RealmID = 0;
    uint16 Build = 0;
    uint32 LocalChallenge = 0;
    uint8 Digest[SHA_DIGEST_LENGTH] = {};
    std::string Account;
    ByteBuffer AddonInfo;
};

struct AccountInfo
{
    struct
    {
        uint32 Id;
        bool IsLockedToIP;
        std::string LastIP;
        std::string LockCountry;
        LocaleConstant Locale;
        std::string OS;
        bool IsBanned;
    } BattleNet;

    struct
    {
        uint32 Id;
        BigNumber SessionKey;
        uint8 Expansion;
        int64 MuteTime;
        uint32 Recruiter;
        bool IsRecruiter;
        AccountTypes Security;
        bool IsBanned;
    } Game;

    bool IsBanned() const { return BattleNet.IsBanned || Game.IsBanned; }

    explicit AccountInfo(Field* fields)
    {
        //           0             1           2          3                4            5           6          7            8      9     10          11
        // SELECT a.id, a.sessionkey, ba.last_ip, ba.locked, ba.lock_country, a.expansion, a.mutetime, ba.locale, a.recruiter, ba.os, ba.id, aa.gmLevel,
        //                                                              12                                                            13    14
        // bab.unbandate > UNIX_TIMESTAMP() OR bab.unbandate = bab.bandate, ab.unbandate > UNIX_TIMESTAMP() OR ab.unbandate = ab.bandate, r.id
        // FROM account a LEFT JOIN battlenet_accounts ba ON a.battlenet_account = ba.id LEFT JOIN account_access aa ON a.id = aa.id AND aa.RealmID IN (-1, ?)
        // LEFT JOIN battlenet_account_bans bab ON ba.id = bab.id LEFT JOIN account_banned ab ON a.id = ab.id LEFT JOIN account r ON a.id = r.recruiter
        // WHERE a.username = ? ORDER BY aa.RealmID DESC LIMIT 1
        Game.Id = fields[0].GetUInt32();
        Game.SessionKey.SetHexStr(fields[1].GetCString());
        BattleNet.LastIP = fields[2].GetString();
        BattleNet.IsLockedToIP = fields[3].GetBool();
        BattleNet.LockCountry = fields[4].GetString();
        Game.Expansion = fields[5].GetUInt8();
        Game.MuteTime = fields[6].GetInt64();
        BattleNet.Locale = LocaleConstant(fields[7].GetUInt8());
        Game.Recruiter = fields[8].GetUInt32();
        BattleNet.OS = fields[9].GetString();
        BattleNet.Id = fields[10].GetUInt32();
        Game.Security = AccountTypes(fields[11].GetUInt8());
        BattleNet.IsBanned = fields[12].GetUInt64() != 0;
        Game.IsBanned = fields[13].GetUInt64() != 0;
        Game.IsRecruiter = fields[14].GetUInt32() != 0;

        uint32 world_expansion = sWorld->getIntConfig(CONFIG_EXPANSION);
        if (Game.Expansion > world_expansion)
            Game.Expansion = world_expansion;

        if (BattleNet.Locale >= TOTAL_LOCALES)
            BattleNet.Locale = LOCALE_enUS;
    }
};

WorldSocket::ReadDataHandlerResult WorldSocket::ReadDataHandler()
{
    if (_initialized)
    {
        ClientPktHeader* header = reinterpret_cast<ClientPktHeader*>(_headerBuffer.GetReadPointer());
        OpcodeClient opcode = static_cast<OpcodeClient>(header->cmd);


        WorldPacket packet(opcode, std::move(_packetBuffer));
        WorldPacket* packetToQueue;

        if (sPacketLog->CanLogPacket())
            sPacketLog->LogPacket(packet, CLIENT_TO_SERVER, GetRemoteIpAddress(), GetRemotePort());

        std::unique_lock<std::mutex> sessionGuard(_worldSessionLock, std::defer_lock);

        switch (opcode)
        {
            case CMSG_PING:
            {
                LogOpcodeText(opcode, sessionGuard);
                try
                {
                    return HandlePing(packet) ? ReadDataHandlerResult::Ok : ReadDataHandlerResult::Error;
                }
                catch (ByteBufferException const&)
                {
                }
                TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler(): client %s sent malformed CMSG_PING", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            case CMSG_AUTH_SESSION:
            {
                LogOpcodeText(opcode, sessionGuard);
                if (_worldSession)
                {
                    // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                    if (sessionGuard.try_lock())
                        TC_LOG_ERROR("network", "WorldSocket::ProcessIncoming: received duplicate CMSG_AUTH_SESSION from %s", _worldSession->GetPlayerInfo().c_str());
                    return ReadDataHandlerResult::Error;
                }

                try
                {
                    HandleAuthSession(packet);
                    return ReadDataHandlerResult::WaitingForQuery;
                }
                catch (ByteBufferException const&)
                {
                }
                TC_LOG_ERROR("network", "WorldSocket::ReadDataHandler(): client %s sent malformed CMSG_AUTH_SESSION", GetRemoteIpAddress().to_string().c_str());
                return ReadDataHandlerResult::Error;
            }
            case CMSG_LOG_DISCONNECT:
                packet.rfinish();   // contains uint32 disconnectReason;
                TC_LOG_DEBUG("network", "%s", GetOpcodeNameForLogging(opcode).c_str());
                sScriptMgr->OnPacketReceive(_worldSession, packet);
                return ReadDataHandlerResult::Ok;
            case CMSG_ENABLE_NAGLE:
                TC_LOG_DEBUG("network", "%s", GetOpcodeNameForLogging(opcode).c_str());
                sScriptMgr->OnPacketReceive(_worldSession, packet);
                if (_worldSession)
                    _worldSession->HandleEnableNagleAlgorithm();
                return ReadDataHandlerResult::Ok;
            case CMSG_KEEP_ALIVE: // todo: handle this packet in the same way of CMSG_TIME_SYNC_RESP
                LogOpcodeText(opcode, sessionGuard);
                sScriptMgr->OnPacketReceive(_worldSession, packet);
                return ReadDataHandlerResult::Ok;
            case CMSG_TIME_SYNC_RESP:
                packetToQueue = new WorldPacket(std::move(packet), std::chrono::steady_clock::now());
                break;
            default:
                packetToQueue = new WorldPacket(std::move(packet));
                break;
        }

        sessionGuard.lock();

        LogOpcodeText(opcode, sessionGuard);

        if (!_worldSession)
        {
            TC_LOG_ERROR("network.opcode", "ProcessIncoming: Client not authed opcode = %u", uint32(opcode));
            return ReadDataHandlerResult::Error;
        }

        // prevent invalid memory access/crash with custom opcodes
        if (uint16(opcode) >= NUM_OPCODE_HANDLERS)
        {
            CloseSocket();
            return ReadDataHandlerResult::Error;
        }

        OpcodeHandler const* handler = opcodeTable[opcode];
        if (!handler)
        {
            TC_LOG_ERROR("network.opcode", "No defined handler for opcode %s sent by %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(packet.GetOpcode())).c_str(), _worldSession->GetPlayerInfo().c_str());
            return ReadDataHandlerResult::Error;
        }

        // Our Idle timer will reset on any non PING opcodes.
        // Catches people idling on the login screen and any lingering ingame connections.
        _worldSession->ResetTimeOutTime();

        // Copy the packet to the heap before enqueuing
        _worldSession->QueuePacket(packetToQueue);
    }
    else
    {
        std::string initializer(reinterpret_cast<char const*>(_packetBuffer.GetReadPointer()), std::min(_packetBuffer.GetActiveSize(), ClientConnectionInitialize.length()));
        if (initializer != ClientConnectionInitialize)
        {
            CloseSocket();
            return ReadDataHandlerResult::Error;
        }

        _initialized = true;
        _headerBuffer.Resize(sizeof(ClientPktHeader));
        _packetBuffer.Reset();
        HandleSendAuthSession();
    }

    return ReadDataHandlerResult::Ok;
}

void WorldSocket::LogOpcodeText(OpcodeClient opcode, std::unique_lock<std::mutex> const& guard) const
{
    if (!guard)
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(opcode).c_str());
    }
    else
    {
        TC_LOG_TRACE("network.opcode", "C->S: %s %s", (_worldSession ? _worldSession->GetPlayerInfo() : GetRemoteIpAddress().to_string()).c_str(),
            GetOpcodeNameForLogging(opcode).c_str());
    }
}

void WorldSocket::SendPacketAndLogOpcode(WorldPacket& packet)
{
    TC_LOG_TRACE("network.opcode", "S->C: %s %s", GetRemoteIpAddress().to_string().c_str(), GetOpcodeNameForLogging(static_cast<OpcodeServer>(packet.GetOpcode())).c_str());
    SendPacket(packet);
}

void WorldSocket::SendPacket(WorldPacket const& packet)
{
    if (!IsOpen())
        return;

    if (sPacketLog->CanLogPacket())
        sPacketLog->LogPacket(packet, SERVER_TO_CLIENT, GetRemoteIpAddress(), GetRemotePort());

    _bufferQueue.Enqueue(new EncryptablePacket(packet, _authCrypt.IsInitialized()));
}

void WorldSocket::HandleAuthSession(WorldPacket& recvPacket)
{
    uint32 addonSize;
    std::shared_ptr<AuthSession> authSession = std::make_shared<AuthSession>();

    recvPacket.read_skip<uint32>(); // ServerId - Used for GRUNT only
    recvPacket.read_skip<uint32>(); // Battlegroup
    recvPacket >> authSession->LoginServerType;
    recvPacket >> authSession->Digest[10];
    recvPacket >> authSession->Digest[18];
    recvPacket >> authSession->Digest[12];
    recvPacket >> authSession->Digest[5];
    recvPacket.read_skip<uint64>();
    recvPacket >> authSession->Digest[15];
    recvPacket >> authSession->Digest[9];
    recvPacket >> authSession->Digest[19];
    recvPacket >> authSession->Digest[4];
    recvPacket >> authSession->Digest[7];
    recvPacket >> authSession->Digest[16];
    recvPacket >> authSession->Digest[3];
    recvPacket >> authSession->Build;
    recvPacket >> authSession->Digest[8];
    recvPacket >> authSession->RealmID;
    recvPacket.read_skip<uint8>();
    recvPacket >> authSession->Digest[17];
    recvPacket >> authSession->Digest[6];
    recvPacket >> authSession->Digest[0];
    recvPacket >> authSession->Digest[1];
    recvPacket >> authSession->Digest[11];
    recvPacket >> authSession->LocalChallenge;
    recvPacket >> authSession->Digest[2];
    recvPacket.read_skip<uint32>(); // Region
    recvPacket >> authSession->Digest[14];
    recvPacket >> authSession->Digest[13];

    recvPacket >> addonSize;

    if (addonSize)
    {
        authSession->AddonInfo.resize(addonSize);
        recvPacket.read((uint8*)authSession->AddonInfo.contents(), addonSize);
    }

    recvPacket.ReadBit();           // UseIPv6
    uint32 accountNameLength = recvPacket.ReadBits(12);
    authSession->Account = recvPacket.ReadString(accountNameLength);

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO_BY_NAME);
    stmt->setInt32(0, int32(realm.Id.Realm));
    stmt->setString(1, authSession->Account);

    _queryProcessor.AddQuery(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback(std::bind(&WorldSocket::HandleAuthSessionCallback, this, authSession, std::placeholders::_1)));
}

void WorldSocket::HandleAuthSessionCallback(std::shared_ptr<AuthSession> authSession, PreparedQueryResult result)
{
    // Stop if the account is not found
    if (!result)
    {
        // We can not log here, as we do not know the account. Thus, no accountId.
        SendAuthResponseError(AUTH_UNKNOWN_ACCOUNT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Sent Auth Response (unknown account).");
        DelayedCloseSocket();
        return;
    }

    AccountInfo account(result->Fetch());

    // For hook purposes, we get Remoteaddress at this point.
    std::string address = GetRemoteIpAddress().to_string();

    // As we don't know if attempted login process by ip works, we update last_attempt_ip right away
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_ATTEMPT_IP);
    stmt->setString(0, address);
    stmt->setString(1, authSession->Account);
    LoginDatabase.Execute(stmt);
    // This also allows to check for possible "hack" attempts on account

    // even if auth credentials are bad, try using the session key we have - client cannot read auth response error without it
    _authCrypt.Init(&account.Game.SessionKey);

    // First reject the connection if packet contains invalid data or realm state doesn't allow logging in
    if (sWorld->IsClosed())
    {
        SendAuthResponseError(AUTH_REJECT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: World closed, denying client (%s).", GetRemoteIpAddress().to_string().c_str());
        DelayedCloseSocket();
        return;
    }

    if (authSession->RealmID != realm.Id.Realm)
    {
        SendAuthResponseError(REALM_LIST_REALM_NOT_FOUND);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Client %s requested connecting with realm id %u but this realm has id %u set in config.",
            GetRemoteIpAddress().to_string().c_str(), authSession->RealmID, realm.Id.Realm);
        DelayedCloseSocket();
        return;
    }

    // Must be done before WorldSession is created
    bool wardenActive = sWorld->getBoolConfig(CONFIG_WARDEN_ENABLED);
    if (wardenActive && account.BattleNet.OS != "Win" && account.BattleNet.OS != "OSX")
    {
        SendAuthResponseError(AUTH_REJECT);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Client %s attempted to log in using invalid client OS (%s).", address.c_str(), account.BattleNet.OS.c_str());
        DelayedCloseSocket();
        return;
    }

    // Check that Key and account name are the same on client and server
    uint32 t = 0;

    SHA1Hash sha;
    sha.UpdateData(authSession->Account);
    sha.UpdateData((uint8*)&t, 4);
    sha.UpdateData((uint8*)&authSession->LocalChallenge, 4);
    sha.UpdateData((uint8*)&_authSeed, 4);
    sha.UpdateBigNumbers(&account.Game.SessionKey, nullptr);
    sha.Finalize();

    if (memcmp(sha.GetDigest(), authSession->Digest, SHA_DIGEST_LENGTH) != 0)
    {
        SendAuthResponseError(AUTH_FAILED);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Authentication failed for account: %u ('%s') address: %s", account.Game.Id, authSession->Account.c_str(), address.c_str());
        DelayedCloseSocket();
        return;
    }

    if (IpLocationRecord const* location = sIPLocation->GetLocationRecord(address))
        _ipCountry = location->CountryCode;

    ///- Re-check ip locking (same check as in auth).
    if (account.BattleNet.IsLockedToIP) // if ip is locked
    {
        if (account.BattleNet.LastIP != address)
        {
            SendAuthResponseError(AUTH_FAILED);
            TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account IP differs. Original IP: %s, new IP: %s).", account.BattleNet.LastIP.c_str(), address.c_str());
            // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
            sScriptMgr->OnFailedAccountLogin(account.Game.Id);
            DelayedCloseSocket();
            return;
        }
    }
    else if (!account.BattleNet.LockCountry.empty() && account.BattleNet.LockCountry != "00" && !_ipCountry.empty())
    {
        if (account.BattleNet.LockCountry != _ipCountry)
        {
            SendAuthResponseError(AUTH_FAILED);
            TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account country differs. Original country: %s, new country: %s).", account.BattleNet.LockCountry.c_str(), _ipCountry.c_str());
            // We could log on hook only instead of an additional db log, however action logger is config based. Better keep DB logging as well
            sScriptMgr->OnFailedAccountLogin(account.Game.Id);
            DelayedCloseSocket();
            return;
        }
    }

    int64 mutetime = account.Game.MuteTime;
    //! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
    if (mutetime < 0)
    {
        mutetime = time(nullptr) + llabs(mutetime);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
        stmt->setInt64(0, mutetime);
        stmt->setUInt32(1, account.Game.Id);
        LoginDatabase.Execute(stmt);
    }

    if (account.IsBanned())
    {
        SendAuthResponseError(AUTH_BANNED);
        TC_LOG_ERROR("network", "WorldSocket::HandleAuthSession: Sent Auth Response (Account banned).");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        DelayedCloseSocket();
        return;
    }

    // Check locked state for server
    AccountTypes allowedAccountType = sWorld->GetPlayerSecurityLimit();
    TC_LOG_DEBUG("network", "Allowed Level: %u Player Level %u", allowedAccountType, account.Game.Security);
    if (allowedAccountType > SEC_PLAYER && account.Game.Security < allowedAccountType)
    {
        SendAuthResponseError(AUTH_UNAVAILABLE);
        TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: User tries to login but his security level is not enough");
        sScriptMgr->OnFailedAccountLogin(account.Game.Id);
        DelayedCloseSocket();
        return;
    }

    TC_LOG_DEBUG("network", "WorldSocket::HandleAuthSession: Client '%s' authenticated successfully from %s.", authSession->Account.c_str(), address.c_str());

    // Update the last_ip in the database as it was successful for login
    stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LAST_IP);
    stmt->setString(0, address);
    stmt->setString(1, authSession->Account);
    LoginDatabase.Execute(stmt);

    // At this point, we can safely hook a successful login
    sScriptMgr->OnAccountLogin(account.Game.Id);

    _worldSession = new WorldSession(account.Game.Id, std::move(authSession->Account), account.BattleNet.Id, shared_from_this(), account.Game.Security,
        account.Game.Expansion, mutetime, account.BattleNet.Locale, account.Game.Recruiter, account.Game.IsRecruiter);
    _worldSession->ReadAddonsInfo(authSession->AddonInfo);

    // Initialize Warden system only if it is enabled by config
    if (wardenActive)
        _worldSession->InitWarden(&account.Game.SessionKey, account.BattleNet.OS);

    _queryProcessor.AddQuery(_worldSession->LoadPermissionsAsync().WithPreparedCallback(std::bind(&WorldSocket::LoadSessionPermissionsCallback, this, std::placeholders::_1)));
    AsyncRead();
}

void WorldSocket::LoadSessionPermissionsCallback(PreparedQueryResult result)
{
    // RBAC must be loaded before adding session to check for skip queue permission
    _worldSession->GetRBACData()->LoadFromDBCallback(result);

    sWorld->AddSession(_worldSession);
}

void WorldSocket::SendAuthResponseError(uint8 code)
{
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
    packet.WriteBit(0); // has queue info
    packet.WriteBit(0); // has account info
    packet << uint8(code);

    SendPacketAndLogOpcode(packet);
}

bool WorldSocket::HandlePing(WorldPacket& recvPacket)
{
    using namespace std::chrono;

    uint32 ping;
    uint32 latency;

    // Get the ping packet content
    recvPacket >> latency;
    recvPacket >> ping;

    if (_LastPingTime == steady_clock::time_point())
    {
        _LastPingTime = steady_clock::now();
    }
    else
    {
        steady_clock::time_point now = steady_clock::now();

        steady_clock::duration diff = now - _LastPingTime;

        _LastPingTime = now;

        if (diff < seconds(27))
        {
            ++_OverSpeedPings;

            uint32 maxAllowed = sWorld->getIntConfig(CONFIG_MAX_OVERSPEED_PINGS);

            if (maxAllowed && _OverSpeedPings > maxAllowed)
            {
                std::unique_lock<std::mutex> sessionGuard(_worldSessionLock);

                if (_worldSession && !_worldSession->HasPermission(rbac::RBAC_PERM_SKIP_CHECK_OVERSPEED_PING))
                {
                    TC_LOG_ERROR("network", "WorldSocket::HandlePing: %s kicked for over-speed pings (address: %s)",
                        _worldSession->GetPlayerInfo().c_str(), GetRemoteIpAddress().to_string().c_str());

                    return false;
                }
            }
        }
        else
            _OverSpeedPings = 0;
    }

    {
        std::lock_guard<std::mutex> sessionGuard(_worldSessionLock);

        if (_worldSession)
            _worldSession->SetLatency(latency);
        else
        {
            TC_LOG_ERROR("network", "WorldSocket::HandlePing: peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", GetRemoteIpAddress().to_string().c_str());
            return false;
        }
    }

    WorldPacket packet(SMSG_PONG, 4);
    packet << ping;
    SendPacketAndLogOpcode(packet);
    return true;
}
