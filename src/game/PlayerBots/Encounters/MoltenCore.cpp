#include "MoltenCore.h"

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#define SPELL_DISPEL_MAGIC 988
#define SPELL_AURA_MOD_DECREASE_HEALTH 4
#define SPELL_AURA_MOD_DECREASE_SPEED 139

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

    if (!anyEncounterInProgress)
    {
        m_overrideMeleePosition = false;
        m_overrideMagicDispel = false;
        return true;
    }
    
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

    if (Unit* pVictim = pPlayer->GetVictim())
    {
        // If bombed, run away from party
        if (pPlayer->HasAuraType(SPELL_AURA_PERIODIC_TRIGGER_SPELL))
        {
            pPlayer->GetMotionMaster()->MoveDistance(pVictim, 30.0f);
            return false;
        }

        if (pBot->GetRole() == ROLE_MELEE_DPS || pBot->GetRole() == ROLE_TANK)
        {
            // Check if we have the Armageddon debuff
            if (pPlayer->HasAuraType(SPELL_AURA_PERIODIC_DAMAGE))
            {
                float angle = pPlayer->GetAngle(pVictim);
                float moveDist = 50.0f; // Move 50 yards away
                float newX = pPlayer->GetPositionX() + moveDist * cos(angle + M_PI);
                float newY = pPlayer->GetPositionY() + moveDist * sin(angle + M_PI);
                float newZ = pPlayer->GetPositionZ();

                if (pBot->SafelyMoveTo(newX, newY, newZ, angle))
                {
                    pBot->SendPartyChat("Running out!");
                    return false;
                }
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
