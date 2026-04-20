#include "lavender_stone.h"

bool GossipSelect_lavender_stone(Player* player, GameObject* go, uint32 sender, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    switch (go->GetEntry())
    {
        case MEETING_STONE_BRD:
            if (ClearLockout(player, MAP_ID_MC))
                player->GetSession()->SendNotification("Molten Core lockout cleared");
                player->PSendSysMessage("Molten Core lockout cleared.");
            break;
        case MEETING_STONE_BRS:
            if (ClearLockout(player, MAP_ID_BWL))
                player->GetSession()->SendNotification("Blackwing Lair lockout cleared");
                player->PSendSysMessage("Blackwing Lair lockout cleared.");
            break;
        default:
            player->GetSession()->SendNotification("Unknown meeting stone");
    }

    player->CLOSE_GOSSIP_MENU();
            

    return false;
}

bool ClearLockout(Player* player, uint32 mapid)
{
    if (!player || !player->IsInWorld())
        return false;

    uint32 counter = 0;
    Player::BoundInstancesMap &binds = player->GetBoundInstances();
    for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
    {
        if (mapid != itr->first)
        {
            ++itr;
            continue;
        }
        if (itr->first != player->GetMapId())
        {
            player->UnbindInstance(itr);
            counter++;
        }
        else
        {
            ++itr;
        }
    }  
    return true;
}


void AddSC_lavender_stone()
{
    Script* pNewScript = new Script;
    pNewScript->Name = "lavender_stone";
    pNewScript->pGOGossipSelect = &GossipSelect_lavender_stone;
    pNewScript->RegisterSelf();
}