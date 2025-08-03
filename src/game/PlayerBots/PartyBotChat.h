#ifndef PARTY_BOT_CHAT_H
#define PARTY_BOT_CHAT_H

#include "Chat.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Group.h"
#include "World.h"

static const char* GetPlayerClassName(uint8 playerClass)
{
    switch (playerClass)
    {
        case CLASS_WARRIOR:
            return "Warrior";
        case CLASS_PALADIN:
            return "Paladin"; 
        case CLASS_HUNTER:
            return "Hunter";
        case CLASS_ROGUE:
            return "Rogue";
        case CLASS_PRIEST:
            return "Priest";
        case CLASS_SHAMAN:
            return "Shaman";
        case CLASS_MAGE:
            return "Mage";
        case CLASS_WARLOCK:
            return "Warlock";
        case CLASS_DRUID:
            return "Druid";
        default:
            return "Unknown";
    }
}


class PartyBotChat
{

public:
    static const std::vector<std::string> greetings;
    static const std::vector<std::string> affirmativeSuccess;
    static const std::vector<std::string> insults;
    static const std::vector<std::string> insultResponses;

    static PartyBotChat* GetInstance();
    static std::string GetHealerTankAnnouncementText(const char* pName, Player* player);
    static std::string GetInsult();
    static std::string GetInsultResponse();
    static std::string GetAffirmativeSuccess();

    void OnPacketReceived(WorldPacket const* packet);
    void ProcessPartyMessage(ObjectGuid const& senderGuid, std::string const& message);
    void SendPartyChat(const char* message, Player* player) const;
    void SendPartyChat(const char* message, Player* player, float delay) const;
    void HealerAnnounceTank(const char* msg, Player* player);
    void ProcessDirectMessage(ObjectGuid const& senderGuid, std::string const& message, Player* player);
    bool HandleHotMeCommand(Player* player, Player* sender);
    bool HasHealerAnnouncedTank = false;
    
    time_t GetLastPartyChatTimestamp(ObjectGuid const& playerGuid) const;
    void SetLastPartyChatTimestamp(ObjectGuid const& playerGuid, time_t timestamp);
    void SetPartyBotCooldown(ObjectGuid const& playerGuid, int seconds);
    int GetPartyBotCooldown(ObjectGuid const& playerGuid) const;
    void ClearAllCooldowns();

private:
    static PartyBotChat* instance;
    PartyBotChat() {}
    ~PartyBotChat() {}
    
    std::map<ObjectGuid, time_t> lastPartyChatTimestamps;
    std::map<ObjectGuid, time_t> partyBotCooldowns;
};

#endif // PARTY_BOT_CHAT_H
