#pragma once
#include "SMJS_Module.h"
#include <map>

enum Dota_Hero {
	Hero_Invalid = -1,
	Hero_Base = 0,
	Hero_AntiMage = 1,
	Hero_Axe = 2,
	Hero_Bane = 3,
	Hero_Bloodseeker = 4,
	Hero_CrystalMaiden = 5,
	Hero_DrowRanger = 6,
	Hero_EarthShaker = 7,
	Hero_Juggernaut = 8,
	Hero_Mirana = 9,
	Hero_Morphling = 10,
	Hero_ShadowFiend = 11,
	Hero_PhantomLancer = 12,
	Hero_Puck = 13,
	Hero_Pudge = 14,
	Hero_Razor = 15,
	Hero_SandKing = 16,
	Hero_StormSpirit = 17,
	Hero_Sven = 18,
	Hero_Tiny = 19,
	Hero_VengefulSpirit = 20,
	Hero_Windrunner = 21,
	Hero_Zeus = 22,
	Hero_Kunkka = 23,
	// Hero_Techies = 24,
	Hero_Lina = 25,
	Hero_Lion = 26,
	Hero_ShadowShaman = 27,
	Hero_Slardar = 28,
	Hero_TideHunter = 29,
	Hero_WitchDoctor = 30,
	Hero_Lich = 31,
	Hero_Riki = 32,
	Hero_Enigma = 33,
	Hero_Tinker = 34,
	Hero_Sniper = 35,
	Hero_Necrolyte = 36,
	Hero_Warlock = 37,
	Hero_BeastMaster = 38,
	Hero_QueenOfPain = 39,
	Hero_Venomancer = 40,
	Hero_FacelessVoid = 41,
	Hero_SkeletonKing = 42,
	Hero_DeathProphet = 43,
	Hero_PhantomAssassin = 44,
	Hero_Pugna = 45,
	Hero_TemplarAssassin = 46,
	Hero_Viper = 47,
	Hero_Luna = 48,
	Hero_DragonKnight = 49,
	Hero_Dazzle = 50,
	Hero_Clockwerk = 51,
	Hero_Leshrac = 52,
	Hero_Furion = 53,
	Hero_Lifestealer = 54,
	Hero_DarkSeer = 55,
	Hero_Clinkz = 56,
	Hero_Omniknight = 57,
	Hero_Enchantress = 58,
	Hero_Huskar = 59,
	Hero_NightStalker = 60,
	Hero_Broodmother = 61,
	Hero_BountyHunter = 62,
	Hero_Weaver = 63,
	Hero_Jakiro = 64,
	Hero_Batrider = 65,
	Hero_Chen = 66,
	Hero_Spectre = 67,
	Hero_AncientApparition = 68,
	Hero_Doom = 69,
	Hero_Ursa = 70,
	Hero_SpiritBreaker = 71,
	Hero_Gyrocopter = 72,
	Hero_Alchemist = 73,
	Hero_Invoker = 74,
	Hero_Silencer = 75,
	Hero_ObsidianDestroyer = 76,
	Hero_Lycan = 77,
	Hero_Brewmaster = 78,
	Hero_ShadowDemon = 79,
	Hero_LoneDruid = 80,
	Hero_ChaosKnight = 81,
	Hero_Meepo = 82,
	Hero_TreantProtector = 83,
	Hero_OgreMagi = 84,
	Hero_Undying = 85,
	Hero_Rubick = 86,
	Hero_Disruptor = 87,
	Hero_NyxAssassin = 88,
	Hero_NagaSiren = 89,
	Hero_KeeperOfTheLight = 90,
	Hero_Wisp = 91,
	Hero_Visage = 92,
	Hero_Slark = 93,
	Hero_Medusa = 94,
	Hero_TrollWarlord = 95,
	Hero_CentaurWarchief = 96,
	Hero_Magnus = 97,
	Hero_Timbersaw = 98,
	Hero_Bristleback = 99,
	Hero_Tusk = 100,
	Hero_SkywrathMage = 101,
	Hero_Abaddon = 102,
	Hero_ElderTitan = 103,
	Hero_LegionCommander = 104,
	
	Hero_Count
};

class MDota : public SMJS_Module {
public:
	MDota();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void OnPluginDestroyed(SMJS_Plugin *plugin){};

	static const char *HeroIdToClassname(int id);

	FUNCTION_DECL(heroIdToClassname);
	FUNCTION_DECL(loadParticleFile);
	FUNCTION_DECL(forceWin);
	FUNCTION_DECL(createUnit);
	

	WRAPPED_CLS(MDota, SMJS_Module) {
		temp->SetClassName(v8::String::New("DotaModule"));

		WRAPPED_FUNC(heroIdToClassname);
		WRAPPED_FUNC(loadParticleFile);
		WRAPPED_FUNC(forceWin);
		WRAPPED_FUNC(createUnit);

		proto->Set(v8::String::New("TEAM_NONE"), v8::Int32::New(0), v8::ReadOnly);
		proto->Set(v8::String::New("TEAM_SPEC"), v8::Int32::New(1), v8::ReadOnly);
		proto->Set(v8::String::New("TEAM_RADI"), v8::Int32::New(2), v8::ReadOnly);
		proto->Set(v8::String::New("TEAM_DIRE"), v8::Int32::New(3), v8::ReadOnly);
		proto->Set(v8::String::New("TEAM_NEUTRAL"), v8::Int32::New(4), v8::ReadOnly);

		proto->Set(v8::String::New("TEAM_RADIANT"), v8::Int32::New(2), v8::ReadOnly);

#define HERO_ID_CONST(name, value) proto->Set(v8::String::New("HERO_" #name), v8::Int32::New(Hero_##value), v8::ReadOnly);

		HERO_ID_CONST(INVALID, Invalid);

		//Radiant Strength
		HERO_ID_CONST(EARTHSHAKER, EarthShaker);
		HERO_ID_CONST(EARTH_SHAKER, EarthShaker);
		HERO_ID_CONST(ES, EarthShaker);
		HERO_ID_CONST(SHAKER, EarthShaker);
		HERO_ID_CONST(RAIGOR, EarthShaker);
		HERO_ID_CONST(RAIGOR_STONEHOOF, EarthShaker);
		HERO_ID_CONST(RAIGORSTONEHOOF, EarthShaker);

		HERO_ID_CONST(SVEN, Sven);
		HERO_ID_CONST(ROGUEKNIGHT, Sven);
		HERO_ID_CONST(ROGUE_KNIGHT, Sven);

		HERO_ID_CONST(TINY, Tiny);
		HERO_ID_CONST(STONEGIANT, Tiny);
		HERO_ID_CONST(STONE_GIANT, Tiny);

		HERO_ID_CONST(KUNKKA, Kunkka);
		HERO_ID_CONST(ADMIRAL, Kunkka);

		HERO_ID_CONST(BEASTMASTER, BeastMaster);
		HERO_ID_CONST(BEAST_MASTER, BeastMaster);
		HERO_ID_CONST(KARROCH, BeastMaster);

		HERO_ID_CONST(DRAGONKNIGHT, DragonKnight);
		HERO_ID_CONST(DK, DragonKnight);
		HERO_ID_CONST(DRAGON_KNIGHT, DragonKnight);
		HERO_ID_CONST(DAVION, DragonKnight);
		HERO_ID_CONST(KNIGHTDAVION, DragonKnight);
		HERO_ID_CONST(KNIGHT_DAVION, DragonKnight);

		HERO_ID_CONST(CLOCKWERK, Clockwerk);
		HERO_ID_CONST(CLOCK, Clockwerk);
		HERO_ID_CONST(RATTLETRAP, Clockwerk);
		HERO_ID_CONST(CLOCKWERK_GOBLIN, Clockwerk);
		HERO_ID_CONST(CLOCKWERKGOBLIN, Clockwerk);

		HERO_ID_CONST(OMNIKNIGHT, Omniknight);
		HERO_ID_CONST(OMNI, Omniknight);
		HERO_ID_CONST(OMNI_KNIGHT, Omniknight);
		HERO_ID_CONST(PURIST, Omniknight);
		HERO_ID_CONST(PURIST_THUNDERWRATH, Omniknight);
		HERO_ID_CONST(PURISTTHUNDERWRATH, Omniknight);
		HERO_ID_CONST(THUNDERWRATH, Omniknight);

		HERO_ID_CONST(HUSKAR, Huskar);
		HERO_ID_CONST(HUSK, Huskar);
		HERO_ID_CONST(SACREDWARRIOR, Huskar);
		HERO_ID_CONST(SACRED_WARRIOR, Huskar);

		HERO_ID_CONST(ALCHEMIST, Alchemist);
		HERO_ID_CONST(RAZZIL, Alchemist);
		HERO_ID_CONST(RAZZILDARKBREW, Alchemist);
		HERO_ID_CONST(RAZZIL_DARKBREW, Alchemist);
		HERO_ID_CONST(DARKBREW, Alchemist);
		HERO_ID_CONST(ALCH, Alchemist);

		HERO_ID_CONST(BREWMASTER, Brewmaster);
		HERO_ID_CONST(PANDEREN_BREWMASTER, Brewmaster);
		HERO_ID_CONST(PANDEREN_BREW_MASTER, Brewmaster);
		HERO_ID_CONST(BREW_MASTER, Brewmaster);
		HERO_ID_CONST(BREW, Brewmaster);
		HERO_ID_CONST(MANGIX, Brewmaster);

		HERO_ID_CONST(TREANTPROTECTOR, TreantProtector);
		HERO_ID_CONST(TREANT_PROTECTOR, TreantProtector);
		HERO_ID_CONST(TREANT, TreantProtector);
		HERO_ID_CONST(PROTECTOR, TreantProtector);
		HERO_ID_CONST(ROOF, TreantProtector);
		HERO_ID_CONST(ROOFTRELLEN, TreantProtector);

		HERO_ID_CONST(WISP, Wisp);
		HERO_ID_CONST(IO, Wisp);
		HERO_ID_CONST(GUARDIANWISP, Wisp);
		HERO_ID_CONST(GUARDIAN_WISP, Wisp);

		HERO_ID_CONST(CENT, CentaurWarchief);
		HERO_ID_CONST(CENTAUR, CentaurWarchief);
		HERO_ID_CONST(CENTAURWARCHIEF, CentaurWarchief);
		HERO_ID_CONST(CENTAUR_WARCHIEF, CentaurWarchief);
		HERO_ID_CONST(CENTAUR_WARRUNNER, CentaurWarchief);
		HERO_ID_CONST(CENTAURWARRUNNER, CentaurWarchief);
		HERO_ID_CONST(WARRUNNER, CentaurWarchief);
		HERO_ID_CONST(BRADWARDEN, CentaurWarchief);
		HERO_ID_CONST(BRAD, CentaurWarchief);
		HERO_ID_CONST(CW, CentaurWarchief);

		HERO_ID_CONST(TIMBERSAW, Timbersaw);
		HERO_ID_CONST(TIMBER, Timbersaw);
		HERO_ID_CONST(SHREDDER, Timbersaw);
		HERO_ID_CONST(GOBLIN_SHREDDER, Timbersaw);
		HERO_ID_CONST(GOBLINSHREDDER, Timbersaw);
		HERO_ID_CONST(RIZZRACK, Timbersaw);

		HERO_ID_CONST(BRISTLEBACK, Bristleback);
		HERO_ID_CONST(BRISTLE_BACK, Bristleback);
		HERO_ID_CONST(BRISTLE, Bristleback);
		HERO_ID_CONST(RIGWARL, Bristleback);

		HERO_ID_CONST(TUSK, Tusk);
		HERO_ID_CONST(TUSKARR, Tusk);
		HERO_ID_CONST(YMIR, Tusk);

		HERO_ID_CONST(ET, ElderTitan);
		HERO_ID_CONST(ELDERTITAN, ElderTitan);
		HERO_ID_CONST(ELDER_TITAN, ElderTitan);
		HERO_ID_CONST(TITAN, ElderTitan);
		HERO_ID_CONST(TAUREN, ElderTitan);
		HERO_ID_CONST(TAURENCHIEFTAIN, ElderTitan);
		HERO_ID_CONST(TAUREN_CHIEFTAIN, ElderTitan);
		HERO_ID_CONST(CHIEFTAIN, ElderTitan);
		HERO_ID_CONST(CAIRNE, ElderTitan);
		HERO_ID_CONST(CAIRNEBLOODHOOF, ElderTitan);
		HERO_ID_CONST(CAIRNE_BLOODHOOF, ElderTitan);

		HERO_ID_CONST(LC, LegionCommander);
		HERO_ID_CONST(LEGIONCOMMANDER, LegionCommander);
		HERO_ID_CONST(LEGION_COMMANDER, LegionCommander);
		HERO_ID_CONST(TRESDIN, LegionCommander);
		HERO_ID_CONST(TRES, LegionCommander);


		//Dire Strength
		HERO_ID_CONST(AXE, Axe);
		HERO_ID_CONST(MOGUL_KHAN, Axe);
		HERO_ID_CONST(MOGULKHAN, Axe);
		HERO_ID_CONST(MOGUL, Axe);
		HERO_ID_CONST(KHAN, Axe);

		HERO_ID_CONST(PUDGE, Pudge);
		HERO_ID_CONST(BUTCHER, Pudge);

		HERO_ID_CONST(SANDKING, SandKing);
		HERO_ID_CONST(SAND_KING, SandKing);
		HERO_ID_CONST(SK, SandKing);
		HERO_ID_CONST(CRIXALIS, SandKing);
		HERO_ID_CONST(CRIX, SandKing);

		HERO_ID_CONST(SLARDAR, Slardar);
		HERO_ID_CONST(SLAR, Slardar);
		HERO_ID_CONST(SLITHEREEN_GUARD, Slardar);
		HERO_ID_CONST(SLITHEREENGUARD, Slardar);

		HERO_ID_CONST(TIDEHUNTER, TideHunter);
		HERO_ID_CONST(TIDE_HUNTER, TideHunter);
		HERO_ID_CONST(TIDE, TideHunter);
		HERO_ID_CONST(TH, TideHunter);
		HERO_ID_CONST(LEVIATHAN, TideHunter);
		HERO_ID_CONST(LEV, TideHunter);

		HERO_ID_CONST(SKELETONKING, SkeletonKing);
		HERO_ID_CONST(SKELETON_KING, SkeletonKing);
		HERO_ID_CONST(SKELE_KING, SkeletonKing);
		HERO_ID_CONST(SKELEKING, SkeletonKing);
		HERO_ID_CONST(SKELE, SkeletonKing);
		HERO_ID_CONST(OSTARION, SkeletonKing);
		HERO_ID_CONST(OST, SkeletonKing);
		HERO_ID_CONST(LEORIC, SkeletonKing);
		HERO_ID_CONST(KING_LEORIC, SkeletonKing);
		HERO_ID_CONST(KINGLEORIC, SkeletonKing);
		HERO_ID_CONST(LEO, SkeletonKing);

		HERO_ID_CONST(LIFESTEALER, Lifestealer);
		HERO_ID_CONST(LIFE_STEALER, Lifestealer);
		HERO_ID_CONST(STEALER, Lifestealer);
		HERO_ID_CONST(LS, Lifestealer);
		HERO_ID_CONST(NAIX, Lifestealer);

		HERO_ID_CONST(NIGHTSTALKER, NightStalker);
		HERO_ID_CONST(NIGHT_STALKER, NightStalker);
		HERO_ID_CONST(STALKER, NightStalker);
		HERO_ID_CONST(NS, NightStalker);
		HERO_ID_CONST(BALANAR, NightStalker);
		HERO_ID_CONST(BAL, NightStalker);

		HERO_ID_CONST(DOOM, Doom);
		HERO_ID_CONST(DOOMBRINGER, Doom);
		HERO_ID_CONST(DOOM_BRINGER, Doom);
		HERO_ID_CONST(LUCIFER, Doom);

		HERO_ID_CONST(SPIRITBREAKER, SpiritBreaker);
		HERO_ID_CONST(SPIRIT_BREAKER, SpiritBreaker);
		HERO_ID_CONST(SB, SpiritBreaker);
		HERO_ID_CONST(BARA, SpiritBreaker);
		HERO_ID_CONST(BARATHRUM, SpiritBreaker);
		HERO_ID_CONST(BREAKER, SpiritBreaker);

		HERO_ID_CONST(LYCAN, Lycan);
		HERO_ID_CONST(LYCANTHROPE, Lycan);
		HERO_ID_CONST(BANEHALLOW, Lycan);

		HERO_ID_CONST(CHAOS_KNIGHT, ChaosKnight);
		HERO_ID_CONST(CHAOSKNIGHT, ChaosKnight);
		HERO_ID_CONST(CHAOS, ChaosKnight);
		HERO_ID_CONST(CK, ChaosKnight);
		HERO_ID_CONST(NESSAJ, ChaosKnight);
		HERO_ID_CONST(NESS, ChaosKnight);

		HERO_ID_CONST(UNDYING, Undying);
		HERO_ID_CONST(DIRGE, Undying);
		HERO_ID_CONST(ALMIGHTYDIRGE, Undying);
		HERO_ID_CONST(ALMIGHTY_DIRGE, Undying);

		HERO_ID_CONST(MAGNUS, Magnus);
		HERO_ID_CONST(MAGNATAUR, Magnus);
		HERO_ID_CONST(MAGNOCEROS, Magnus);

		HERO_ID_CONST(ABADDON, Abaddon);
		HERO_ID_CONST(LORDOFAVERNUS, Abaddon);
		HERO_ID_CONST(LORD_OF_AVERNUS, Abaddon);


		//Radiant Agility
		HERO_ID_CONST(ANTI_MAGE, AntiMage);
		HERO_ID_CONST(ANTIMAGE, AntiMage);
		HERO_ID_CONST(AM, AntiMage);
		HERO_ID_CONST(MAG, AntiMage);
		HERO_ID_CONST(MAGINA, AntiMage);

		HERO_ID_CONST(DROW_RANGER, DrowRanger);
		HERO_ID_CONST(DROWRANGER, DrowRanger);
		HERO_ID_CONST(DROW, DrowRanger);
		HERO_ID_CONST(TRAXEX, DrowRanger);
		HERO_ID_CONST(TRAX, DrowRanger);
		HERO_ID_CONST(DR, DrowRanger);

		HERO_ID_CONST(JUGGERNAUT, Juggernaut);
		HERO_ID_CONST(JUG, Juggernaut);
		HERO_ID_CONST(JUGG, Juggernaut);
		HERO_ID_CONST(JUGGER, Juggernaut);
		HERO_ID_CONST(YURNERO, Juggernaut);
		HERO_ID_CONST(YURN, Juggernaut);

		HERO_ID_CONST(MIRANA, Mirana);
		HERO_ID_CONST(MIRANA_NIGHTSHADE, Mirana);
		HERO_ID_CONST(MIRANANIGHTSHADE, Mirana);
		HERO_ID_CONST(POTM, Mirana);
		HERO_ID_CONST(PRIESTESS, Mirana);
		HERO_ID_CONST(PRIESTESS_OF_THE_MOON, Mirana);
		HERO_ID_CONST(PRIESTESSOFTHEMOON, Mirana);

		HERO_ID_CONST(MORPH, Morphling);
		HERO_ID_CONST(MORPHLING, Morphling);

		HERO_ID_CONST(PHANTOMLANCER, PhantomLancer);
		HERO_ID_CONST(PHANTOM_LANCER, PhantomLancer);
		HERO_ID_CONST(PL, PhantomLancer);
		HERO_ID_CONST(LANCER, PhantomLancer);
		HERO_ID_CONST(AZWRAITH, PhantomLancer);
		HERO_ID_CONST(AZ, PhantomLancer);

		HERO_ID_CONST(VENGE, VengefulSpirit);
		HERO_ID_CONST(VENGEFUL, VengefulSpirit);
		HERO_ID_CONST(VENGEFULSPIRIT, VengefulSpirit);
		HERO_ID_CONST(VENGEFUL_SPIRIT, VengefulSpirit);
		HERO_ID_CONST(VS, VengefulSpirit);
		HERO_ID_CONST(SHENDELZARE, VengefulSpirit);
		HERO_ID_CONST(SHENDELZARESILKWOOD, VengefulSpirit);
		HERO_ID_CONST(SHENDELZARE_SILKWOOD, VengefulSpirit);
		HERO_ID_CONST(SHENDEL, VengefulSpirit);
		HERO_ID_CONST(SHEND, VengefulSpirit);
		HERO_ID_CONST(SHEN, VengefulSpirit);

		HERO_ID_CONST(RIKI, Riki);
		HERO_ID_CONST(RIKIMARU, Riki);
		HERO_ID_CONST(SA, Riki);
		HERO_ID_CONST(STEALTH_ASSASSIN, Riki);
		HERO_ID_CONST(STEALTHASSASSIN, Riki);

		HERO_ID_CONST(SNIPER, Sniper);
		HERO_ID_CONST(DWARVENSNIPER, Sniper);
		HERO_ID_CONST(DWARVEN_SNIPER, Sniper);
		HERO_ID_CONST(SHARPEYE, Sniper);
		HERO_ID_CONST(KARDEL, Sniper);
		HERO_ID_CONST(KS, Sniper);
		HERO_ID_CONST(KARDELSHARPEYE, Sniper);
		HERO_ID_CONST(KARDEL_SHARPEYE, Sniper);

		HERO_ID_CONST(TA, TemplarAssassin);
		HERO_ID_CONST(TEMPLARASSASSIN, TemplarAssassin);
		HERO_ID_CONST(TEMPLAR_ASSASSIN, TemplarAssassin);
		HERO_ID_CONST(LAN, TemplarAssassin);
		HERO_ID_CONST(LANAYA, TemplarAssassin);

		HERO_ID_CONST(LUNA, Luna);
		HERO_ID_CONST(LUNA_MOONFANG, Luna);
		HERO_ID_CONST(LUNAMOONFANG, Luna);
		HERO_ID_CONST(MOONRIDER, Luna);
		HERO_ID_CONST(MOON_RIDER, Luna);

		HERO_ID_CONST(BOUNTY_HUNTER, BountyHunter);
		HERO_ID_CONST(BOUNTYHUNTER, BountyHunter);
		HERO_ID_CONST(BOUNTY, BountyHunter);
		HERO_ID_CONST(BH, BountyHunter);
		HERO_ID_CONST(GON, BountyHunter);
		HERO_ID_CONST(GOND, BountyHunter);
		HERO_ID_CONST(GONDAR, BountyHunter);

		HERO_ID_CONST(URSA, Ursa);
		HERO_ID_CONST(URSAWARRIOR, Ursa);
		HERO_ID_CONST(URSA_WARRIOR, Ursa);
		HERO_ID_CONST(ULFSAAR, Ursa);

		HERO_ID_CONST(GYRO, Gyrocopter);
		HERO_ID_CONST(GYROCOPTER, Gyrocopter);
		HERO_ID_CONST(AUREL, Gyrocopter);
		HERO_ID_CONST(AURELVLAICU, Gyrocopter);
		HERO_ID_CONST(AUREL_VLAICU, Gyrocopter);

		HERO_ID_CONST(LONE, LoneDruid);
		HERO_ID_CONST(DRUID, LoneDruid);
		HERO_ID_CONST(LONE_DRUID, LoneDruid);
		HERO_ID_CONST(LONEDRUID, LoneDruid);
		HERO_ID_CONST(SYLLA, LoneDruid);
		HERO_ID_CONST(SYLLABEAR, LoneDruid);
		HERO_ID_CONST(LD, LoneDruid);

		HERO_ID_CONST(NAGA, NagaSiren);
		HERO_ID_CONST(NAGA_SIREN, NagaSiren);
		HERO_ID_CONST(NAGASIREN, NagaSiren);
		HERO_ID_CONST(SLITH, NagaSiren);
		HERO_ID_CONST(SLITHICE, NagaSiren);

		HERO_ID_CONST(TROLL, TrollWarlord);
		HERO_ID_CONST(TROLLWARLORD, TrollWarlord);
		HERO_ID_CONST(TROLL_WARLORD, TrollWarlord);
		HERO_ID_CONST(TW, TrollWarlord);
		HERO_ID_CONST(JAHRAKAL, TrollWarlord);
		HERO_ID_CONST(JAHRA, TrollWarlord);


		//Dire Agility
		HERO_ID_CONST(BLOODSEEKER, Bloodseeker);
		HERO_ID_CONST(BLOOD, Bloodseeker);
		HERO_ID_CONST(BLOOD_SEEKER, Bloodseeker);
		HERO_ID_CONST(BS, Bloodseeker);
		HERO_ID_CONST(STRYGWYR, Bloodseeker);

		HERO_ID_CONST(SHADOWFIEND, ShadowFiend);
		HERO_ID_CONST(SHADOW_FIEND, ShadowFiend);
		HERO_ID_CONST(NEVERMORE, ShadowFiend);
		HERO_ID_CONST(NEVER, ShadowFiend);
		HERO_ID_CONST(SF, ShadowFiend);

		HERO_ID_CONST(RAZOR, Razor);
		HERO_ID_CONST(LIGHTNINGREVENANT, Razor);
		HERO_ID_CONST(LIGHTNING_REVENANT, Razor);

		HERO_ID_CONST(VENO, Venomancer);
		HERO_ID_CONST(VENOMANCER, Venomancer);
		HERO_ID_CONST(LESALE, Venomancer);
		HERO_ID_CONST(DEATHBRINGER, Venomancer);
		HERO_ID_CONST(LESALEDEATHBRINGER, Venomancer);
		HERO_ID_CONST(LESALE_DEATHBRINGER, Venomancer);

		HERO_ID_CONST(VOID, FacelessVoid);
		HERO_ID_CONST(FACELESSVOID, FacelessVoid);
		HERO_ID_CONST(FACELESS_VOID, FacelessVoid);
		HERO_ID_CONST(DARKTERROR, FacelessVoid);

		HERO_ID_CONST(MORTRED, PhantomAssassin);
		HERO_ID_CONST(MORT, PhantomAssassin);
		HERO_ID_CONST(PHANTOMASSASSIN, PhantomAssassin);
		HERO_ID_CONST(PHANTOM_ASSASSIN, PhantomAssassin);
		HERO_ID_CONST(PA, PhantomAssassin);

		HERO_ID_CONST(VIPER, Viper);
		HERO_ID_CONST(NETHERDRAKE, Viper);
		HERO_ID_CONST(NETHER, Viper);

		HERO_ID_CONST(CLINKZ, Clinkz);
		HERO_ID_CONST(FLETCHER, Clinkz);
		HERO_ID_CONST(BONEFLETCHER, Clinkz);
		HERO_ID_CONST(BONE_FLETCHER, Clinkz);

		HERO_ID_CONST(BROOD, Broodmother);
		HERO_ID_CONST(BROODMOTHER, Broodmother);
		HERO_ID_CONST(BLACKARACHNIA, Broodmother);
		HERO_ID_CONST(BLACK_ARACHNIA, Broodmother);

		HERO_ID_CONST(WEAVER, Weaver);
		HERO_ID_CONST(NERUBIANWEAVER, Weaver);
		HERO_ID_CONST(NERUBIAN_WEAVER, Weaver);
		HERO_ID_CONST(SKITSKURR, Weaver);
		HERO_ID_CONST(ANUBSERAN, Weaver);
		HERO_ID_CONST(ANUB_SERAN, Weaver);
		HERO_ID_CONST(SERAN, Weaver);

		HERO_ID_CONST(SPECTRE, Spectre);
		HERO_ID_CONST(MERCURIAL, Spectre);

		HERO_ID_CONST(MEEPO, Meepo);
		HERO_ID_CONST(GEO, Meepo);
		HERO_ID_CONST(GEOMANCER, Meepo);

		HERO_ID_CONST(NYX, NyxAssassin);
		HERO_ID_CONST(NA, NyxAssassin);
		HERO_ID_CONST(NYXASSASSIN, NyxAssassin);
		HERO_ID_CONST(NYX_ASSASSIN, NyxAssassin);
		HERO_ID_CONST(NERUBIANASSASSIN, NyxAssassin);
		HERO_ID_CONST(NERUBIAN_ASSASSIN, NyxAssassin);
		HERO_ID_CONST(ANUBARAK, NyxAssassin);
		HERO_ID_CONST(ANUB_ARAK, NyxAssassin);
		HERO_ID_CONST(ARAK, NyxAssassin);

		HERO_ID_CONST(SLARK, Slark);
		HERO_ID_CONST(NIGHTCRAWLER, Slark);
		HERO_ID_CONST(MURLOCNIGHTCRAWLER, Slark);
		HERO_ID_CONST(MURLOC_NIGHTCRAWLER, Slark);
		HERO_ID_CONST(MURLOC_NIGHT_CRAWLER, Slark);
		HERO_ID_CONST(NIGHT_CRAWLER, Slark);

		HERO_ID_CONST(MEDUSA, Medusa);
		HERO_ID_CONST(GORGON, Medusa);


		//Radiant Intelligence
		HERO_ID_CONST(MAIDEN, CrystalMaiden);
		HERO_ID_CONST(CM, CrystalMaiden);
		HERO_ID_CONST(CRYSTALMAIDEN, CrystalMaiden);
		HERO_ID_CONST(CRYSTAL_MAIDEN, CrystalMaiden);
		HERO_ID_CONST(RYLAI, CrystalMaiden);
		HERO_ID_CONST(RYLAICRESTFALL, CrystalMaiden);
		HERO_ID_CONST(RYLAI_CRESTFALL, CrystalMaiden);

		HERO_ID_CONST(PUCK, Puck);
		HERO_ID_CONST(FAERIEDRAGON, Puck);
		HERO_ID_CONST(FAERIE_DRAGON, Puck);
		HERO_ID_CONST(FAERIE, Puck);

		HERO_ID_CONST(STORM, StormSpirit);
		HERO_ID_CONST(STORMSPIRIT, StormSpirit);
		HERO_ID_CONST(STORM_SPIRIT, StormSpirit);
		HERO_ID_CONST(RAIJIN, StormSpirit);
		HERO_ID_CONST(RAIJINTHUNDERKEG, StormSpirit);
		HERO_ID_CONST(RAIJIN_THUNDERKEG, StormSpirit);

		HERO_ID_CONST(WINDRUNNER, Windrunner);
		HERO_ID_CONST(WIND_RUNNER, Windrunner);
		HERO_ID_CONST(WR, Windrunner);
		HERO_ID_CONST(WIND, Windrunner);
		HERO_ID_CONST(LYRALEI, Windrunner);
		HERO_ID_CONST(ALLERIA, Windrunner);

		HERO_ID_CONST(ZEUS, Zeus);
		HERO_ID_CONST(LORDOFHEAVEN, Zeus);
		HERO_ID_CONST(LORD_OF_HEAVEN, Zeus);
		HERO_ID_CONST(LORD_OF_OLYMPUS, Zeus);
		HERO_ID_CONST(LORDOFOLYMPUS, Zeus);

		HERO_ID_CONST(SLAYER, Lina);
		HERO_ID_CONST(LINA, Lina);
		HERO_ID_CONST(LINAINVERSE, Lina);
		HERO_ID_CONST(LINA_INVERSE, Lina);

		HERO_ID_CONST(RHASTA, ShadowShaman);
		HERO_ID_CONST(SHAMAN, ShadowShaman);
		HERO_ID_CONST(SHADOWSHAMAN, ShadowShaman);
		HERO_ID_CONST(SHADOW_SHAMAN, ShadowShaman);

		HERO_ID_CONST(TINKER, Tinker);
		HERO_ID_CONST(BOUSH, Tinker);

		HERO_ID_CONST(FURION, Furion);
		HERO_ID_CONST(NATURESPROPHET, Furion);
		HERO_ID_CONST(NATURES_PROPHET, Furion);
		HERO_ID_CONST(PROPHET, Furion);
		HERO_ID_CONST(TEQUOIA, Furion);
		HERO_ID_CONST(NP, Furion);

		HERO_ID_CONST(ENCHANT, Enchantress);
		HERO_ID_CONST(ENCHANTRESS, Enchantress);
		HERO_ID_CONST(AIUSHTHA, Enchantress);

		HERO_ID_CONST(JAK, Jakiro);
		HERO_ID_CONST(JAKIRO, Jakiro);
		HERO_ID_CONST(TWINHEADDRAGON, Jakiro);
		HERO_ID_CONST(TWIN_HEAD_DRAGON, Jakiro);
		HERO_ID_CONST(THD, Jakiro);

		HERO_ID_CONST(CHEN, Chen);
		HERO_ID_CONST(HOLYKNIGHT, Chen);
		HERO_ID_CONST(HOLY_KNIGHT, Chen);

		HERO_ID_CONST(SILENCER, Silencer);
		HERO_ID_CONST(NORT, Silencer);
		HERO_ID_CONST(NORTROM, Silencer);

		HERO_ID_CONST(OM, OgreMagi);
		HERO_ID_CONST(OGRE, OgreMagi);
		HERO_ID_CONST(OGREMAGI, OgreMagi);
		HERO_ID_CONST(AGGRON, OgreMagi);
		HERO_ID_CONST(AGGRONSTONEBREAK, OgreMagi);
		HERO_ID_CONST(AGGRON_STONEBREAK, OgreMagi);
		HERO_ID_CONST(STONEBREAK, OgreMagi);

		HERO_ID_CONST(RUBICK, Rubick);
		HERO_ID_CONST(GRANDMAGUS, Rubick);
		HERO_ID_CONST(GRAND_MAGUS, Rubick);

		HERO_ID_CONST(DISRUPTOR, Disruptor);
		HERO_ID_CONST(DISRUPTOR, Disruptor);
		HERO_ID_CONST(STORMCRAFTER, Disruptor);
		HERO_ID_CONST(STORM_CRAFTER, Disruptor);
		HERO_ID_CONST(THRALL, Disruptor);

		HERO_ID_CONST(KOTL, KeeperOfTheLight);
		HERO_ID_CONST(KEEPEROFTHELIGHT, KeeperOfTheLight);
		HERO_ID_CONST(KEEPER_OF_THE_LIGHT, KeeperOfTheLight);
		HERO_ID_CONST(EZALOR, KeeperOfTheLight);

		HERO_ID_CONST(SM, SkywrathMage);
		HERO_ID_CONST(SKY, SkywrathMage);
		HERO_ID_CONST(SKYWRATH, SkywrathMage);
		HERO_ID_CONST(SKYWRATHMAGE, SkywrathMage);
		HERO_ID_CONST(SKYWRATH_MAGE, SkywrathMage);
		HERO_ID_CONST(DRAGONUS, SkywrathMage);


		//Dire Intelligence
		HERO_ID_CONST(BANE, Bane);
		HERO_ID_CONST(BANEELEMENTAL, Bane);
		HERO_ID_CONST(BANE_ELEMENTAL, Bane);
		HERO_ID_CONST(ATROPOS, Bane);
		HERO_ID_CONST(ATROP, Bane);

		HERO_ID_CONST(LICH, Lich);
		HERO_ID_CONST(ETH, Lich);
		HERO_ID_CONST(ETHREAIN, Lich);
		HERO_ID_CONST(KEL, Lich);
		HERO_ID_CONST(KELTHUZAD, Lich);
		HERO_ID_CONST(KEL_THUZAD, Lich);

		HERO_ID_CONST(LION, Lion);
		HERO_ID_CONST(DEMONWITCH, Lion);
		HERO_ID_CONST(DEMON_WITCH, Lion);

		HERO_ID_CONST(WD, WitchDoctor);
		HERO_ID_CONST(WITCHDOCTOR, WitchDoctor);
		HERO_ID_CONST(WITCH_DOCTOR, WitchDoctor);
		HERO_ID_CONST(ZHARVAKKO, WitchDoctor);
		HERO_ID_CONST(VOLJIN, WitchDoctor);
		HERO_ID_CONST(VOL_JIN, WitchDoctor);

		HERO_ID_CONST(ENIGMA, Enigma);
		HERO_ID_CONST(DARCHROW, Enigma);
		HERO_ID_CONST(DARCH, Enigma);

		HERO_ID_CONST(NECRO, Necrolyte);
		HERO_ID_CONST(NECROLYTE, Necrolyte);
		HERO_ID_CONST(ROTUND, Necrolyte);
		HERO_ID_CONST(ROTUNDJERE, Necrolyte);
		HERO_ID_CONST(ROTUND_JERE, Necrolyte);

		HERO_ID_CONST(WARLOCK, Warlock);
		HERO_ID_CONST(DEMNOK, Warlock);
		HERO_ID_CONST(DEMNOK_LANNIK, Warlock);
		HERO_ID_CONST(DEMNOKLANNIK, Warlock);

		HERO_ID_CONST(QOP, QueenOfPain);
		HERO_ID_CONST(QUEENOFPAIN, QueenOfPain);
		HERO_ID_CONST(QUEEN_OF_PAIN, QueenOfPain);
		HERO_ID_CONST(AKASHA, QueenOfPain);

		HERO_ID_CONST(DP, DeathProphet);
		HERO_ID_CONST(DEATHPROPHET, DeathProphet);
		HERO_ID_CONST(DEATH_PROPHET, DeathProphet);
		HERO_ID_CONST(KROB, DeathProphet);
		HERO_ID_CONST(KROBELUS, DeathProphet);

		HERO_ID_CONST(PUGNA, Pugna);
		HERO_ID_CONST(OBLIVION, Pugna);

		HERO_ID_CONST(DAZZLE, Dazzle);
		HERO_ID_CONST(SHADOWPRIEST, Dazzle);
		HERO_ID_CONST(SHADOW_PRIEST, Dazzle);

		HERO_ID_CONST(LESH, Leshrac);
		HERO_ID_CONST(LESHRAC, Leshrac);
		HERO_ID_CONST(MALICIOUS, Leshrac);
		HERO_ID_CONST(TORMENTEDSOUL, Leshrac);
		HERO_ID_CONST(TORMENTED_SOUL, Leshrac);

		HERO_ID_CONST(DS, DarkSeer);
		HERO_ID_CONST(DARKSEER, DarkSeer);
		HERO_ID_CONST(DARK_SEER, DarkSeer);
		HERO_ID_CONST(SEER, DarkSeer);
		HERO_ID_CONST(ISH, DarkSeer);
		HERO_ID_CONST(ISHKAFEl, DarkSeer);
		HERO_ID_CONST(ISH_KAFEl, DarkSeer);

		HERO_ID_CONST(BAT, Batrider);
		HERO_ID_CONST(BATRIDER, Batrider);
		HERO_ID_CONST(BAT_RIDER, Batrider);
		HERO_ID_CONST(JINZAKK, Batrider);
		HERO_ID_CONST(JIN_ZAKK, Batrider);

		HERO_ID_CONST(AA, AncientApparition);
		HERO_ID_CONST(ANCIENTAPPARITION, AncientApparition);
		HERO_ID_CONST(ANCIENT_APPARITION, AncientApparition);
		HERO_ID_CONST(APPARITION, AncientApparition);
		HERO_ID_CONST(KALDR, AncientApparition);

		HERO_ID_CONST(INVOKER, Invoker);
		HERO_ID_CONST(VOKER, Invoker);
		HERO_ID_CONST(KAEL, Invoker);
		HERO_ID_CONST(CARL, Invoker);
		HERO_ID_CONST(ARSENALMAGUS, Invoker);
		HERO_ID_CONST(ARSENAL_MAGUS, Invoker);

		HERO_ID_CONST(OD, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIANDESTROYER, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIAN_DESTROYER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLD_DESTROYER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLDDESTROYER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLDDEVOURER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLD_DEVOURER, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIANDEVOURER, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIAN_DEVOURER, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIANDEMOLISHER, ObsidianDestroyer);
		HERO_ID_CONST(OBSIDIAN_DEMOLISHER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLD_DEMOLISHER, ObsidianDestroyer);
		HERO_ID_CONST(OUTWORLDDEMOLISHER, ObsidianDestroyer);
		HERO_ID_CONST(HARBRINGER, ObsidianDestroyer);
		HERO_ID_CONST(HARB, ObsidianDestroyer);
		HERO_ID_CONST(HAMBURGER_DEVOURER, ObsidianDestroyer);

		HERO_ID_CONST(SHADOWDEMON, ShadowDemon);
		HERO_ID_CONST(SD, ShadowDemon);
		HERO_ID_CONST(SHADOW_DEMON, ShadowDemon);
		HERO_ID_CONST(EREDAR, ShadowDemon);

		HERO_ID_CONST(VIS, Visage);
		HERO_ID_CONST(VISAGE, Visage);
		HERO_ID_CONST(NECROLIC, Visage);
		HERO_ID_CONST(BOUNDFORMOFNECROLIC, Visage);
		HERO_ID_CONST(BOUND_FORM_OF_NECROLIC, Visage);


		// Other
		HERO_ID_CONST(GANDALF, KeeperOfTheLight);
		HERO_ID_CONST(BATHROOM, SpiritBreaker);
		HERO_ID_CONST(SHINY_HORSE_MAN, Leshrac);
		HERO_ID_CONST(CANCER_LANCER, PhantomLancer);
		HERO_ID_CONST(NATURES_PROFIT, Furion);
		HERO_ID_CONST(NYX_NYX_NYX_NYX, NyxAssassin);

#undef HERO_ID_CONST

		auto heroArray = v8::Array::New();
		int cur = 0;
		for(int i = 0; i < Hero_Count; ++i){
			auto v = HeroIdToClassname(i);
			if(v == NULL) continue;
			heroArray->Set(cur++, v8::String::New(v));
		}

		proto->Set("heroes", heroArray);
	}

	
};
