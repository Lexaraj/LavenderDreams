#include "ZulGurub.h"
#include "Spells/SpellAuras.h"
#include "Spells/Spell.h"
#include "PlayerBots/PartyBotChat.h"



void PartyBotEncounters_ZG::ResetEncounterVars()
{
    m_overrideMeleePosition = false;
    m_overrideRangedPosition = false;
    m_overrideMagicDispel = false;
}

bool PartyBotEncounters_ZG::HandleEncounterAI(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance)
        return true;

    uint32 venoxisState = pInstance->GetData(TYPE_VENOXIS);
    uint32 jeklikState = pInstance->GetData(TYPE_JEKLIK);
    uint32 marliState = pInstance->GetData(TYPE_MARLI);
    uint32 thekalState = pInstance->GetData(TYPE_THEKAL);
    uint32 arlokkState = pInstance->GetData(TYPE_ARLOKK);
    uint32 hakkarState = pInstance->GetData(TYPE_HAKKAR);
    uint32 madnessState = pInstance->GetData(TYPE_MADNESS);
    uint32 mandokirState = pInstance->GetData(TYPE_MANDOKIR);
    uint32 gahzrankaState = pInstance->GetData(TYPE_GAHZRANKA);
    uint32 jindoState = pInstance->GetData(TYPE_JINDO);

    // Check if any encounter is in progress
    bool anyEncounterInProgress = 
        (venoxisState == IN_PROGRESS) ||
        (jeklikState == IN_PROGRESS) ||
        (marliState == IN_PROGRESS) ||
        (thekalState == IN_PROGRESS) ||
        (arlokkState == IN_PROGRESS) ||
        (hakkarState == IN_PROGRESS) ||
        (madnessState == IN_PROGRESS) ||
        (mandokirState == IN_PROGRESS) ||
        (gahzrankaState == IN_PROGRESS) ||
        (jindoState == IN_PROGRESS);

    if (!anyEncounterInProgress && !pBot->me->IsInCombat())
        ResetEncounterVars();
    
    if (venoxisState == IN_PROGRESS)
    {
        return VenoxisEncounter(pBot);
    }
    else if (jeklikState == IN_PROGRESS)
    {
        return JeklikEncounter(pBot);
    }
    else if (marliState == IN_PROGRESS)
    {
        return MarliEncounter(pBot);
    }
    else if (thekalState == IN_PROGRESS)
    {
        return ThekalEncounter(pBot);
    }
    else if (arlokkState == IN_PROGRESS)
    {
        return ArlokkEncounter(pBot);
    }
    else if (hakkarState == IN_PROGRESS)
    {
        return HakkarEncounter(pBot);
    }
    else if (madnessState == IN_PROGRESS)
    {
        return MadnessEncounter(pBot);
    }
    else if (mandokirState == IN_PROGRESS)
    {
        return MandokirEncounter(pBot);
    }
    else if (gahzrankaState == IN_PROGRESS)
    {
        return GahzrankaEncounter(pBot);
    }
    else if (jindoState == IN_PROGRESS)
    {
        return JindoEncounter(pBot);
    }
    else if (pBot->me->IsInCombat())
    {
        return TrashEncounter(pBot);
    }

    return true;
}

bool PartyBotEncounters_ZG::TrashEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // Check for nearby enemies
    std::list<Unit*> enemies;
    pPlayer->GetEnemyListInRadiusAround(pPlayer, 15.0f, enemies);
    
    for (Unit* enemy : enemies)
    {
        if (!enemy || !enemy->IsAlive())
            continue;
            
        // Check if this is a Gurubashi Bat Rider
        if (enemy->GetEntry() == 14750)
        {
            // Check if the bat rider is casting
            if (enemy->IsNonMeleeSpellCasted(false))
            {
                // Make the bot run away from the bat rider
                if (pBot->RunAwayFromTarget(enemy))
                {
                    pBot->SendPartyChat("OH FUCK IT'S GONNA BLOW!");
                    return false;
                }
                
            }
        }
    }

    return true;
}

bool PartyBotEncounters_ZG::VenoxisEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::JeklikEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::MarliEncounter(PartyBotAI* pBot)
{
    // TODO: make bots hard swap to the spawned spiderlings
    return true;
}

bool PartyBotEncounters_ZG::ThekalEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::ArlokkEncounter(PartyBotAI* pBot)
{
    // TODO: make bots hard swap to the spawned panthers
    return true;
}

bool PartyBotEncounters_ZG::HakkarEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::MadnessEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::MandokirEncounter(PartyBotAI* pBot)
{
    Player* pPlayer = pBot->me;
    if (!pPlayer)
        return true;

    // When being watched, do nothing - no movement, no attacks, no spells
    if (pPlayer->HasAura(MANDOKIR_WATCH))
    {
        pBot->SetStaying(true);
        
        if (pPlayer->GetVictim())
            pPlayer->AttackStop();
        
        if (pPlayer->IsNonMeleeSpellCasted(false))
            pPlayer->InterruptNonMeleeSpells(false);

        return false;
    }
    else
    {
        if (pBot->IsStaying())
        {
            pBot->SetStaying(false);

            if (pBot->GetRole() == ROLE_HEALER)
            {
                pBot->FindAndHealInjuredAlly();
            }
            else
            {
                Unit* pTarget = pPlayer->GetVictim();
                if (!pTarget)
                    pTarget = pBot->SelectPartyAttackTarget();

                if (pTarget)
                    pBot->AttackStart(pTarget);
                else
                    pBot->SendPartyChat("I'm a stupid ass bitch");
            }
        }
    }

    return true;
}

bool PartyBotEncounters_ZG::GahzrankaEncounter(PartyBotAI* pBot)
{
    return true;
}

bool PartyBotEncounters_ZG::JindoEncounter(PartyBotAI* pBot)
{
    return true;
} 