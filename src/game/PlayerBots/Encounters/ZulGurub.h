#include "PartyBotAI.h"
#include "PartyBotEncounters.h"

#define UNSTABLE_CONCOCTION 24024
#define MANDOKIR_WATCH 24314


enum EncounterIds_ZG
{
    TYPE_VENOXIS   = 0,
    TYPE_JEKLIK    = 1,
    TYPE_MARLI     = 2,
    TYPE_THEKAL    = 3,
    TYPE_ARLOKK    = 4,
    TYPE_HAKKAR    = 5,
    TYPE_MADNESS   = 6,
    TYPE_MANDOKIR  = 7,
    TYPE_GAHZRANKA = 8,
    TYPE_JINDO     = 9
};
    
class PartyBotEncounters_ZG : public PartyBotEncounters
{
public:
    static bool HandleEncounterAI(PartyBotAI* pBot);
    
    static bool VenoxisEncounter(PartyBotAI* pBot);
    static bool JeklikEncounter(PartyBotAI* pBot);
    static bool MarliEncounter(PartyBotAI* pBot);
    static bool ThekalEncounter(PartyBotAI* pBot);
    static bool ArlokkEncounter(PartyBotAI* pBot);
    static bool HakkarEncounter(PartyBotAI* pBot);
    static bool MadnessEncounter(PartyBotAI* pBot);
    static bool MandokirEncounter(PartyBotAI* pBot);
    static bool GahzrankaEncounter(PartyBotAI* pBot);
    static bool JindoEncounter(PartyBotAI* pBot);
    static bool TrashEncounter(PartyBotAI* pBot);
};
