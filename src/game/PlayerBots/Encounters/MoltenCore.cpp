#include "MoltenCore.h"
#include "Spells/SpellAuras.h"
#include "PlayerBots/PartyBotChat.h"

// General
uint32 PartyBotEncounters_MC::LastInsultedTime = 0;

// Baron Geddon
bool PartyBotEncounters_MC::AnnouncedLivingBomb = false;
bool PartyBotEncounters_MC::BaronGeddonInferno = false;
Player* PartyBotEncounters_MC::LivingBombTarget = nullptr;


void PartyBotEncounters_MC::ResetEncounterVars()
{
    PartyBotEncounters::ResetEncounterVars();
    AnnouncedLivingBomb = false;
    LivingBombTarget = nullptr;
    LastInsultedTime = 0;
}


bool PartyBotEncounters_MC::HandleEncounterAI(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    uint32 lucifronState  = pInstance->GetData(TYPE_LUCIFRON);
    uint32 magmadarState  = pInstance->GetData(TYPE_MAGMADAR);
    uint32 gehennasState  = pInstance->GetData(TYPE_GEHENNAS);
    uint32 garrState      = pInstance->GetData(TYPE_GARR);
    uint32 geddonState    = pInstance->GetData(TYPE_GEDDON);
    uint32 shazzrahState  = pInstance->GetData(TYPE_SHAZZRAH);
    uint32 sulfuronState  = pInstance->GetData(TYPE_SULFURON);
    uint32 golemaggState  = pInstance->GetData(TYPE_GOLEMAGG);
    uint32 majordomoState = pInstance->GetData(TYPE_MAJORDOMO);
    uint32 ragnarosState  = pInstance->GetData(TYPE_RAGNAROS);

    // Check if any encounter is in progress
    bool anyEncounterInProgress = 
        (lucifronState == IN_PROGRESS) ||
        (magmadarState == IN_PROGRESS) ||
        (gehennasState == IN_PROGRESS) ||
        (garrState == IN_PROGRESS) ||
        (geddonState == IN_PROGRESS) ||
        (shazzrahState == IN_PROGRESS) ||
        (sulfuronState == IN_PROGRESS) ||
        (golemaggState == IN_PROGRESS) ||
        (majordomoState == IN_PROGRESS) ||
        (ragnarosState == IN_PROGRESS || ragnarosState == SPECIAL);

    if (!anyEncounterInProgress && !pBot->me->IsInCombat())
        ResetEncounterVars();
    
    if (lucifronState == IN_PROGRESS)
    {
        return LucifronEncounter(pBot);
    }
    else if (magmadarState == IN_PROGRESS)
    {
       return MagmadarEncounter(pBot);
    }
    else if (gehennasState == IN_PROGRESS)
    {
        return GehennasEncounter(pBot);
    }
    else if (garrState == IN_PROGRESS)
    {
        return GarrEncounter(pBot);
    }
    else if (geddonState == IN_PROGRESS)
    {
        return GeddonEncounter(pBot);
    }
    else if (shazzrahState == IN_PROGRESS)
    {
        return ShazzrahEncounter(pBot);
    }
    else if (sulfuronState == IN_PROGRESS)
    {
        return SulfuronEncounter(pBot);
    }
    else if (golemaggState == IN_PROGRESS)
    {
        return GolemaggEncounter(pBot);
    }
    else if (majordomoState == IN_PROGRESS)
    {
        return MajordomoEncounter(pBot);
    }
    else if (ragnarosState == IN_PROGRESS || ragnarosState == SPECIAL)
    {
        return RagnarosEncounter(pBot);
    }
    else if (pBot->me->IsInCombat())
    {
        return TrashEncounter(pBot);
    }

    return true;
}


bool PartyBotEncounters_MC::TrashEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Stack on Lava Surgers
    std::list<Creature*> lavaSurgers;
    pPlayer->GetCreatureListWithEntryInGrid(lavaSurgers, LAVA_SURGER, 40.0f);
    if (!lavaSurgers.empty())
    {
        Creature* surger = lavaSurgers.front();
        if (surger && surger->IsAlive())
        {
            try {
                pPlayer->GetMotionMaster()->Clear();
                pPlayer->GetMotionMaster()->MoveChase(surger, 1.0f);
                pPlayer->SetCasterChaseDistance(1.0f);
                pPlayer->GetMotionMaster()->MoveDistance(surger, 1.0f);
                m_overrideRangedPosition = true;
            }
            catch (std::exception& e) {
                // fail silently
            }
        }
    } else {
        m_overrideRangedPosition = false;
    }

    if (pBot->GetRole() == ROLE_HEALER)
        return true;

    // Focus Lava Spawns
    std::list<Creature*> lavaSpawns;
    pPlayer->GetCreatureListWithEntryInGrid(lavaSpawns, LAVA_SPAWN, 40.0f);
    
    // Filter out dead lava spawns
    lavaSpawns.remove_if([](Creature* spawn) { return !spawn || !spawn->IsAlive(); });
    
    if (!lavaSpawns.empty())
    {
        bool assist = false;
        if (Group* pGroup = pPlayer->GetGroup())
        {
            // First check if any group member is already targeting a lava spawn
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* member = itr->getSource())
                {
                    if (member != pPlayer && member->IsAlive())
                    {
                        if (Unit* memberTarget = member->GetVictim())
                        {
                            if (memberTarget->GetEntry() == LAVA_SPAWN && 
                                memberTarget->IsAlive() &&
                                pPlayer->GetVictim() != memberTarget)
                            {
                                pBot->SendPartyChat(("Assisting " + std::string(member->GetName()) + " on lava spawn").c_str());
                                pPlayer->SetTargetGuid(memberTarget->GetObjectGuid());
                                assist = true;
                                break;
                            }
                        }
                    }
                }
            }

            // If no one is targeting a lava spawn, pick the first available one
            if (!assist)
            {
                Creature* firstAvailableSpawn = lavaSpawns.front();
                if (firstAvailableSpawn && 
                    pPlayer->GetVictim() != firstAvailableSpawn)
                {
                    pBot->SendPartyChat("Focusing on lava spawn!");
                    pPlayer->SetTargetGuid(firstAvailableSpawn->GetObjectGuid());
                }
            }
        }
    }

    return true;
}


bool PartyBotEncounters_MC::LucifronEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Healers dont waste mana on dispels below 40%
    if (pBot->GetRole() == ROLE_HEALER)
    {
        if (pPlayer->GetPower(POWER_MANA) * 100 / pPlayer->GetMaxPower(POWER_MANA) < 40)
        {
            m_overrideMagicDispel = true;
        }
    }

    return true;
}


bool PartyBotEncounters_MC::MagmadarEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    if(pPlayer->IsMoving())
        return false;

    Player* pLeader = pBot->GetPartyLeader();

    // Find all conflagration game objects (177704) within 100 yards
    std::list<GameObject*> conflagObjects;
    pPlayer->GetGameObjectListWithEntryInGrid(conflagObjects, 177704, 100.0f);
    
    Unit* cMagmadar = pPlayer->GetVictim();

    // Check if we're actually standing in a conflagration
    bool inFire = false;
    for (GameObject* pConflag : conflagObjects)
    {
        if (pPlayer->GetDistance2d(pConflag) < 5.0f) // If within 5 yards of a conflagration
        {
            inFire = true;
            break;
        }
    }

    bool onFire = false;
    if (pPlayer->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE))   
        onFire = true;
    
    if (!inFire && !onFire)
        return true;

    uint32 currentTime = WorldTimer::getMSTime();

    // If we're in fire, we need to move away
    if (inFire || (onFire && currentTime - pBot->m_lastMoveTime > 2000))
    {
         // Check again if we're actually standing in a conflagration
        inFire = false;
        for (GameObject* pConflag : conflagObjects)
        {
            if (pPlayer->GetDistance2d(pConflag) < 5.0f)
            {
                inFire = true;
                break;
            }
        }

        if (!inFire)
            return true;

        pBot->SendPartyChat("AHHH FUCK!");
        
        // Find the nearest conflagration
        GameObject* nearestConflag = nullptr;
        float nearestDist = 9999.0f;
        for (GameObject* pConflag : conflagObjects)
        {
            float dist = pPlayer->GetDistance2d(pConflag);
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearestConflag = pConflag;
            }
        }

        if (nearestConflag)
        {
            // Calculate a position away from the nearest conflagration
            float angle = pPlayer->GetAngle(nearestConflag);
            float moveDist = 7.0f; // Move 7 yards away
            
            // Move in the opposite direction of the conflagration
            float newX = pPlayer->GetPositionX() + moveDist * cos(angle + M_PI);
            float newY = pPlayer->GetPositionY() + moveDist * sin(angle + M_PI);
            float newZ = pPlayer->GetPositionZ();
            float bossAngle = pPlayer->GetAngle(cMagmadar);

            // Make sure we're not moving into another conflagration
            bool safeSpot = true;
            for (GameObject* pConflag : conflagObjects)
            {
                if (pConflag != nearestConflag && pConflag->GetDistance2d(newX, newY) < 5.0f)
                {
                    safeSpot = false;
                    break;
                }
            }

            // For melee DPS, ensure we stay within melee range of the boss
            if (safeSpot && cMagmadar && (pBot->GetRole() == ROLE_MELEE_DPS))
            {
                float distToBoss = cMagmadar->GetDistance2d(newX, newY);
                if (distToBoss > 5.0f) // If we'd be too far from boss
                {
                    // Try to find a position that's both away from fire and within melee range
                    float bossAngle = pPlayer->GetAngle(cMagmadar);
                    float meleeDist = cMagmadar->GetCombatReach();
                    
                    // Try positions around the boss
                    for (int i = 0; i < 8; i++)
                    {
                        float testAngle = bossAngle + (i * M_PI / 4); // Try 8 positions around boss
                        float testX = cMagmadar->GetPositionX() + meleeDist * cos(testAngle);
                        float testY = cMagmadar->GetPositionY() + meleeDist * sin(testAngle);
                        
                        // Check if this position is safe from conflagrations
                        bool positionSafe = true;
                        for (GameObject* pConflag : conflagObjects)
                        {
                            if (pConflag->GetDistance2d(testX, testY) < 5.0f)
                            {
                                positionSafe = false;
                                break;
                            }
                        }
                        
                        if (positionSafe)
                        {
                            newX = testX;
                            newY = testY;
                            break;
                        }
                    }
                }
            }

            if (safeSpot)
            {
                if (pBot->SafelyMoveTo(newX, newY, newZ, bossAngle))
                {
                    pBot->SendPartyChat("Moving to safe spot.");
                    pBot->m_lastMoveTime = currentTime;
                    m_overrideMeleePosition = true;
                    return false;
                }
            }
        }

        // If we couldn't find a safe spot or move failed, try to move to a random position
        float randomAngle = frand(0, 2 * M_PI);
        float randomDist = frand(7.0f, 10.0f);
        float randomX = pPlayer->GetPositionX() + randomDist * cos(randomAngle);
        float randomY = pPlayer->GetPositionY() + randomDist * sin(randomAngle);
        float randomZ = pPlayer->GetPositionZ();
        float bossAngle = pPlayer->GetAngle(cMagmadar);

        // For melee DPS, ensure random position is within melee range
        if (cMagmadar && (pBot->GetRole() == ROLE_MELEE_DPS))
        {
            float bossAngle = pPlayer->GetAngle(cMagmadar);
            float meleeDist = cMagmadar->GetCombatReach();
            randomX = cMagmadar->GetPositionX() + meleeDist * cos(bossAngle + frand(-M_PI/4, M_PI/4));
            randomY = cMagmadar->GetPositionY() + meleeDist * sin(bossAngle + frand(-M_PI/4, M_PI/4));
        }

        if (pBot->SafelyMoveTo(randomX, randomY, randomZ, bossAngle))
        {
            pBot->SendPartyChat("Moving to random spot.");
            pBot->m_lastMoveTime = currentTime;
            m_overrideMeleePosition = true;
            return false;
        }

        // If all else fails, try to move to a position behind the boss (for melee)
        if (cMagmadar && (pBot->GetRole() == ROLE_MELEE_DPS))
        {
            float bossAngle = pPlayer->GetAngle(cMagmadar);
            float meleeDist = cMagmadar->GetCombatReach();
            float fleeX = cMagmadar->GetPositionX() + meleeDist * cos(bossAngle + M_PI);
            float fleeY = cMagmadar->GetPositionY() + meleeDist * sin(bossAngle + M_PI);
            float fleeZ = pPlayer->GetPositionZ();

            if (pBot->SafelyMoveTo(fleeX, fleeY, fleeZ, bossAngle))
            {
                pBot->SendPartyChat("Moving to behind boss.");
                pBot->m_lastMoveTime = currentTime;
                m_overrideMeleePosition = true;
                return false;
            }
        }
    }

    return true;
}


bool PartyBotEncounters_MC::GehennasEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    if (Unit* pVictim = pPlayer->GetVictim())
    {
        // Move out of rain of fire
    }

    return true;
}


bool PartyBotEncounters_MC::GarrEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Tell healer to stay away from the adds and not to waste mana on dispels
    if (pBot->GetRole() == ROLE_HEALER)
    {
        m_overrideMagicDispel = true;
    }

    return true;
}


bool PartyBotEncounters_MC::GeddonEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;
    
    if (!pPlayer->IsAlive())
        return true;

    // Healers need to prioritize dispelling Ignite Mana
    uint8 pClass = pPlayer->GetClass();
    if (pClass == CLASS_PRIEST || pClass == CLASS_PALADIN)
    {
        if (pPlayer->HasAura(BARON_IGNITEMANA))
        {
            PartyBotEncounters::DispelMagic(pBot, pPlayer);
        }
        else if (Group* group = pPlayer->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* member = itr->getSource())
                {
                    if (member->IsAlive() && member->HasAura(BARON_IGNITEMANA))
                    {
                        if (member->GetPowerType() == POWER_MANA)
                            PartyBotEncounters::DispelMagic(pBot, member);
                    }
                }
            }
        }
    }

    if (pPlayer->IsMoving() && !BaronGeddonInferno)
        return false;

    Unit* baron = pBot->SelectAttackTarget(pBot->GetPartyLeader());
    
    // Handle living bomb
    if (pPlayer->HasAura(BARON_LIVINGBOMB))
    {
        uint32 timeLeft = pPlayer->GetSpellAuraHolder(BARON_LIVINGBOMB)->GetAuraDuration();
        
        LivingBombTarget = pPlayer;

        if (!AnnouncedLivingBomb)
        {
            pBot->SendPartyChat("Living bomb on me");
            AnnouncedLivingBomb = true;
        }
        // Check for nearby players
        std::list<Player*> nearbyPlayers;
        pPlayer->GetAlivePlayerListInRange(pPlayer, nearbyPlayers, 15.0f);
        nearbyPlayers.remove(pPlayer);
        
        if (!nearbyPlayers.empty() && timeLeft < 5000)
        {
            if (pBot->GetRole() != ROLE_HEALER)
            {
                // Run away from other players
                float baseAngle = pPlayer->GetAngle(nearbyPlayers.front());
                float moveDist = 15.0f;
                
                // Try different angles to find a safe spot
                bool foundSafeSpot = false;
                float currentDist = moveDist;
                int angleIndex = 0;
                do {
                    float testAngle = baseAngle + (angleIndex * M_PI);
                    float newX = pPlayer->GetPositionX() + currentDist * cos(testAngle + M_PI);
                    float newY = pPlayer->GetPositionY() + currentDist * sin(testAngle + M_PI);
                    float newZ = pPlayer->GetPositionZ();

                    // Check if this position is safe from other players
                    bool safeFromPlayers = true;
                    for (Player* nearPlayer : nearbyPlayers)
                    {
                        if (nearPlayer->GetDistance2d(newX, newY) < 15.0f)
                        {
                            safeFromPlayers = false;
                            break;
                        }
                    }

                    // Check if this position is safe from the boss
                    bool safeFromBoss = true;
                    if (baron)
                    {
                        if (baron->GetDistance2d(newX, newY) < 30.0f)
                            safeFromBoss = false;
                    }

                    if (safeFromPlayers && safeFromBoss)
                    {
                        if (pBot->SafelyMoveTo(newX, newY, newZ, testAngle))
                        {
                            uint32 currentTime = WorldTimer::getMSTime();
                            if (currentTime - pBot->m_lastMoveTime > 7000)
                            {
                                pBot->SendPartyChat("Running away from players!");
                                pBot->m_lastMoveTime = currentTime;
                                foundSafeSpot = true;
                                return false;
                            }
                        }
                    }
                    angleIndex++;
                    if (angleIndex >= 359) // After trying all angles at current distance
                    {
                        angleIndex = 0;
                        currentDist += 5.0f; // Increase distance by 5 yards each iteration
                    }
                } while (!foundSafeSpot && currentDist <= 50.0f);

                if (!foundSafeSpot)
                {
                    uint32 currentTime = WorldTimer::getMSTime();
                    if (currentTime - pBot->m_lastMoveTime > 4000)
                    {
                        pBot->SendPartyChat("brb squatting on a breadstick");
                        Player* pDaddy = nearbyPlayers.front();
                        pPlayer->GetMotionMaster()->MoveDistance(pDaddy, 15.0f);
                        pBot->m_lastMoveTime = currentTime;
                        return false;
                    }
                }
            }
            else
            {
                uint32 currentTime = WorldTimer::getMSTime();
                if (currentTime - LastInsultedTime > 5000)
                {
                    std::string insult = PartyBotChat::GetInsult();
                    std::string message = nearbyPlayers.size() > 1 ? 
                        ("Get away from me you " + insult + "s!!") : 
                        (std::string(nearbyPlayers.front()->GetName()) + ", run away from me you " + insult + "!");
                    pBot->SendPartyChat(message.c_str());
                    LastInsultedTime = currentTime;

                    // move the other party bots away
                    for (Player* nearPlayer : nearbyPlayers)
                    {
                        if (nearPlayer->AI() && nearPlayer->GetDistance(pPlayer) < 14.0f)
                        {
                            PartyBotAI* paiNear = dynamic_cast<PartyBotAI*>(nearPlayer->AI());
                            if (paiNear)
                            {
                                std::string response = PartyBotChat::GetInsultResponse();
                                paiNear->SendPartyChat(response.c_str(), urand(1000, 3000));
                                paiNear->m_lastMoveTime = currentTime;
                            }
                            nearPlayer->GetMotionMaster()->MoveDistance(pPlayer, 15.0f);
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (pPlayer == LivingBombTarget)
        {
            AnnouncedLivingBomb = false;
            LivingBombTarget = nullptr;
        }
    }


    // Handle inferno
    if (baron && baron->HasAura(BARON_INFERNO))
    {
        BaronGeddonInferno = true;

        // get the fuck out if we're too close
        if (pPlayer->GetDistance(baron) < 30.0f)
        {
            pPlayer->GetMotionMaster()->MoveDistance(baron, 30.0f);
            return false;
        }

        // dont move if we're waiting for inferno to stop
        if (LivingBombTarget != pPlayer)
        {
            pPlayer->GetMotionMaster()->Clear();
            pPlayer->GetMotionMaster()->MoveIdle();
            return false;
        }
        else
        {
            // Move away from other players since we have living bomb
            std::list<Player*> nearbyPlayers;
            pPlayer->GetAlivePlayerListInRange(pPlayer, nearbyPlayers, 15.0f);
            nearbyPlayers.remove(pPlayer);

            if (!nearbyPlayers.empty())
            {
                pBot->SendPartyChat("Living bomb on me, running away!");
                float baseAngle = pPlayer->GetAngle(nearbyPlayers.front());
                float moveDist = 15.0f;
                float newX = pPlayer->GetPositionX() + moveDist * cos(baseAngle);
                float newY = pPlayer->GetPositionY() + moveDist * sin(baseAngle);
                float newZ = pPlayer->GetPositionZ();
                pPlayer->GetMotionMaster()->Clear();
                pBot->SafelyMoveTo(newX, newY, newZ, pPlayer->GetAngle(baron));
                return false;
            }
        }
    }
    else
    {
        BaronGeddonInferno = false;

        if(baron)
        {
            // run out if we're too close to the boss
            if ((pBot->GetRole() == ROLE_HEALER || pBot->GetRole() == ROLE_RANGE_DPS) &&
                !pBot->IsStaying())
            {
                if (pPlayer->GetDistance(baron) < 25.0f && 
                !pPlayer->IsMoving() && !pPlayer->IsNonMeleeSpellCasted())
                {
                    pPlayer->GetMotionMaster()->MoveDistance(baron, 30.0f);
                    return false;
                }

                // Ensure we're facing the boss
                pPlayer->SetCasterChaseDistance(32.0f);
                pPlayer->GetMotionMaster()->MoveChase(baron, 30.0f, frand(0, M_PI / 2));
                pBot->AttackStart(baron);                
            }
        }
    }

    return true;
}


bool PartyBotEncounters_MC::ShazzrahEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    return true;
}


bool PartyBotEncounters_MC::SulfuronEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    return true;
}


bool PartyBotEncounters_MC::GolemaggEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    return true;
}


bool PartyBotEncounters_MC::MajordomoEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Move out of the hot coals
    if (pPlayer->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE))
    {
        pBot->SendPartyChat("Yikes McGavin that's hot!");
        pPlayer->GetMotionMaster()->MoveDistance(pPlayer, 15.0f);
        return false;
    }

    return true;
}


bool PartyBotEncounters_MC::RagnarosEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Melee dps avoid lava

    // Healers ignore hunter pets

    return true;
} 
