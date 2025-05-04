#include "PartyBotAI.h"
#include "PartyBotChat.h"

PartyBotChat* PartyBotChat::instance = nullptr;

const std::set<std::string> PartyBotChat::greetings = {
    "hey",
    "hello",
    "hi",
    "greetings",
    "salutations",
    "yo",
    "sup",
    "what's good",
    "whats good",
};


PartyBotChat* PartyBotChat::GetInstance()
{
    if (!instance)
        instance = new PartyBotChat();
    
    return instance;
}


void PartyBotChat::HealerAnnounceTank(const char* pName, Player* player)
{
    std::string message = GetHealerTankAnnouncementText(pName, player);
    SendPartyChat(message.c_str(), player);
    HasHealerAnnouncedTank = true;
}

std::string PartyBotChat::GetHealerTankAnnouncementText(const char* pName, Player* player)
{
    char message[128];
    uint8 pClass = player->GetClass();
    
    switch (pClass)
    {
        case CLASS_PRIEST:
            snprintf(message, sizeof(message), "By the grace of the Light, I stand ready. %s is chosen as the vanguard—I shall be their balm.", pName);
            break;
        case CLASS_DRUID:
            snprintf(message, sizeof(message), "By Nature's blessing, I shall keep %s whole through whatever trials await.", pName);
            break;
        case CLASS_PALADIN:
            snprintf(message, sizeof(message), "The Light guide us—%s stands as my shield. I shall mend the wounds of battle.", pName);
            break;
        case CLASS_SHAMAN:
            snprintf(message, sizeof(message), "The elements grant me power to preserve %s's strength in battle.", pName);
            break;
        default:
            snprintf(message, sizeof(message), "I will focus my healing on %s, our protector. (Class: %d)", pName, pClass);
            break;
    }
    return message;
}


void PartyBotChat::SendPartyChat(const char* message, Player* player) const
{
    if (!player || !player->GetGroup())
        return;

    WorldPacket* data = new WorldPacket();
    ChatHandler::BuildChatPacket(*data, CHAT_MSG_PARTY, message, LANG_UNIVERSAL, CHAT_TAG_NONE, player->GetObjectGuid(), player->GetName());
    player->GetGroup()->BroadcastPacket(data, false);
    delete data;
}


void PartyBotChat::OnPacketReceived(WorldPacket const* packet)
{
    uint8 msgtype = 0;
    uint32 lang = 0;
    ObjectGuid senderGuid;
    ObjectGuid targetGuid;
    uint32 msgLen = 0;
    std::string message;

    try {
        WorldPacket& buffer = const_cast<WorldPacket&>(*packet);
        buffer >> msgtype;
        buffer >> lang;
        buffer >> senderGuid;
        buffer >> targetGuid;
        buffer >> msgLen;
        
        if (msgLen > 0 && msgLen <= 255)
            buffer >> message;

        ProcessPartyMessage(senderGuid, message);
    }
    catch (ByteBufferException const& e) {
        return;
    }
}

void PartyBotChat::ProcessPartyMessage(ObjectGuid const& senderGuid, std::string const& message)
{    
    Player* sender = ObjectAccessor::FindPlayer(senderGuid);
    if (!sender)
        return;

    // Extract player name and guid
    std::string senderInfo = senderGuid.GetString();
    size_t nameStart = senderInfo.find("Player ") + 7;
    size_t nameEnd = senderInfo.find(" (Guid");
    std::string playerName = senderInfo.substr(nameStart, nameEnd - nameStart);
    size_t guidStart = senderInfo.find("Guid: ") + 6; 
    size_t guidEnd = senderInfo.find(")");
    uint32 playerGuid = std::stoul(senderInfo.substr(guidStart, guidEnd - guidStart));

    

    // Process the message
    std::string msg = message;
    std::transform(msg.begin(), msg.end(), msg.begin(), ::tolower);

    // Greetings
    if (greetings.find(msg) != greetings.end())
    {
        if (Group* group = sender->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* pMember = itr->getSource())
                {
                    if (pMember->GetGUIDLow() == playerGuid)
                        continue;

                    if (pMember->AI())
                    {
                        std::string greeting = *std::next(greetings.begin(), rand() % greetings.size());
                        std::string response = (char)toupper(greeting[0]) + greeting.substr(1) + " " + playerName + "!";
                        SendPartyChat(response.c_str(), pMember);
                    }
                }
            }
        }
    }
}