--
DELETE FROM `spell_script_names` WHERE `spell_id` IN (99257,99402,99403,99404,37594);
INSERT INTO `spell_script_names` VALUES
(99257, 'spell_baleroc_tormented_debuff'),
(99402, 'spell_baleroc_tormented_debuff'),
(99403, 'spell_baleroc_tormented_debuff'),
(99404, 'spell_baleroc_tormented_debuff'),
(37594, 'spell_pri_item_greater_heal_refund');

UPDATE `creature_template` SET `ScriptName`="" WHERE `entry` IN (2530, 11056, 11057);

UPDATE `creature` SET `phaseMask` = 1 WHERE `guid` = 250021;

-- Professor Phizzlethorpe
UPDATE `creature_template` SET `AIName`="SmartAI", `ScriptName`="" WHERE `entry`=2768;
DELETE FROM `smart_scripts` WHERE `entryorguid`=2768 AND `source_type`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (276800,276801,276802,276803) AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(2768,0,0,1,19,0,100,0,26050,0,0,0,0,64,1,0,0,0,0,0,7,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Quest 'Sunken Treasure' Taken - Store Targetlist"),
(2768,0,1,2,61,0,100,0,0,0,0,0,0,83,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Quest 'Sunken Treasure' Taken - Remove Npc Flag Questgiver"),
(2768,0,2,3,61,0,100,0,0,0,0,0,0,80,276800,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Quest 'Sunken Treasure' Taken - Run Script"),
(2768,0,3,0,61,0,100,0,0,0,0,0,0,2,113,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Quest 'Sunken Treasure' Taken - Set Faction 113"),
(2768,0,4,0,6,0,100,0,0,0,0,0,0,6,26050,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Just Died - Fail Quest 'Sunken Treasure'"),
(2768,0,5,0,4,0,100,0,0,0,0,0,0,1,4,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Aggro - Say Line 4"),
(2768,0,6,0,40,0,100,0,5,276800,0,0,0,80,276801,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Waypoint 5 Reached - Run Script"),
(2768,0,7,0,40,0,100,0,8,276800,0,0,0,80,276802,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Waypoint 8 Reached - Run Script"),
(2768,0,8,0,40,0,100,0,8,276801,0,0,0,80,276803,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Waypoint 8 Reached - Run Script"),
(276800,9,0,0,0,0,100,0,0,0,0,0,0,1,0,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 0"),
(276800,9,1,0,0,0,100,0,4000,4000,0,0,0,53,0,276800,0,0,0,2,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Start Waypoint"),
(276801,9,0,0,0,0,100,0,0,0,0,0,0,54,7000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Pause Waypoint"),
(276801,9,1,0,0,0,100,0,0,0,0,0,0,1,1,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 1"),
(276801,9,2,0,0,0,100,0,7000,7000,0,0,0,1,2,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 2"),
(276802,9,0,0,0,0,100,0,0,0,0,0,0,1,3,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 3"),
(276802,9,1,0,0,0,100,0,10000,10000,0,0,0,107,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Summon Group"),
(276802,9,2,0,0,0,100,0,15000,15000,0,0,0,1,5,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 5"),
(276802,9,3,0,0,0,100,0,10000,10000,0,0,0,1,6,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 6"),
(276802,9,4,0,0,0,100,0,3000,3000,0,0,0,53,0,276801,0,0,0,2,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Start Waypoint"),
(276803,9,0,0,0,0,100,0,0,0,0,0,0,1,7,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 7"),
(276803,9,1,0,0,0,100,0,5000,5000,0,0,0,1,8,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 8"),
(276803,9,2,0,0,0,100,0,0,0,0,0,0,1,9,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Say Line 9"),
(276803,9,3,0,0,0,100,0,0,0,0,0,0,15,26050,0,0,0,0,0,12,1,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Complete Quest 'Sunken Treasure'"),
(276803,9,4,0,0,0,100,0,0,0,0,0,0,82,2,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Add Npc Flag Questgiver"),
(276803,9,5,0,0,0,100,0,0,0,0,0,0,2,35,0,0,0,0,0,1,0,0,0,0,0,0,0,"Professor Phizzlethorpe - On Script - Set Faction 35");

DELETE FROM `creature_summon_groups` WHERE `summonerId`=2768;
INSERT INTO `creature_summon_groups` (`summonerId`, `summonerType`, `groupId`, `entry`, `position_x`, `position_y`, `position_z`, `orientation`, `summonType`, `summonTime`) VALUES
(2768,0,1,2776,-2056.19,-2143.23,20.4528,5.61003,5,3000),
(2768,0,1,2776,-2052.2,-2139.68,19.6804,5.31158,5,3000);

DELETE FROM `script_waypoint` WHERE `entry`=2768;
DELETE FROM `waypoints` WHERE `entry` IN (276800, 276801);
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(276800,1,-2077.73,-2091.17,9.49,""),
(276800,2,-2077.99,-2105.33,13.24,""),
(276800,3,-2074.6,-2109.67,14.24,""),
(276800,4,-2076.6,-2117.46,16.67,""),
(276800,5,-2073.51,-2123.46,18.42,""),
(276800,6,-2066.6,-2131.85,21.56,""),
(276800,7,-2053.85,-2143.19,20.31,""),
(276800,8,-2043.49,-2153.73,20.2,""),
(276801,1,-2053.85,-2143.19,20.31,""),
(276801,2,-2063.52,-2131.57,21.22,""),
(276801,3,-2073.51,-2123.46,18.42,""),
(276801,4,-2076.6,-2117.46,16.67,""),
(276801,5,-2074.6,-2109.67,14.24,""),
(276801,6,-2077.99,-2105.33,13.24,""),
(276801,7,-2077.73,-2091.17,9.49,""),
(276801,8,-2066.41,-2086.21,8.97,"");

-- Vengeful Surge
DELETE FROM `smart_scripts` WHERE `entryorguid`=2776 AND `source_type`=0 AND `id`=1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`event_param5`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(2776,0,1,0,54,0,100,0,0,0,0,0,0,49,0,0,0,0,0,0,19,2768,0,0,0,0,0,0,"Vengeful Surge - Just Summoned - Start Attacking");

DELETE FROM `quest_template_addon` WHERE `ID` IN (260501);
INSERT INTO `quest_template_addon` (`ID`, `SpecialFlags`) VALUES
(26050, 2);
