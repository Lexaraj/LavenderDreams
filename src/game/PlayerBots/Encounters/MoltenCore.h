#include "PartyBotAI.h"
#include "PartyBotEncounters.h"

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

enum Bolos
{
    LAVA_SURGER = 12101,
    LAVA_SPAWN = 12265    
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

    static void ResetEncounterVars();

    static bool AnnouncedLivingBomb;
    static Player* LivingBombTarget;
    static bool BaronGeddonInferno;
    static uint32 LastInsultedTime;
};
