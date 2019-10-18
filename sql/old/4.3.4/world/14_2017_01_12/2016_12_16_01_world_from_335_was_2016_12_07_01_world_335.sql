/*
SET @OGUID:=9709;
SET @FourthOfJulyEvent:=72;
SET @NewYearEvent:=6;

DELETE FROM `game_event` WHERE `eventEntry`=@FourthOfJulyEvent;
INSERT INTO `game_event` (`eventEntry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`) VALUES
(@FourthOfJulyEvent, '2010-07-04 10:00:00', '2020-07-04 10:00:00', 525600, 1440, 62, 'Fireworks Spectacular');

UPDATE `gameobject_template` SET `ScriptName`='go_cheer_speaker' WHERE `entry`=180749;

DELETE FROM `gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+33;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(@OGUID+0 , 180749, 0, 0, 0, 1, 1, -14370.99, 420.3124, 16.51293, 4.398232, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 1759) -- Cheer Speaker
(@OGUID+1 , 180749, 0, 0, 0, 1, 1, -14306.81, 508.3444, 8.637923, 2.530723, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 1759)
(@OGUID+2 , 180749, 0, 0, 0, 1, 1, -14420.4, 519.201, 5.00313, 6.265733, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 1759)
(@OGUID+3 , 180749, 0, 0, 0, 1, 1, -14465.18, 457.3802, 15.16439, 4.71239, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 1759)
(@OGUID+4 , 180749, 530, 0, 0, 1, 1, 9344.27, -7232.614, 12.6885, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3487)
(@OGUID+5 , 180749, 530, 0, 0, 1, 1, 9389.193, -7278.438, 14.24046, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3487)
(@OGUID+6 , 180749, 530, 0, 0, 1, 1, 9347.431, -7311.366, 14.84726, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3487)
(@OGUID+7 , 180749, 530, 0, 0, 1, 1, 9303.132, -7282.21, 14.98293, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3487)
(@OGUID+8 , 180749, 1, 0, 0, 1, 1, 1503.283, -4409.271, 22.16975, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 5170)
(@OGUID+9 , 180749, 1, 0, 0, 1, 1, 1436.682, -4422.708, 25.45605, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 5170)
(@OGUID+10, 180749, 1, 0, 0, 1, 1, 1327.655, -4380.633, 26.21982, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 4982)
(@OGUID+11, 180749, 1, 0, 0, 1, 1, 1266.912, -4402.42, 26.21174, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 4982)
(@OGUID+12, 180749, 0, 0, 0, 1, 1, 1831.196, 270.56, 59.91178, 5.183629, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 85)
(@OGUID+13, 180749, 0, 0, 0, 1, 1, 1897.282, 225.292, 55.46832, 2.91469, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 85)
(@OGUID+14, 180749, 0, 0, 0, 1, 1, 1786.145, 208.0461, 59.81123, 5.218536, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 85)
(@OGUID+15, 180749, 0, 0, 0, 1, 1, 1932.849, 256.6353, 46.37667, 2.408554, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 85)
(@OGUID+16, 180749, 1, 0, 0, 1, 1, 8590.172, 1013.607, 5.256284, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 702)
(@OGUID+17, 180749, 1, 0, 0, 1, 1, 8648.091, 860.5833, -137.8639, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 702)
(@OGUID+18, 180749, 1, 0, 0, 1, 1, 8698.927, 1020.247, -1.065949, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 702)
(@OGUID+19, 180749, 1, 0, 0, 1, 1, 8734.993, 942.0018, -1.033469, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 702)
(@OGUID+20, 180749, 530, 0, 0, 1, 1, -4002.936, -11879.86, -0.793618, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3557)
(@OGUID+21, 180749, 530, 0, 0, 1, 1, -4064.482, -11883.77, -1.251729, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3557)
(@OGUID+22, 180749, 530, 0, 0, 1, 1, -4054.23, -11952.88, 0.597575, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3557)
(@OGUID+23, 180749, 530, 0, 0, 1, 1, -3966.142, -11945.27, -1.621891, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 3557)
(@OGUID+24, 180749, 1, 0, 0, 1, 1, -1096.49, 104.1394, 141.7214, 2.91469, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 2197)
(@OGUID+25, 180749, 1, 0, 0, 1, 1, -1085.63, -43.56782, 141.4417, 2.408554, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 2197)
(@OGUID+26, 180749, 0, 0, 0, 1, 1, -5079.372, -735.1215, 473.224, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 131)
(@OGUID+27, 180749, 0, 0, 0, 1, 1, -5169.606, -870.4445, 506.4104, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 131)
(@OGUID+28, 180749, 0, 0, 0, 1, 1, -5133.856, -836.9531, 507.1588, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 131)
(@OGUID+29, 180749, 0, 0, 0, 1, 1, -5061.806, -816.5382, 495.3532, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 0)
(@OGUID+30, 180749, 0, 0, 0, 1, 1, -8862.081, 653.8984, 96.19956, 5.340709, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 5148)
(@OGUID+31, 180749, 0, 0, 0, 1, 1, -8862.221, 583.0981, 93.54657, 5.445428, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 5148)
(@OGUID+32, 180749, 0, 0, 0, 1, 1, -8983.388, 552.9216, 93.85145, 5.846854, 0, 0, 0, 1, 120, 255, 1, 21742), -- 180749 (Area: 5148)
(@OGUID+33, 180749, 0, 0, 0, 1, 1, -9044.079, 412.5068, 120.3821, 3.892087, 0, 0, 0, 1, 120, 255, 1, 21742); -- 180749 (Area: 5148)

DELETE FROM `game_event_gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+33 AND `eventEntry`=@FourthOfJulyEvent;
INSERT INTO `game_event_gameobject` SELECT @FourthOfJulyEvent, gameobject.guid FROM `gameobject` WHERE gameobject.guid BETWEEN @OGUID+0 AND @OGUID+33;
DELETE FROM `game_event_gameobject` WHERE `guid` BETWEEN @OGUID+0 AND @OGUID+33 AND `eventEntry`=@NewYearEvent;
INSERT INTO `game_event_gameobject` SELECT @NewYearEvent, gameobject.guid FROM `gameobject` WHERE gameobject.guid BETWEEN @OGUID+0 AND @OGUID+33;
*/
