INSERT INTO creature_template VALUES(41, 0, 19645, 0, 19645, 0, "Teleporteur ethere", "Event WoW-Mania", NULL, 70, 70, 5000, 5000, 5000, 5000, 1, 35, 35, 1, 1, 1, 1, 500, 500, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 5, 5, 5, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, "", 0, 3, 0, 1, 0, 0, 2, "npc_teleporter");
INSERT INTO creature_template VALUES(42, 0, 18783, 0, 18783, 0, "Arrivee du teleporteur (Horde)", "Event WoW-Mania", NULL, 70, 70, 5000, 5000, 5000, 5000, 1, 35, 35, 1, 1, 1, 1, 500, 500, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 5, 5, 5, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, "", 0, 3, 0, 1, 0, 0, 130, "");
INSERT INTO creature_template VALUES(44, 0, 18783, 0, 18783, 0, "Arrivee du teleporteur (Alliance)", "Event WoW-Mania", NULL, 70, 70, 5000, 5000, 5000, 5000, 1, 35, 35, 1, 1, 1, 1, 500, 500, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 5, 5, 5, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, "", 0, 3, 0, 1, 0, 0, 130, "");
UPDATE creature_template SET npcflag = 0 WHERE entry IN (42, 44);