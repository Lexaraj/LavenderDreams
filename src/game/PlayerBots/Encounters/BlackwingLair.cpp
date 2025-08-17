#include "BlackwingLair.h"
#include "PlayerBots/PartyBotChat.h"
#include "AI/ScriptedInstance.h"
#include "Group/Group.h"


void PartyBotEncounters_BWL::ResetEncounterVars()
{
    PartyBotEncounters::ResetEncounterVars();
}


bool PartyBotEncounters_BWL::HandleEncounterAI(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    uint32 razorgoreState = pInstance->GetData(TYPE_RAZORGORE);
    uint32 vaelastraszState = pInstance->GetData(TYPE_VAELASTRASZ);
    uint32 broodlordState = pInstance->GetData(TYPE_BROODLORD);
    uint32 firemawState = pInstance->GetData(TYPE_FIREMAW);
    uint32 ebonrocState = pInstance->GetData(TYPE_EBONROC);
    uint32 flamegorState = pInstance->GetData(TYPE_FLAMEGOR);
    uint32 chromaggusState = pInstance->GetData(TYPE_CHROMAGGUS);
    uint32 nefarianState = pInstance->GetData(TYPE_NEFARIAN);

    // Check if any encounter is in progress
    bool anyEncounterInProgress = 
        (razorgoreState == IN_PROGRESS) ||
        (vaelastraszState == IN_PROGRESS) ||
        (broodlordState == IN_PROGRESS) ||
        (firemawState == IN_PROGRESS) ||
        (ebonrocState == IN_PROGRESS) ||
        (flamegorState == IN_PROGRESS) ||
        (chromaggusState == IN_PROGRESS) ||
        (nefarianState == IN_PROGRESS);

    if (!anyEncounterInProgress && !pBot->me->IsInCombat())
        ResetEncounterVars();
    
    if (razorgoreState == IN_PROGRESS)
    {
        return RazorgoreEncounter(pBot);
    }
    else if (vaelastraszState == IN_PROGRESS)
    {
        return VaelastraszEncounter(pBot);
    }
    else if (broodlordState == IN_PROGRESS)
    {
        return BroodlordEncounter(pBot);
    }
    else if (firemawState == IN_PROGRESS)
    {
        return FiremawEncounter(pBot);
    }
    else if (ebonrocState == IN_PROGRESS)
    {
        return EbonrocEncounter(pBot);
    }
    else if (flamegorState == IN_PROGRESS)
    {
        return FlamegorEncounter(pBot);
    }
    else if (chromaggusState == IN_PROGRESS)
    {
        return ChromaggusEncounter(pBot);
    }
    else if (nefarianState == IN_PROGRESS)
    {
        return NefarianEncounter(pBot);
    }
    else if (pBot->me->IsInCombat())
    {
        return TrashEncounter(pBot);
    }

    return true;
}


bool PartyBotEncounters_BWL::TrashEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::RazorgoreEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;
    
    if (pInstance->GetData(RAZORGORE_EGGS_DESTROYED) >= 15)
        return true;

    if (Unit* pTarget = pPlayer->GetVictim())
        return true;

    // Define focus mobs within 100 yards
    std::list<Creature*> mages;
    std::list<Creature*> legionnaires;
    std::list<Creature*> dragonspawns;
    pPlayer->GetCreatureListWithEntryInGrid(mages, BLACKWING_MAGE, 100.0f);
    pPlayer->GetCreatureListWithEntryInGrid(legionnaires, BLACKWING_LEGIONNAIRE, 100.0f);
    pPlayer->GetCreatureListWithEntryInGrid(dragonspawns, DEATH_TALON_DRAGONSPAWN, 100.0f);
    
    // Ignore dead mobs
    mages.remove_if([](Creature* creature) { return !creature || !creature->IsAlive(); });
    legionnaires.remove_if([](Creature* creature) { return !creature || !creature->IsAlive(); });
    dragonspawns.remove_if([](Creature* creature) { return !creature || !creature->IsAlive(); });
    
    if (!mages.empty() || !legionnaires.empty() || !dragonspawns.empty())
    {
        bool assist = false;
        if (Group* pGroup = pPlayer->GetGroup())
        {
            // Check if any group member is already targeting a focus mob
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* member = itr->getSource())
                {
                    if (member != pPlayer && member->IsAlive())
                    {
                        if (Unit* memberTarget = member->GetVictim())
                        {
                            if ((memberTarget->GetEntry() == BLACKWING_MAGE || 
                                 memberTarget->GetEntry() == BLACKWING_LEGIONNAIRE ||
                                 memberTarget->GetEntry() == DEATH_TALON_DRAGONSPAWN) && 
                                 memberTarget->IsAlive() &&
                                 pPlayer->GetVictim() != memberTarget)
                            {
                                pPlayer->Attack(memberTarget, pBot->GetRole() == ROLE_MELEE_DPS);
                                assist = true;
                                break;
                            }
                        }
                    }
                }
            }

            // If no one is targeting a focus mob, pick the first available one
            if (!assist)
            {
                Creature* target = nullptr;
                if (!mages.empty())
                    target = mages.front();
                else if (!legionnaires.empty())
                    target = legionnaires.front();
                else if (!dragonspawns.empty())
                    target = dragonspawns.front();
                
                if (target && pPlayer->GetVictim() != target)
                {
                    pBot->SendPartyChat(("Focusing on " + std::string(target->GetName()) + "!").c_str());
                    pPlayer->Attack(target, pBot->GetRole() == ROLE_MELEE_DPS);
                }
            }
        }
    }

    return true;
}


bool PartyBotEncounters_BWL::VaelastraszEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::BroodlordEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::FiremawEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::EbonrocEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::FlamegorEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::ChromaggusEncounter(PartyBotAI* pBot)
{
    return true;
}


bool PartyBotEncounters_BWL::NefarianEncounter(PartyBotAI* pBot)
{
    return true;
}
