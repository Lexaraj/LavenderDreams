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
    
    GetInstance()->SetLastPartyChatTimestamp(player->GetObjectGuid(), time(nullptr));
}

void PartyBotChat::SendPartyChat(const char* message, Player* player, float delay) const
{
    if (!player || !player->GetGroup())
        return;

    // check if the player is on cooldown
    if (GetPartyBotCooldown(player->GetObjectGuid()) > 0)
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
                    
                    GetInstance()->SetLastPartyChatTimestamp(playerPtr->GetObjectGuid(), time(nullptr));
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
                        ProcessDirectMessage(senderGuid, message, pMember);
                        return;
                    }
                }
            }
        }
    }


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


    // Who is the pLeader
    if (msg.find("who's your daddy") != std::string::npos || 
        msg.find("who is your daddy") != std::string::npos)
    {
        if (Group* group = sender->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* pMember = itr->getSource())
                {
                    if (pMember->GetGUIDLow() == playerGuid)
                        continue;

                    if(PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
                    {
                        char message[128];
                        Player* pLeader = ObjectAccessor::FindPlayer(pAI->m_leaderGuid);
                        if(pLeader)
                        {

                            if(pLeader->GetDeathState() == DEAD)
                            {
                                snprintf(message, sizeof(message), "My daddy, for your fuckin information, is DEAD, asshole. Lol jk, it's %s.", pLeader->GetName());
                                SendPartyChat(message, pMember);
                            }
                            else
                            {
                                const char* messages[] = {
                                    "%s is my leader and I follow them with unwavering loyalty.",
                                    "I serve %s, they are my absolute lordingheimenschmiggdt.",
                                    "I'm %s's bitch, and I'm proud of it.",
                                    "My daddy is %s."
                                };
                                const char* selectedMessage = messages[urand(0,2)];
                                snprintf(message, sizeof(message), selectedMessage, pLeader->GetName());
                                SendPartyChat(message, pMember);
                            }
                        }
                    }
                }
            }
        }
    }

    // Paladin party bots
    bool const paladinAuraVerb = (msg.find("use") != std::string::npos || msg.find("cast") != std::string::npos);
    bool const paladinAuraTarget = (msg.find("devotion") != std::string::npos ||
        msg.find("devo") != std::string::npos ||
        msg.find("concentration") != std::string::npos ||
        msg.find("conc") != std::string::npos ||
        msg.find("sanctity") != std::string::npos ||
        msg.find("sanc") != std::string::npos ||
        msg.find("retribution") != std::string::npos ||
        msg.find("ret") != std::string::npos ||
        msg.find("valiance") != std::string::npos ||
        msg.find("val") != std::string::npos);
    if (paladinAuraVerb && paladinAuraTarget &&
        (msg.find("paladin") != std::string::npos || 
         msg.find("pal") != std::string::npos ||
         msg.find("pally") != std::string::npos ||
         msg.find("aura") != std::string::npos)
    ) {
        Player* respondent = nullptr;
        std::string chosenAuraName;
        if (Group* group = sender->GetGroup())
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* pMember = itr->getSource())
                {
                    if (pMember->GetGUIDLow() == playerGuid)
                        continue;
                    if (pMember->GetClass() != CLASS_PALADIN)
                        continue;
                    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(pMember->AI()))
                    {
                        std::string thisAura;
                        if (pAI->TryApplyPaladinAura(msg, &thisAura))
                        {
                            if (!respondent)
                            {
                                respondent = pMember;
                                chosenAuraName = std::move(thisAura);
                            }
                        }
                    }
                }
            }
        }
        if (respondent && !chosenAuraName.empty())
        {
            char reply[192];
            snprintf(reply, sizeof(reply), "Using %s.", chosenAuraName.c_str());
            SendPartyChat(reply, respondent);
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
    else
    if (messageLower.find("i'm your daddy") != std::string::npos)
    {
        if (HandleFollowMeCommand(player, sender))
        {
            char message[128];
            snprintf(message, sizeof(message), "Oh yeah fuck yeah shit yeah kum yeah, %s.", sender->GetName());
            SendPartyChat(message, player);
        }
    }
    else
    if (messageLower.find("cc pull") != std::string::npos)
    {
        if (HandleCCPullCommand(player, sender))
        {
            char message[128];
            snprintf(message, sizeof(message), "Pulling!", sender->GetName());
            SendPartyChat(message, player);
        }
    }
    else if (player->GetClass() == CLASS_PALADIN &&
             (messageLower.find("use") != std::string::npos || messageLower.find("cast") != std::string::npos))
    {
        if (messageLower.find("devotion") != std::string::npos ||
            messageLower.find("devo") != std::string::npos ||
            messageLower.find("concentration") != std::string::npos ||
            messageLower.find("conc") != std::string::npos ||
            messageLower.find("sanctity") != std::string::npos ||
            messageLower.find("sanc") != std::string::npos ||
            messageLower.find("retribution") != std::string::npos ||
            messageLower.find("ret") != std::string::npos ||
            messageLower.find("valiance") != std::string::npos ||
            messageLower.find("val") != std::string::npos)
        {
            if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(player->AI()))
            {
                std::string auraName;
                if (pAI->TryApplyPaladinAura(messageLower, &auraName))
                {
                    char reply[192];
                    snprintf(reply, sizeof(reply), "Using %s.", auraName.c_str());
                    SendPartyChat(reply, player);
                }
            }
        }
    }

    
}


bool PartyBotChat::HandleHotMeCommand(Player* player, Player* sender)
{
    // cast a HoT on the sender if I'm a healer
    
    return true;
}

bool PartyBotChat::HandleFollowMeCommand(Player* player, Player* sender)
{
    // make the bot follow the sender
    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(player->AI()))
    {
        pAI->SetOwner(sender);
    }
    return true;
}


bool PartyBotChat::HandleCCPullCommand(Player* player, Player* sender)
{
    if (PartyBotAI* pAI = dynamic_cast<PartyBotAI*>(player->AI()))
    {
        if (pAI->m_marksToCC.empty())
        {
            char message[128];
            snprintf(message, sizeof(message), "I don't have a cc mark, %s.", sender->GetName());
            SendPartyChat(message, player);
            return false;
        }
        return pAI->CrowdControlMarkedTargets();
    }
    return false;
}


time_t PartyBotChat::GetLastPartyChatTimestamp(ObjectGuid const& playerGuid) const
{
    auto it = lastPartyChatTimestamps.find(playerGuid);
    if (it != lastPartyChatTimestamps.end())
    {
        return it->second;
    }
    return 0; // Return 0 if no timestamp found for this player
}

void PartyBotChat::SetLastPartyChatTimestamp(ObjectGuid const& playerGuid, time_t timestamp)
{
    lastPartyChatTimestamps[playerGuid] = timestamp;
}

void PartyBotChat::SetPartyBotCooldown(ObjectGuid const& playerGuid, int seconds)
{
    // Calculate the timestamp when the cooldown expires (current time + seconds)
    time_t cooldownExpiry = time(nullptr) + seconds;
    partyBotCooldowns[playerGuid] = cooldownExpiry;
}

int PartyBotChat::GetPartyBotCooldown(ObjectGuid const& playerGuid) const
{
    auto it = partyBotCooldowns.find(playerGuid);
    if (it != partyBotCooldowns.end())
    {
        time_t currentTime = time(nullptr);
        time_t cooldownExpiry = it->second;
        
        // If cooldown has expired, return 0
        if (currentTime >= cooldownExpiry)
        {
            return 0;
        }
        
        // Return remaining seconds
        return static_cast<int>(cooldownExpiry - currentTime);
    }
    
    // No cooldown set for this player
    return 0;
}

void PartyBotChat::ClearAllCooldowns()
{
    partyBotCooldowns.clear();
}