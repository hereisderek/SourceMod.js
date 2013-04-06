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
	// ???? (Techies)
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


	FUNCTION_DECL(heroIdToClassname);
	FUNCTION_DECL(loadParticleFile);
	
	

	WRAPPED_CLS(MDota, SMJS_Module) {
		temp->SetClassName(v8::String::New("DotaModule"));

		WRAPPED_FUNC(heroIdToClassname);
		WRAPPED_FUNC(loadParticleFile);
	}

	
};
