#ifndef PARTY_BOT_CHAT_H
#define PARTY_BOT_CHAT_H

#include "Chat.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Group.h"



class PartyBotChat
{

public:
    static PartyBotChat* GetInstance();
    void OnPacketReceived(WorldPacket const* packet);
    void ProcessPartyMessage(ObjectGuid const& senderGuid, std::string const& message);
    void SendPartyChat(const char* message, Player* player) const;
    void HealerAnnounceTank(const char* msg, Player* player);
    std::string GetHealerTankAnnouncementText(const char* pName, Player* player);

    // Define greetings
    static const std::set<std::string> greetings; 

    bool HasHealerAnnouncedTank = false;

private:
    static PartyBotChat* instance;
    PartyBotChat() {}
    ~PartyBotChat() {}
};

#endif // PARTY_BOT_CHAT_H
