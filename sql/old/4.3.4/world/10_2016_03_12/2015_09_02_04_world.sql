-- Temple Guardian Anhuur loot -- Normal/Heroic
UPDATE creature_template SET `lootid`= 39425 where `entry`= 39425;
UPDATE creature_template SET `lootid`= 49262 where `entry`= 49262;

DELETE FROM `creature_loot_template` WHERE `Entry` IN (39425, 49262);
INSERT INTO `creature_loot_template` (`entry`, `item`, `Reference`, `Chance`, `QuestRequired`, `Lootmode`, `GroupId`, `MinCount`, `MaxCount`,`Comment`) VALUES
(39425,1,39425,100,0,1,0,1,1,"Anhuur N - Reference Loot"),
(49262,1,49262,100,0,1,0,1,1,"Anhuur H - Reference Loot");

DELETE FROM `reference_loot_template` where `entry` IN (39425,49262);
INSERT INTO `reference_loot_template` (`entry`, `item`, `Reference`, `Chance`, `QuestRequired`, `Lootmode`, `GroupId`, `MinCount`, `MaxCount`) VALUES
(39425,55889,0,0,0,1,1,1,1),
(39425,55888,0,0,0,1,1,1,1),
(39425,55886,0,0,0,1,1,1,1),
(39425,55887,0,0,0,1,1,1,1),
(39425,55890,0,0,0,1,1,1,1),
(49262,56407,0,0,0,1,1,1,1),
(49262,56410,0,0,0,1,1,1,1),
(49262,56411,0,0,0,1,1,1,1),
(49262,56408,0,0,0,1,1,1,1),
(49262,56409,0,0,0,1,1,1,1);

UPDATE  `smart_scripts` SET `link`=0 WHERE `entryorguid`= 39946 AND `source_type`=0 AND `id`=1;
