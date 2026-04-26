#include "scriptPCH.h"
#include "custom.h"

#define MEETING_STONE_BRD 179584
#define MEETING_STONE_BRS 179585

#define MAP_ID_MC 409
#define MAP_ID_BWL 469

uint32 const GOSSIP_ACT_CLEAR_MC  = GOSSIP_ACTION_INFO_DEF;
uint32 const GOSSIP_ACT_CLEAR_BWL = GOSSIP_ACTION_INFO_DEF + 1;


bool GossipHello_lavender_stone(Player* player, GameObject* go);

bool GossipSelect_lavender_stone(Player* player, GameObject* go, uint32 sender, uint32 action);

bool ClearLockout(Player* player, uint32 mapid);

void AddSC_lavender_stone();