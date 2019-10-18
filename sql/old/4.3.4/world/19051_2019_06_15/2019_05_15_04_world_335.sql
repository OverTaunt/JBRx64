/*
-- Message to the Wildhammer
UPDATE `gameobject_template` SET `AIName`="SmartGameObjectAI" WHERE `entry`=179911;
DELETE FROM `smart_scripts` WHERE `entryorguid`=179911 AND `source_type`=1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(179911,1,0,0,11,0,100,0,0,0,0,0,1,0,0,0,0,0,0,10,92849,5635,0,0,0,0,0,"Regulator Valve - On Spawn - Say Line 0 (Falstad Wildhammer)");

-- Falstad Wildhammer
DELETE FROM `creature_text` WHERE `CreatureID`=5635;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(5635,0,0,"Invaders! Invaders! We're under attack, boys! Get 'em!",14,0,100,0,0,0,10009,2,"Falstad Wildhammer");
*/
