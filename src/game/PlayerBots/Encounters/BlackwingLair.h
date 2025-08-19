#include "PartyBotAI.h"
#include "PartyBotEncounters.h"

// Razorgore
#define RAZORGORE 12435
#define RAZORGORE_EGGS_DESTROYED 13    // scriptedInstance 
#define DEATH_TALON_DRAGONSPAWN 12422
#define BLACKWING_MAGE 12420
#define BLACKWING_LEGIONNAIRE 12416

// Broodlord trash
#define DEATH_TALON_HATCHER 12468
#define CORRUPTED_GREEN_WHELP  14023
#define CORRUPTED_RED_WHELP    14022
#define CORRUPTED_BLUE_WHELP   14024
#define CORRUPTED_BRONZE_WHELP 14025


enum BWL_EncounterIds
{
    TYPE_RAZORGORE = 0,
    TYPE_VAELASTRASZ = 1,
    TYPE_BROODLORD = 2,
    TYPE_FIREMAW = 3,
    TYPE_EBONROC = 4,
    TYPE_FLAMEGOR = 5,
    TYPE_CHROMAGGUS = 6,
    TYPE_NEFARIAN = 7
};

class PartyBotEncounters_BWL : public PartyBotEncounters
{
public:
    static bool HandleEncounterAI(PartyBotAI* pBot);
    
    static bool RazorgoreEncounter(PartyBotAI* pBot);
    static bool VaelastraszEncounter(PartyBotAI* pBot);
    static bool BroodlordEncounter(PartyBotAI* pBot);
    static bool FiremawEncounter(PartyBotAI* pBot);
    static bool EbonrocEncounter(PartyBotAI* pBot);
    static bool FlamegorEncounter(PartyBotAI* pBot);
    static bool ChromaggusEncounter(PartyBotAI* pBot);
    static bool NefarianEncounter(PartyBotAI* pBot);
    static bool TrashEncounter(PartyBotAI* pBot);

    static void ResetEncounterVars();
};
