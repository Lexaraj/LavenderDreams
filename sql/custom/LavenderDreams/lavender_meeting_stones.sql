-- Lavender Dreams - Meeting Stones

INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `Probability0`, `BroadcastTextID1`, `Probability1`, `BroadcastTextID2`, `Probability2`, `BroadcastTextID3`, `Probability3`, `BroadcastTextID4`, `Probability4`, `BroadcastTextID5`, `Probability5`, `BroadcastTextID6`, `Probability6`, `BroadcastTextID7`, `Probability7`) VALUES (85000, 85000, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
INSERT INTO `broadcast_text` (`entry`, `male_text`, `female_text`) VALUES (85000, 'Hello $N, what brings you here?', 'Hello $N, what brings you here?');


-- Blackrock Depths -> Molten Core
INSERT INTO `gossip_menu` (`entry`, `text_id`, `script_id`, `condition_id`) VALUES (8501, 85000, 0, 0);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_broadcast_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `box_broadcast_text`, `condition_id`) VALUES (8501, 0, 5, 'Clear raid lockout for Molten Core', 0, 1, 1, 0, 0, 0, 0, 0, NULL, 0, 0);
UPDATE `gameobject_template` SET `type` = 2, `data0` = 0, `data1` = 0, `data2` = 3, `data3` = 8501, `data4` = 1, `script_name` = 'lavender_stone' WHERE `entry` = 179584;

-- Blackrock Spire -> Blackwing Lair
INSERT INTO `gossip_menu` (`entry`, `text_id`, `script_id`, `condition_id`) VALUES (8502, 85000, 0, 0);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_broadcast_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `box_broadcast_text`, `condition_id`) VALUES (8502, 0, 5, 'Clear raid lockout for Blackwing Lair', 0, 1, 1, 0, 0, 0, 0, 0, NULL, 0, 0);
UPDATE `gameobject_template` SET `type` = 2, `data0` = 0, `data1` = 0, `data2` = 3, `data3` = 8502, `data4` = 1, `script_name` = 'lavender_stone' WHERE `entry` = 179585;