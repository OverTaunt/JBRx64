-- 
DELETE FROM `creature` WHERE `guid` IN (78807,78808,78809,79283,79284,79285,79368,79378);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `VerifiedBuild`) VALUES
(78807, 6491, 530, 0, 0, 1, 1, 0, 0, 6724.2768, -7935.263, 170.098, 5.784735, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(78808, 6491, 530, 0, 0, 1, 1, 0, 0, -1423.50, 4358.489, 241.566, 5.185481, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(78809, 6491, 530, 0, 0, 1, 1, 0, 0, -2438.868, 4665.473, 161.979, 4.648291, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(79283, 6491, 571, 0, 0, 1, 1, 0, 0, 8466.29, 442.502, 596.0717, 1.601781, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
-- (79284, 6491, 1,   0, 0, 1, 1, 0, 0,   -4555.235, -3600.542, 41.486, 3.899855, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(79285, 6491, 571, 0, 0, 1, 1, 0, 0, 4686.022, 52.123, 73.449, 4.463720, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(79368, 6491, 571, 0, 0, 1, 1, 0, 0, 6445.856, 2062.501, 563.705, 5.488712, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(79378, 6491, 571, 0, 0, 1, 1, 0, 0, 2583.27, -5802.575, 295.615, 3.652452, 72000, 0, 0, 0, 0, 0, 0, 0, 0, 0);
