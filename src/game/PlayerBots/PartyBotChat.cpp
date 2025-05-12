#include "PartyBotAI.h"
#include "PartyBotChat.h"

PartyBotChat* PartyBotChat::instance = nullptr;


const std::vector<std::string> PartyBotChat::greetings = {
    "hey",
    "hello",
    "hi",
    "greetings",
    "salutations",
    "yo",
    "sup",
    "what's good",
    "what up",
    "what's up",
    "what's poppin",
    
};


const std::vector<std::string> PartyBotChat::affirmativeSuccess = {
    "got you",
    "gotcha",
    "you got it",
    "coming right up",
    "on it",
    "roger that",
    "incoming"
};



const std::vector<std::string> PartyBotChat::insults = {
    "fool",
    "idiot",
    "noob",
    "clown",
    "moron",
    "imbecile",
    "dipshit",
    "faggot",
    "cunt",
    "loser",
    "retard",
    "nig",
    "queer",
    "homo",
    "rabbit",
    "rabbi",
    "bear fucker",
    "Jackson lover",
    "Jerry toucher"
};


const std::vector<std::string> PartyBotChat::insultResponses = {
    "I'm not a fan of your kind",
    "Fuck you",
    "Go fuck yourself",
    "I'm not in the mood for your shit",
    "Damn chill out",
    "Tu madre",
    "Rofl",
    "Lmao",
    "Ok",
    "Whatever",
    "I'm busy"
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


std::string PartyBotChat::GetInsult()
{
    return insults[urand(0, insults.size()-1)];
}


std::string PartyBotChat::GetAffirmativeSuccess()
{
    return affirmativeSuccess[urand(0, affirmativeSuccess.size()-1)];
}


std::string PartyBotChat::GetInsultResponse()
{
    return insultResponses[urand(0, insultResponses.size()-1)];
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

void PartyBotChat::SendPartyChat(const char* message, Player* player, float delay) const
{
    if (!player || !player->GetGroup())
        return;

    // Schedule this task to execute after delay (milliseconds)
    if (delay > 0)
    {
        auto sendPartyMessage = [message = std::string(message), playerGuid = player->GetObjectGuid()]() {
            if (Player* playerPtr = sObjectMgr.GetPlayer(playerGuid))
            {
                if (Group* group = playerPtr->GetGroup())
                {
                    WorldPacket* data = new WorldPacket();
                    ChatHandler::BuildChatPacket(*data, CHAT_MSG_PARTY, message.c_str(), LANG_UNIVERSAL, CHAT_TAG_NONE, playerPtr->GetObjectGuid(), playerPtr->GetName());
                    group->BroadcastPacket(data, false);
                    delete data;
                }
            }
        };
        GetSWorld().GetMessager().AddMessage([sendPartyMessage, delay](World* world) {
            world->AddAsyncTask(sendPartyMessage);
        });
    }
    else
    {
        SendPartyChat(message, player);
    }
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
    if (std::find(greetings.begin(), greetings.end(), msg) != greetings.end())
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
                        SendPartyChat(response.c_str(), pMember, urand(1000, 4000));
                    }
                }
            }
        }
    }


    // My name
    // Check if message contains bot names
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
                    std::string mName = pMember->GetName();
                    uint8 mClass = pMember->GetClass();
                    std::string mNameLower = mName;
                    std::transform(mNameLower.begin(), mNameLower.end(), mNameLower.begin(), ::tolower);

                    if (msg.find(mNameLower) != std::string::npos)
                    {
                        // i've been spoken to
                        ProcessDirectMessage(senderGuid, message, pMember);
                    }
                }
            }
        }
    }
}


void PartyBotChat::ProcessDirectMessage(ObjectGuid const& senderGuid, std::string const& message, Player* player)
{
    Player* sender = ObjectAccessor::FindPlayer(senderGuid);
    if (!sender)
        return;

    // look for commands in the message
    std::string messageLower = message;
    std::transform(messageLower.begin(), messageLower.end(), messageLower.begin(), ::tolower);
    if (messageLower.find("hot me") != std::string::npos)
    {
        if (HandleHotMeCommand(player, sender))
        {
            char message[128];
            snprintf(message, sizeof(message), "Got you %s!", sender->GetName());
            SendPartyChat(message, player);
        }
    }
    
}


bool PartyBotChat::HandleHotMeCommand(Player* player, Player* sender)
{
    // cast a HoT on the sender if I'm a healer
    
    return true;
}