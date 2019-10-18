/*
-- 
DELETE FROM `pool_template` WHERE entry = 1004;
INSERT INTO `pool_template` (`entry`, `max_limit`, `description`) VALUES
(1004, 1, 'Leprithus (572)');

DELETE FROM `pool_creature` WHERE `pool_entry` IN (1004);
INSERT INTO `pool_creature` (`guid`, `pool_entry`, `chance`, `description`) VALUES
(28458, 1004, 0, 'Leprithus (572) - Spawn 1'),
(86825, 1004, 0, 'Leprithus (572) - Spawn 2'),
(86826, 1004, 0, 'Leprithus (572) - Spawn 3');

DELETE FROM `creature` WHERE `guid` IN (28458, 134020,134021,134022,134023, 86825,86826,86827,86828,86829,86830,86831,86837,86838,86839,86840,86841,86842,86844);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES
(28458, 572, 0, 1, 1, 0, 0, -10084.3, 1558.13, 41.064, 3.65463, 72000, 5, 0, 0, 0, 1),
(86825, 572, 0, 1, 1, 0, 0, -11277.3, 1021.93, 94.7337, 5.18302, 72000, 0, 0, 0, 0, 2),
(86826, 572, 0, 1, 1, 0, 0, -10021.02, 1422.169, 41.042, 2.939070, 72000, 5, 0, 0, 0, 1),
(86827, 846, 0, 1, 1, 0, 0, -10084.3, 1559.49, 40.8883, 3.00608, 300, 5, 0, 0, 0, 1),
(86828, 846, 0, 1, 1, 0, 0, -10082.4, 1556.31, 41.0149, 2.17749, 300, 5, 0, 0, 0, 1),
(86829, 846, 0, 1, 1, 0, 0, -11281.5, 1017.69, 94.4474, 5.30334, 300, 5, 0, 0, 0, 1),
(86830, 846, 0, 1, 1, 0, 0, -11275.7, 1022.68, 94.7905, 5.70782, 300, 5, 0, 0, 0, 1),
(86831, 846, 0, 1, 1, 0, 0, -10988.2, 1600.10, 45.6427, 5.37523, 300, 5, 0, 0, 0, 1),
(86837, 846, 0, 1, 1, 0, 0, -10958.9, 1603.63, 47.8132, 1.78282, 300, 5, 0, 0, 0, 1),
(86838, 846, 0, 1, 1, 0, 0, -10976.4, 1610.50, 46.0335, 1.98000, 300, 5, 0, 0, 0, 1),
(86839, 846, 0, 1, 1, 0, 0, -10990.2, 1623.08, 45.1017, 4.46040, 300, 5, 0, 0, 0, 1),
(86840, 846, 0, 1, 1, 0, 0, -10962.3, 1625.83, 46.4382, 3.24442, 300, 5, 0, 0, 0, 1),
(86841, 846, 0, 1, 1, 0, 0, -10015.233, 1424.467, 40.940, 3.276005, 300, 5, 0, 0, 0, 1),
(86842, 846, 0, 1, 1, 0, 0, -10025.086, 1419.180, 41.218, 2.727011, 300, 5, 0, 0, 0, 1),
(86844, 846, 0, 1, 1, 0, 0, -10025.003, 1427.107, 41.107, 1.611750, 300, 5, 0, 0, 0, 1);

DELETE FROM `creature_addon` WHERE `guid`=86825;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`) VALUES
(86825,868250,1);
 
DELETE FROM `waypoint_data` WHERE `id`=868250;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(868250, 1, -11257.1, 988.193, 83.3336, 5.69163, 0, 0, 0, 100, 0),
(868250, 2, -11244.4, 962.304, 83.269, 5.05783, 0, 0, 0, 100, 0),
(868250, 3, -11240.5, 948.932, 75.6219, 5.30995, 0, 0, 0, 100, 0),
(868250, 4, -11212.6, 921.358, 54.3906, 5.10103, 0, 0, 0, 100, 0),
(868250, 5, -11194.6, 895.302, 48.4349, 5.39948, 0, 0, 0, 100, 0),
(868250, 6, -11172.6, 876.756, 41.3173, 6.03015, 0, 0, 0, 100, 0),
(868250, 7, -11146.3, 866.02, 38.4824, 5.63196, 0, 0, 0, 100, 0),
(868250, 8, -11126, 851.423, 40.3828, 5.64138, 0, 0, 0, 100, 0),
(868250, 9, -11086.8, 834.864, 40.7715, 0.273191, 0, 0, 0, 100, 0),
(868250, 10, -11059.5, 846.814, 38.6569, 6.25714, 0, 0, 0, 100, 0),
(868250, 11, -11039.1, 843.255, 36.3932, 5.48039, 0, 0, 0, 100, 0),
(868250, 12, -11012.4, 812.002, 37.4404, 5.31939, 0, 0, 0, 100, 0),
(868250, 13, -11002.7, 788.433, 36.4366, 5.5715, 0, 0, 0, 100, 0),
(868250, 14, -10982.7, 761.666, 43.9919, 5.11518, 0, 0, 0, 100, 0),
(868250, 15, -10962.6, 736.809, 46.4806, 6.21378, 0, 0, 0, 100, 0),
(868250, 16, -10913.9, 722.066, 42.727, 5.94361, 60000, 0, 0, 100, 0),
(868250, 17, -10913.9, 722.066, 42.727, 5.94361, 2, 0, 0, 100, 0),
(868250, 18, -10913.9, 722.066, 42.727, 5.94361, 60000, 0, 0, 100, 0),
(868250, 19, -10962.6, 736.809, 46.4806, 6.21378, 0, 0, 0, 100, 0),
(868250, 20, -10982.7, 761.666, 43.9919, 5.11518, 0, 0, 0, 100, 0),
(868250, 21, -11002.7, 788.433, 36.4366, 5.5715, 0, 0, 0, 100, 0),
(868250, 22, -11012.4, 812.002, 37.4404, 5.31939, 0, 0, 0, 100, 0),
(868250, 23, -11039.1, 843.255, 36.3932, 5.48039, 0, 0, 0, 100, 0),
(868250, 24, -11059.5, 846.814, 38.6569, 6.25714, 0, 0, 0, 100, 0),
(868250, 25, -11086.8, 834.864, 40.7715, 0.273191, 0, 0, 0, 100, 0),
(868250, 26, -11126, 851.423, 40.3828, 5.64138, 0, 0, 0, 100, 0),
(868250, 27, -11146.3, 866.02, 38.4824, 5.63196, 0, 0, 0, 100, 0),
(868250, 28, -11172.6, 876.756, 41.3173, 6.03015, 0, 0, 0, 100, 0),
(868250, 29, -11194.6, 895.302, 48.4349, 5.39948, 0, 0, 0, 100, 0),
(868250, 30, -11212.6, 921.358, 54.3906, 5.10103, 0, 0, 0, 100, 0),
(868250, 31, -11240.5, 948.932, 75.6219, 5.30995, 0, 0, 0, 100, 0),
(868250, 32, -11244.4, 962.304, 83.269, 5.05783, 0, 0, 0, 100, 0),
(868250, 33, -11257.1, 988.193, 83.3336, 5.69163, 0, 0, 0, 100, 0);
 
DELETE FROM `game_event_creature` WHERE `guid` IN (28458, 86825,86826,86827,86828,86829,86830,86831,86837,86838,86839,86840,90280,52595,52596,52594,90279,86841,86842,86844,90128,89879) AND `eventEntry` IN (-25, 25);
INSERT INTO `game_event_creature` (`eventEntry`, `guid`) VALUES
(-25,90280),
(-25,52595),
(-25,52596),
(-25,52594),
(-25,90279),
(25,28458),
(25,86825),
(25,86826),
(25,86827),
(25,86828),
(25,86829),
(25,86830),
(25,86831),
(25,86837),
(25,86838),
(25,86839),
(25,86840),
(25,86841),
(25,86842),
(25,86844),
(-25,90128),
(-25,89879);
*/
