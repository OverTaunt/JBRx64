/*
 * Copyright (C) 2008-2019 TrinityCore <https://www.trinitycore.org/>
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

#ifndef DB2STORE_H
#define DB2STORE_H

#include "Common.h"
#include "DB2FileLoader.h"
#include "Errors.h"
#include "ByteBuffer.h"
#include <vector>

/// Interface class for common access
class DB2StorageBase
{
public:
    virtual ~DB2StorageBase() { }

    uint32 GetHash() const { return tableHash; }

    virtual bool HasRecord(uint32 id) const = 0;

    virtual void WriteRecord(uint32 id, LocaleConstant localeConstant, ByteBuffer& buffer) const = 0;

protected:
    uint32 tableHash;
};

template<class T>
class DB2Storage;

template<class T>
bool DB2StorageHasEntry(DB2Storage<T> const& store, uint32 id)
{
    return store.LookupEntry(id) != nullptr;
}

template<class T>
void WriteDB2RecordToPacket(DB2Storage<T> const& store, uint32 id, uint32 locale, ByteBuffer& buffer)
{
    uint8 const* entry = (uint8 const*)store.LookupEntry(id);
    ASSERT(entry);

    std::string format = store.GetFormat();
    for (uint32 i = 0; i < format.length(); ++i)
    {
        switch (format[i])
        {
            case FT_IND:
            case FT_INT:
                buffer << *(uint32*)entry;
                entry += 4;
                break;
            case FT_FLOAT:
                buffer << *(float*)entry;
                entry += 4;
                break;
            case FT_BYTE:
                buffer << *(uint8*)entry;
                entry += 1;
                break;
            case FT_STRING:
            {
                LocalizedString* locStr = *(LocalizedString**)entry;
                if (locStr->Str[locale][0] == '\0')
                    locale = 0;

                char const* str = locStr->Str[locale];
                size_t len = strlen(str);
                buffer << uint16(len);
                if (len)
                    buffer << str;
                entry += sizeof(char*);
                break;
            }
            case FT_NA:
            case FT_SORT:
                buffer << uint32(0);
                break;
            case FT_NA_BYTE:
                buffer << uint8(0);
                break;
        }
    }
}

template<class T>
class DB2Storage : public DB2StorageBase
{
    typedef std::vector<char*> StringPoolList;
    typedef std::vector<T*> DataTableEx;
    typedef bool(*EntryChecker)(DB2Storage<T> const&, uint32);
    typedef void(*PacketWriter)(DB2Storage<T> const&, uint32, LocaleConstant, ByteBuffer&);
public:
    DB2Storage(char const* f, EntryChecker checkEntry = nullptr, PacketWriter writePacket = nullptr) :
        nCount(0), fieldCount(0), fmt(f), m_dataTable(nullptr)
    {
        indexTable.asT = nullptr;
        CheckEntry = checkEntry ? checkEntry : (EntryChecker)&DB2StorageHasEntry<T>;
        WritePacket = writePacket ? writePacket : (PacketWriter)&WriteDB2RecordToPacket<T>;
    }

    ~DB2Storage() { Clear(); }

    bool HasRecord(uint32 id) const { return CheckEntry(*this, id); }
    T const* LookupEntry(uint32 id) const { return (id >= nCount) ? nullptr : indexTable.asT[id]; }
    uint32 GetNumRows() const { return nCount; }
    char const* GetFormat() const { return fmt; }
    uint32 GetFieldCount() const { return fieldCount; }
    void WriteRecord(uint32 id, LocaleConstant localeConstant, ByteBuffer& buffer) const
    {
        WritePacket(*this, id, localeConstant, buffer);
    }

    T* CreateEntry(uint32 id, bool evenIfExists = false)
    {
        if (evenIfExists && LookupEntry(id))
            return nullptr;

        if (id >= nCount)
        {
            // reallocate index table
            char** tmpIdxTable = new char*[id + 1];
            memset(tmpIdxTable, 0, (id + 1) * sizeof(char*));
            memcpy(tmpIdxTable, indexTable.asChar, nCount * sizeof(char*));
            delete[] reinterpret_cast<char*>(indexTable.asT);
            nCount = id + 1;
            indexTable.asChar = tmpIdxTable;
        }

        T* entryDst = new T;
        m_dataTableEx.push_back(entryDst);
        indexTable.asT[id] = entryDst;
        return entryDst;
    }

    void EraseEntry(uint32 id) { indexTable.asT[id] = nullptr; }

    bool Load(char const* fn, LocaleConstant localeConstant)
    {
        DB2FileLoader db2;
        // Check if load was sucessful, only then continue
        if (!db2.Load(fn, fmt))
            return false;

        fieldCount = db2.GetCols();
        tableHash = db2.GetHash();

        // load raw non-string data
        m_dataTable = reinterpret_cast<T*>(db2.AutoProduceData(fmt, nCount, indexTable.asChar));

        // create string holders for loaded string fields
        m_stringPoolList.push_back(db2.AutoProduceStringsArrayHolders(fmt, (char*)m_dataTable));

        // load strings from dbc data
        m_stringPoolList.push_back(db2.AutoProduceStrings(fmt, (char*)m_dataTable, localeConstant));

        // error in dbc file at loading if nullptr
        return indexTable.asT != nullptr;
    }

    bool LoadStringsFrom(char const* fn, uint32 locale)
    {
        // DBC must be already loaded using Load
        if (!indexTable.asT)
            return false;

        DB2FileLoader db2;
        // Check if load was successful, only then continue
        if (!db2.Load(fn, fmt))
            return false;

        // load strings from another locale dbc data
        m_stringPoolList.push_back(db2.AutoProduceStrings(fmt, (char*)m_dataTable, locale));

        return true;
    }

    void Clear()
    {
        if (!indexTable.asT)
            return;

        delete[] reinterpret_cast<char*>(indexTable.asT);
        indexTable.asT = nullptr;

        delete[] reinterpret_cast<char*>(m_dataTable);
        m_dataTable = nullptr;

        for (typename DataTableEx::iterator itr = m_dataTableEx.begin(); itr != m_dataTableEx.end(); ++itr)
            delete *itr;
        m_dataTableEx.clear();

        for (char* strings : m_stringPoolList)
            delete[] strings;
        m_stringPoolList.clear();

        nCount = 0;
    }

    EntryChecker CheckEntry;
    PacketWriter WritePacket;

private:
    uint32 nCount;
    uint32 fieldCount;
    char const* fmt;
    union
    {
        T** asT;
        char** asChar;
    } indexTable;
    T* m_dataTable;
    DataTableEx m_dataTableEx;
    StringPoolList m_stringPoolList;
};

#endif
