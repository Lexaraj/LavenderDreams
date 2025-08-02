#include "PartyBotAI.h"
#include "PartyBotEncounters.h"

// Trash Priorities
#define LAVA_SURGER 12101
#define LAVA_SPAWN 12265

// Baron Geddon
#define BARON_LIVINGBOMB 20475
#define BARON_INFERNO 19695
#define BARON_IGNITEMANA 19659
#define BARON_ARMAGEDDON 20478

// Gehennas
#define GEHENNAS_CURSE 19716
#define GEHENNAS_RAIN_OF_FIRE 19717

enum EncounterIds
{
    TYPE_SULFURON  = 0,
    TYPE_GEDDON    = 1,
    TYPE_SHAZZRAH  = 2,
    TYPE_GOLEMAGG  = 3,
    TYPE_GARR      = 4,
    TYPE_MAGMADAR  = 5,
    TYPE_GEHENNAS  = 6,
    TYPE_LUCIFRON  = 7,
    TYPE_MAJORDOMO = 8,
    TYPE_RAGNAROS  = 9
};
    
class PartyBotEncounters_MC : public PartyBotEncounters
{
public:
    static bool HandleEncounterAI(PartyBotAI* pBot);
    
    static bool LucifronEncounter(PartyBotAI* pBot);
    static bool MagmadarEncounter(PartyBotAI* pBot);
    static bool GehennasEncounter(PartyBotAI* pBot);
    static bool GarrEncounter(PartyBotAI* pBot);
    static bool GeddonEncounter(PartyBotAI* pBot);
    static bool ShazzrahEncounter(PartyBotAI* pBot);
    static bool SulfuronEncounter(PartyBotAI* pBot);
    static bool GolemaggEncounter(PartyBotAI* pBot);
    static bool MajordomoEncounter(PartyBotAI* pBot);
    static bool RagnarosEncounter(PartyBotAI* pBot);
    static bool TrashEncounter(PartyBotAI* pBot);

    static void ResetEncounterVars();

    static uint32 LastInsultedTime;

    static bool AnnouncedLivingBomb;
    static bool BaronGeddonInferno;
    static Player* LivingBombTarget;
};
