--
DELETE FROM `gameobject_addon` WHERE `guid` = 23450;
DELETE FROM `gameobject_addon` WHERE `guid` BETWEEN 235364 AND 235372;
INSERT INTO `gameobject_addon` (`guid`, `parent_rotation0`, `parent_rotation1`, `parent_rotation2`, `parent_rotation3`, `invisibilityType`, `invisibilityValue`) VALUES
('235364', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235365', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235366', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235367', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235368', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235369', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235370', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235371', '0', '0', '-0.709134', '0.705074', '0', '0'),
('235372', '0', '0', '-0.709134', '0.705074', '0', '0');
