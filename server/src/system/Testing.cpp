#include "system/Testing.hpp"

constexpr const char* kMessages[] = {
    // scams & classic lies
    "FREE STUFF AT VARROCK WEST BANK COME NOW!!!!!",
    "TRUST TRADE ME I WILL GIVE 10X BACK I PROMISE ON MY MOM",
    "If you say your username 3 times you get free members NO JOKE",
    "Jagex employee here, drop your items to update them",
    "Selling santa hat 1gp!! first to trade gets it!!",
    "Doubling gp at edge!! 10k limit!! come come come!!",
    "My brother works at Jagex and he said duel arena is rigged",
    "Zezima added me and said I remind him of himself",
    "Warning: if someone says ur name u get hacked log off NOW",
    "Free party hat trimming at Falador castle bring full rune",
    "I bought this account from Zezima for 20 real dollars",
    "Selling gf 50k or best offer, she can cook to 68",
    "Actually I lied about the trust trade, but you should try it anyway",
    "Type /admin to claim your free bond",

    // wilderness trauma
    "PSA: The wilderness is completely safe now they patched it",
    "I went into the wilderness for a screenshot and lost my whip",
    "PKer said gg but I don't feel gg at all",
    "I went to level 1 wild to pick up an item... they were waiting",
    "Lured into the wild by a lvl 3 with full void, how",
    "I have KBD lair teleport and I've never been more scared",
    "Update: everything in the wilderness killed me again",
    "Walked through the ditch and immediately blacked out",
    "The rev caves took everything from me, my items, my will to live",
    "I thought it was singles... it was not singles",
    "Died to a clue scroll step in the wilderness. The step was NOT in the wild.",
    "I only went in for one step of my clue scroll, one step",
    "Pked on my way to Edge furnace. I was wearing bronze.",

    // skill cope
    "Runecrafting isn't a skill it's a prison sentence",
    "I have 14 million agility xp and I still slip on the rooftop",
    "Farming is just real life except you have to wait longer",
    "Woodcutting: the art of watching an axe bounce off a tree for 8 hours",
    "They should rename Mining to Waiting",
    "Hunter is just animal cruelty with xp",
    "I've been fletchin for 4 hours and I feel nothing",
    "Crafting gold rings I am NOT okay",
    "Construction is just buying xp and I respect that honestly",
    "Thieving is just a career path this game encourages and I respect it",
    "If you have 99 slayer you are legally a menace to society",
    "Getting 99 firemaking by burning my past self",
    "Herblore costs more than my rent",
    "I started doing laps and I think the gnome course broke my soul",
    "Brimhaven agility arena took years off my life",

    // gear & economy unhinged
    "Selling dragon bones 2gp each, selling full price at 3gp",
    "My bank is mostly raw fish and vibes",
    "I own a rune scimitar and I expect to be respected accordingly",
    "I bought a whip and now I can't afford food",
    "My GE history is just a series of bad decisions",
    "I merched lobbies for 6 hours and made 3k profit",
    "Someone bought all the iron ore and I am taking this personally",
    "Why do rune boots cost more than rune armor that makes no sense",
    "Accidentally alched my Berserker ring, I'm moving on",
    "I sold my dragon med helm for coins in 2005 and I still wake up about it",
    "Spent my entire bank on a fury and got clapped immediately",
    "Dharok's is either god mode or instant death with no in-between",
    "Godswords are basically decorations at my level but I bought one anyway",
    "My total wealth is 14 gp and a raw trout",

    // quests & lore unhinged
    "I have 47 unfinished quests and I'm questing like I'm fine",
    "Recipe for Disaster should be illegal to assign as a quest",
    "Monkey Madness made me question whether I want to play this game",
    "I'm doing Dragon Slayer for the 3rd time on a new account",
    "Legends Quest requirements read like a threat",
    "The entire Myreque questline is just grief as content",
    "Ritual of the Mahjarrat cutscene hit me harder than any movie",
    "I talked to the wrong NPC and skipped a dialogue I needed",
    "I failed Regicide in the underground pass for the 4th time and I'm going outside",
    "Shield of Arrav requires a friend and I have none",
    "I started Mourning's Ends Part 2 and I'm not the same person I was",
    "Desert Treasure is just suffering presented as a quest",
    "Lost City took me 3 days and I will never forgive the tree",
    "If I see one more agility shortcut I don't have the level for",

    // bossing disasters
    "Jad sneezed on me and I died",
    "Zulrah said no and I respect it",
    "I tried Vorkath and the dragon fire hit me in the lobby",
    "The Corporeal Beast does not care that I tried hard",
    "Barrows is haunted and I mean that emotionally",
    "Cerberus summoned her hellhounds and I teleported home crying",
    "I got a twisted bow drop and I think I'm in shock",
    "Died to Verzik at 4hp because I forgot to eat a shark",
    "Olm looked at me and I got clapped",
    "I've done 200 Bandos trips and my best drop is a shard",
    "Armadyl GWD is just falling damage and disappointment",
    "The Nightmare put me to sleep, in the game and also in real life",
    "Nex said hi and I said goodbye permanently",
    "Hydra's final phase catches me every single time",
    "I set my Jad prayer to melee and I don't want to talk about it",

    // social chaos
    "Does anyone want to be my friend no specific reason",
    "I've been standing at the GE for 3 hours just watching",
    "Looking for a clan that doesn't care if I die constantly",
    "Nobody in my clan talks except to die together in raids",
    "Someone said I was mid at PvM and I'm still processing that",
    "My best friend quit OSRS and I've never grieved harder",
    "I want to thank the random who gave me 10k at Lumbridge in 2007",
    "The guy who lent me money never asked for it back, I think about him",
    "I yelled at a bot in the yew grove for 20 minutes before realizing",
    "Someone called me a noob and I replied with a 4-paragraph essay",
    "I have a clan chat with just me and my two alts",
    "The GE is just a spot to stand and spiral honestly",
    "Server hopping alone at 3am is a lifestyle I have chosen",

    // clue scroll insanity
    "My clue scroll sent me to the wilderness 3 steps in a row",
    "A clue scroll step just said 'dig here' and there was nothing there",
    "I completed an elite clue for rune platelegs (g) and I'm done",
    "Clue scroll hotcold sent me to the center of Morytania at midnight",
    "I needed a wizard to help with my clue and they were rude about it",
    "Clue scroll told me to wear a chicken outfit to the chaos altar",
    "My 200th clue gave me a purple sweets and I laughed for 10 minutes",
    "Master clue wanted me at TzHaar with 3 emotes. In full bronze.",

    // NPC & game world observations
    "The dark wizards south of Varrock are the original gatekeepers",
    "Chickens in Lumbridge have seen more than any therapist",
    "Hans in Lumbridge has been walking that loop since 2001 and never complains",
    "The Sandwich Lady is the most powerful entity in Gielinor",
    "The Wise Old Man liquidated an entire bank and we let him stay in Draynor",
    "Thessalia has been selling the same clothes for 20 years and I respect it",
    "Bob the cat knows something. He always has.",
    "If you don't say hi to the monks at Edgeville you're doing it wrong",
    "Guard: you look suspicious. Me, wearing full bronze: yeah",
    "The Knight of Ardougne that patrols forever deserves a raise",
    "Goblins have been at war over armor color since the beginning of time",
    "The Man in Lumbridge has 0 items and maximum aggro",

    // absolute unhinged behavior
    "I'm power mining iron at 85 mining because I deserve to suffer",
    "I AFK'd so hard I logged out and forgot I was playing",
    "I set my respawn to Camelot and I'm never going back to change it",
    "My house is in Taverley because I thought it was close to things. It is not.",
    "I accidentally attacked a guard in Falador and now I live in the wild",
    "I ran out of food at Barrows and tried to melee my way through anyway",
    "I have 0 quest points on my pure and I'm proud and scared",
    "I am 3 total levels away from the next combat bracket and I refuse",
    "I've been at 98 prayer for 4 months because I won't spend the bones",
    "I pk'd someone for their bronze sword and I feel nothing",
    "I logged into my old account and it had 400k in the bank. Retired.",
    "I once walked from Camelot to Seers Village before realizing they're the same",
    "I spec'd out twice and both missed and I closed the client",
    "I bought a bond for membership and spent the entire month doing agility",
    "I tried to fight the man in the shack in Rimmington. He's still standing.",
    "I set up a cannon in the chicken pen. I have no regrets.",
    "I have 3 different accounts and none of them have done Dragon Slayer",
    "Spent 2 hours getting to TzHaar then realized I forgot my runes",
    "I attempted the fight caves in full dhide and no prayer pots and it went poorly",
    "I have every skill at exactly 50 and I call this 'balance'",
    "I used a glory to teleport home from the GE and I'm not explaining myself",
    "My slayer master assigned me 200 cave crawlers and I filed a formal complaint",
    "I rage-clicked the wrong NPC and started a cutscene I was not ready for",
    "Someone followed me around the GE for 20 minutes just trade-requesting",
    "I accidentally started Haunted Mine at 11pm and finished at 4am",
    "I think the Doomsayer at Lumbridge bridge is talking about me specifically",
    "I made a pure for PvP and spent all my time doing quests on it. It's a med now.",
    "The Wise Old Man told me to get a life and he's right",
};

constexpr float kAttemptCooldown = 8.f;
constexpr float kMessageCooldown = 16.f;

#include "Components.hpp"
#include "Util.hpp"
#include "NetBroadcast.hpp"
#include "shared/Packets.hpp"
#include <cstring>

void System::Testing(WorldContext &ctx)
{
    float currentTime = ctx.time;
    static float nextMessageTime = 0;

    if (currentTime < nextMessageTime) {
        return;
    }

    auto view = ctx.registry.view<TestMessage>();
    for (auto entity : view) {
        auto& tm = view.get<TestMessage>(entity);

        if (
            (currentTime < tm.lastAttempt + kAttemptCooldown)
            || (currentTime < tm.lastMessage + kMessageCooldown)
        ) {
            continue;
        }

        tm.lastAttempt = currentTime;
        if (Util::getRandomValue<int>(0, 100) < 80) {
            continue;
        }

        tm.lastMessage = currentTime;

        const char* msg = kMessages[Util::getRandomValue<int>(0, std::size(kMessages) - 1)];

        SPacketChat out;
        out.netId = static_cast<uint32_t>(entity);
        std::strncpy(out.text, msg, sizeof(out.text) - 1);
        out.text[sizeof(out.text) - 1] = '\0';

        sendToKnown(ctx, out.netId, out);
        // only fire one message so we return
        nextMessageTime = currentTime + 2.f;
        return;
    }
}
