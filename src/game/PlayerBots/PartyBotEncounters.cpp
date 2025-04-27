#include "ScriptedInstance.h"
#include "Encounters/MoltenCore.h"

bool PartyBotEncounters::m_overrideMeleePosition = false;
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

    if (!IsEncounterInProgress(pBot))
        return true;

    switch (pBot->me->GetMap()->GetId())
    {
        case MAP_ID_MOLTEN_CORE:
        {
            return PartyBotEncounters_MC::HandleEncounterAI(pBot);
        }
    }

    return true;
}
