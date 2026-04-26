-- Lavender Dreams - Meeting Stones

REPLACE INTO `npc_text` (`ID`, `BroadcastTextID0`, `Probability0`, `BroadcastTextID1`, `Probability1`, `BroadcastTextID2`, `Probability2`, `BroadcastTextID3`, `Probability3`, `BroadcastTextID4`, `Probability4`, `BroadcastTextID5`, `Probability5`, `BroadcastTextID6`, `Probability6`, `BroadcastTextID7`, `Probability7`) VALUES (85000, 85000, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
REPLACE INTO `broadcast_text` (`entry`, `male_text`, `female_text`) VALUES (85000, 'Greetings $N, what brings you here?', 'Greetings $N, what brings you here?');


-- Blackrock Depths -> MC & BWL
UPDATE `gameobject_template` SET `type` = 2, `data0` = 0, `data1` = 0, `data2` = 3, `data3` = 0, `data4` = 1, `script_name` = 'lavender_stone' WHERE `entry` = 179584;
DELETE FROM `gossip_menu` WHERE `entry` = 8501;
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 8501;

-- Remove Blackrock Spire meeting stone
DELETE FROM `gossip_menu` WHERE `entry` = 8502;
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 8502;
DELETE FROM `gameobject_template` WHERE `entry` = 179585;
DELETE FROM `gameobject` WHERE `id` = 179585;