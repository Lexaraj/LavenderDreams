#ifndef MANGOS_PartyBotEncounters_H
#define MANGOS_PartyBotEncounters_H

class PartyBotAI;  // Forward declaration

class PartyBotEncounters
{
public:
    // Map IDs
    enum MapIds
    {
        MAP_ID_MOLTEN_CORE = 409,
        MAP_ID_BLACKROCK_SPIRE = 229,
        MAP_ID_BLACKROCK_DEPTHS = 230,
        MAP_ID_BLACKWING_LAIR = 469,
        MAP_ID_AHN_QIRAJ_TEMPLE = 531,
        MAP_ID_NAXXRAMAS = 533,
        MAP_ID_ZUL_GURUB = 309,
        MAP_ID_ONYXIAS_LAIR = 249,
        MAP_ID_STRATHOLME = 329,
        MAP_ID_SCHOLOMANCE = 289,
        MAP_ID_DIRE_MAUL = 429,
        MAP_ID_MARAUDON = 349,
        MAP_ID_ULDAMAN = 70,
        MAP_ID_SUNKEN_TEMPLE = 109,
        MAP_ID_RAZORFEN_DOWNS = 129,
        MAP_ID_RAZORFEN_KRAUL = 47,
        MAP_ID_BLACKFATHOM_DEEPS = 48,
        MAP_ID_WAILING_CAVERNS = 43,
        MAP_ID_DEADMINES = 36,
        MAP_ID_STORMWIND_STOCKADE = 34,
        MAP_ID_SHADOWFANG_KEEP = 33,
        MAP_ID_RAGEFIRE_CHASM = 389
    };

    static bool m_overrideMeleePosition;
    static bool m_overrideMagicDispel;
    static bool OverrideMeleePosition() { return m_overrideMeleePosition; }
    static bool OverrideMagicDispel() { return m_overrideMagicDispel; }

    static bool HandleEncounterAI(PartyBotAI* pBot);
    static bool IsEncounterInProgress(PartyBotAI* pBot);
};

#endif 