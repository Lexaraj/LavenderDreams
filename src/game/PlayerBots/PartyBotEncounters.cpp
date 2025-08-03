#include "ScriptedInstance.h"
#include "Encounters/MoltenCore.h"
#include "Encounters/ZulGurub.h"

bool PartyBotEncounters::m_overrideMeleePosition = false;
bool PartyBotEncounters::m_overrideRangedPosition = false;
bool PartyBotEncounters::m_overrideMagicDispel = false;

bool PartyBotEncounters::IsEncounterInProgress(PartyBotAI* pBot)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pBot->me->GetInstanceData();
    if (!pInstance)
        return false;

    return pInstance->IsEncounterInProgress();
}


// Handle the encounter AI for the given map
// Return true if the UpdateAI loop should continue, false if it should stop
bool PartyBotEncounters::HandleEncounterAI(PartyBotAI* pBot)
{
    if (!pBot || !pBot->me)
        return true;

    switch (pBot->me->GetMap()->GetId())
    {
        case MAP_ID_MOLTEN_CORE:
        {
            return PartyBotEncounters_MC::HandleEncounterAI(pBot);
        }
        case MAP_ID_ZUL_GURUB:
        {
            return PartyBotEncounters_ZG::HandleEncounterAI(pBot);
        }
    }

    return true;
}

bool PartyBotEncounters::DispelMagic(PartyBotAI* pCaster, Unit* pTarget)
{
    if (!pCaster || !pTarget)
        return false;

    if (!pCaster->me->IsAlive())
        return false;

    uint8 pClass = pCaster->me->GetClass();
    if (pClass != CLASS_PRIEST && pClass != CLASS_PALADIN)
        return false;

    uint32 spellId = (pClass == CLASS_PRIEST) ? SPELL_DISPEL_MAGIC : SPELL_CLEANSE;
    std::string spellName = (pClass == CLASS_PRIEST) ? "Dispel Magic" : "Cleanse";
    SpellEntry const* pSpell = sSpellMgr.GetSpellEntry(spellId);
    if (Unit* pFriend = pCaster->SelectDispelTarget(pSpell))
    {
        if (pCaster->CanTryToCastSpell(pFriend, pSpell))
        {
            if (pCaster->DoCastSpell(pFriend, pSpell) == SPELL_CAST_OK)
            {
                pCaster->SendPartyChat(std::string(spellName).append(" cast on ").append(pFriend->GetName()).c_str());
                return true;
            }
        }
    }
    
    return false;
}
