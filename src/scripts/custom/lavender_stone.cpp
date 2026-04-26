#include "lavender_stone.h"

bool GossipHello_lavender_stone(Player* player, GameObject* go)
{
    if (go->GetEntry() != MEETING_STONE_BRD)
        return false;

    player->PlayerTalkClass->ClearMenus();
    player->ADD_GOSSIP_ITEM(5, "Clear raid lockout for Molten Core", GOSSIP_SENDER_MAIN, GOSSIP_ACT_CLEAR_MC);
    player->ADD_GOSSIP_ITEM(5, "Clear raid lockout for Blackwing Lair", GOSSIP_SENDER_MAIN, GOSSIP_ACT_CLEAR_BWL);
    player->SEND_GOSSIP_MENU(85000, go->GetObjectGuid());
    return true;
}

bool GossipSelect_lavender_stone(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
{
    if (go->GetEntry() == MEETING_STONE_BRD)
    {
        if (action == GOSSIP_ACT_CLEAR_MC)
        {
            if (ClearLockout(player, MAP_ID_MC))
            {
                player->GetSession()->SendNotification("Molten Core lockout cleared");
                player->PSendSysMessage("Molten Core lockout cleared.");
            }
        }
        else if (action == GOSSIP_ACT_CLEAR_BWL)
        {
            if (ClearLockout(player, MAP_ID_BWL))
            {
                player->GetSession()->SendNotification("Blackwing Lair lockout cleared");
                player->PSendSysMessage("Blackwing Lair lockout cleared.");
            }
        }
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
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
    pNewScript->pGOGossipHello = &GossipHello_lavender_stone;
    pNewScript->pGOGossipSelect = &GossipSelect_lavender_stone;
    pNewScript->RegisterSelf();
}