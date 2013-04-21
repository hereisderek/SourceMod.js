#include "modules/MDota.h"
#include "SMJS_Plugin.h"
#include "modules/MEntities.h"
#include "CDetour/detours.h"
#include "SMJS_Entity.h"



WRAPPED_CLS_CPP(MDota, SMJS_Module);

IGameConfig *dotaConf = NULL;
void *LoadParticleFile;
void *CreateUnit;


CDetour *parseUnitDetour;
CDetour *playerPickHeroDetour;

DETOUR_DECL_MEMBER3(ParseUnit, void*, void*, a2, void*, a3, void*, a4){
	void *ret = DETOUR_MEMBER_CALL(ParseUnit)(a2, a3, a4);
	SMJS_Entity *entWrapper = GetEntityWrapper((CBaseEntity*) this);
	
	int len = GetNumPlugins();
	for(int i = 0; i < len; ++i){
		SMJS_Plugin *pl = GetPlugin(i);
		if(pl == NULL) continue;
		
		HandleScope handle_scope(pl->GetIsolate());
		Context::Scope context_scope(pl->GetContext());

		auto hooks = pl->GetHooks("Dota_OnUnitParsed");

		for(auto it = hooks->begin(); it != hooks->end(); ++it){
			auto func = *it;
			func->Call(pl->GetContext()->Global(), 1, &(entWrapper->GetWrapper(pl)));
		}
	}

	return ret;
}

DETOUR_DECL_MEMBER2(PlayerPickHero, int, int, a1, char*, hero){

	// Repick
	if(hero == NULL){
		return DETOUR_MEMBER_CALL(PlayerPickHero)(a1, hero);
	}

	int len = GetNumPlugins();
	for(int i = 0; i < len; ++i){
		SMJS_Plugin *pl = GetPlugin(i);
		if(pl == NULL) continue;
		
		HandleScope handle_scope(pl->GetIsolate());
		Context::Scope context_scope(pl->GetContext());

		auto hooks = pl->GetHooks("Dota_OnHeroPicked");
		v8::Handle<v8::Value> args = v8::String::New(hero);

		for(auto it = hooks->begin(); it != hooks->end(); ++it){
			auto func = *it;
			auto ret = func->Call(pl->GetContext()->Global(), 1, &args);
			if(!ret.IsEmpty() && ret->IsString()){
				v8::String::AsciiValue ascii(ret);
				return DETOUR_MEMBER_CALL(PlayerPickHero)(a1, *ascii);
			}
		}
	}

	return DETOUR_MEMBER_CALL(PlayerPickHero)(a1, hero);
}


void* GetValueForLevel_Actual = NULL;
__declspec(naked) void* GetValueForLevel(void *a4){
	static const char *str1 = "%p = %p\n";
	const char *a1;
	const char *ability;
	void *output;
	void *result;

	__asm {
		push	ebp
		mov		ebp, esp
		sub		esp, 16

		mov		a1, eax
		mov		ability, ecx
		mov		output, edi

		push	[ebp + 8]
		call	GetValueForLevel_Actual

		mov		result, eax

		mov		eax, ability
		test	eax, eax
		jz		end

		pushad
		mov		eax, result
		push	eax
		mov		eax, ability
		push	eax
		push	str1
		call	printf
		add		esp, 0Ch
		popad

end:
		mov eax, result

		mov esp, ebp
		pop ebp
		ret 4
	}
}

MDota::MDota(){
	identifier = "dota";

	char conf_error[255] = "";
	if (!gameconfs->LoadGameConfigFile("smjs.dota", &dotaConf, conf_error, sizeof(conf_error))){
		if (conf_error){
			smutils->LogError(myself, "Could not read smjs.dota.txt: %s", conf_error);
			return;
		}
	}

	CDetourManager::Init(g_pSM->GetScriptingEngine(), dotaConf);


	parseUnitDetour = DETOUR_CREATE_MEMBER(ParseUnit, "ParseUnit");
	if(!parseUnitDetour){
		smutils->LogError(myself, "Unable to hook ParseUnit!");
		return;
	}
	
	parseUnitDetour->EnableDetour();

	playerPickHeroDetour = DETOUR_CREATE_MEMBER(PlayerPickHero, "PlayerPickHero");
	if(!playerPickHeroDetour){
		smutils->LogError(myself, "Unable to hook PlayerPickHero!");
		return;
	}
	
	playerPickHeroDetour->EnableDetour();

	if(!dotaConf->GetMemSig("LoadParticleFile", &LoadParticleFile) || LoadParticleFile == NULL){
		smutils->LogError(myself, "Couldn't sigscan LoadParticleFile");
		return;
	}

	if(!dotaConf->GetMemSig("CreateUnit", &CreateUnit) || CreateUnit == NULL){
		smutils->LogError(myself, "Couldn't sigscan CreateUnit");
		return;
	}
}

void MDota::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	
}

FUNCTION_M(MDota::loadParticleFile)
	ARG_COUNT(1);
	PSTR(file);
	char *fileStr = *file;
	__asm{
		mov eax, fileStr
		call LoadParticleFile
	}

	RETURN_UNDEF;
END

const char* MDota::HeroIdToClassname(int id) {
	switch(id){
		case Hero_Base: return "npc_dota_hero_base";
		case Hero_AntiMage: return "npc_dota_hero_antimage";
		case Hero_Axe: return "npc_dota_hero_axe";
		case Hero_Bane: return "npc_dota_hero_bane";
		case Hero_Bloodseeker: return "npc_dota_hero_bloodseeker";
		case Hero_CrystalMaiden: return "npc_dota_hero_crystal_maiden";
		case Hero_DrowRanger: return "npc_dota_hero_drow_ranger";
		case Hero_EarthShaker: return "npc_dota_hero_earthshaker";
		case Hero_Juggernaut: return "npc_dota_hero_juggernaut";
		case Hero_Mirana: return "npc_dota_hero_mirana";
		case Hero_Morphling: return "npc_dota_hero_morphling";
		case Hero_ShadowFiend: return "npc_dota_hero_nevermore";
		case Hero_PhantomLancer: return "npc_dota_hero_phantom_lancer";
		case Hero_Puck: return "npc_dota_hero_puck";
		case Hero_Pudge: return "npc_dota_hero_pudge";
		case Hero_Razor: return "npc_dota_hero_razor";
		case Hero_SandKing: return "npc_dota_hero_sandking";
		case Hero_StormSpirit: return "npc_dota_hero_storm_spirit";
		case Hero_Sven: return "npc_dota_hero_sven";
		case Hero_Tiny: return "npc_dota_hero_tiny";
		case Hero_VengefulSpirit: return "npc_dota_hero_vengefulspirit";
		case Hero_Windrunner: return "npc_dota_hero_windrunner";
		case Hero_Zeus: return "npc_dota_hero_zuus";
		case Hero_Kunkka: return "npc_dota_hero_kunkka";
		case Hero_Lina: return "npc_dota_hero_lina";
		case Hero_Lion: return "npc_dota_hero_lion";
		case Hero_ShadowShaman: return "npc_dota_hero_shadow_shaman";
		case Hero_Slardar: return "npc_dota_hero_slardar";
		case Hero_TideHunter: return "npc_dota_hero_tidehunter";
		case Hero_WitchDoctor: return "npc_dota_hero_witch_doctor";
		case Hero_Lich: return "npc_dota_hero_lich";
		case Hero_Riki: return "npc_dota_hero_riki";
		case Hero_Enigma: return "npc_dota_hero_enigma";
		case Hero_Tinker: return "npc_dota_hero_tinker";
		case Hero_Sniper: return "npc_dota_hero_sniper";
		case Hero_Necrolyte: return "npc_dota_hero_necrolyte";
		case Hero_Warlock: return "npc_dota_hero_warlock";
		case Hero_BeastMaster: return "npc_dota_hero_beastmaster";
		case Hero_QueenOfPain: return "npc_dota_hero_queenofpain";
		case Hero_Venomancer: return "npc_dota_hero_venomancer";
		case Hero_FacelessVoid: return "npc_dota_hero_faceless_void";
		case Hero_SkeletonKing: return "npc_dota_hero_skeleton_king";
		case Hero_DeathProphet: return "npc_dota_hero_death_prophet";
		case Hero_PhantomAssassin: return "npc_dota_hero_phantom_assassin";
		case Hero_Pugna: return "npc_dota_hero_pugna";
		case Hero_TemplarAssassin: return "npc_dota_hero_templar_assassin";
		case Hero_Viper: return "npc_dota_hero_viper";
		case Hero_Luna: return "npc_dota_hero_luna";
		case Hero_DragonKnight: return "npc_dota_hero_dragon_knight";
		case Hero_Dazzle: return "npc_dota_hero_dazzle";
		case Hero_Clockwerk: return "npc_dota_hero_rattletrap";
		case Hero_Leshrac: return "npc_dota_hero_leshrac";
		case Hero_Furion: return "npc_dota_hero_furion";
		case Hero_Lifestealer: return "npc_dota_hero_life_stealer";
		case Hero_DarkSeer: return "npc_dota_hero_dark_seer";
		case Hero_Clinkz: return "npc_dota_hero_clinkz";
		case Hero_Omniknight: return "npc_dota_hero_omniknight";
		case Hero_Enchantress: return "npc_dota_hero_enchantress";
		case Hero_Huskar: return "npc_dota_hero_huskar";
		case Hero_NightStalker: return "npc_dota_hero_night_stalker";
		case Hero_Broodmother: return "npc_dota_hero_broodmother";
		case Hero_BountyHunter: return "npc_dota_hero_bounty_hunter";
		case Hero_Weaver: return "npc_dota_hero_weaver";
		case Hero_Jakiro: return "npc_dota_hero_jakiro";
		case Hero_Batrider: return "npc_dota_hero_batrider";
		case Hero_Chen: return "npc_dota_hero_chen";
		case Hero_Spectre: return "npc_dota_hero_spectre";
		case Hero_AncientApparition: return "npc_dota_hero_ancient_apparition";
		case Hero_Doom: return "npc_dota_hero_doom_bringer";
		case Hero_Ursa: return "npc_dota_hero_ursa";
		case Hero_SpiritBreaker: return "npc_dota_hero_spirit_breaker";
		case Hero_Gyrocopter: return "npc_dota_hero_gyrocopter";
		case Hero_Alchemist: return "npc_dota_hero_alchemist";
		case Hero_Invoker: return "npc_dota_hero_invoker";
		case Hero_Silencer: return "npc_dota_hero_silencer";
		case Hero_ObsidianDestroyer: return "npc_dota_hero_obsidian_destroyer";
		case Hero_Lycan: return "npc_dota_hero_lycan";
		case Hero_Brewmaster: return "npc_dota_hero_brewmaster";
		case Hero_ShadowDemon: return "npc_dota_hero_shadow_demon";
		case Hero_LoneDruid: return "npc_dota_hero_lone_druid";
		case Hero_ChaosKnight: return "npc_dota_hero_chaos_knight";
		case Hero_Meepo: return "npc_dota_hero_meepo";
		case Hero_TreantProtector: return "npc_dota_hero_treant";
		case Hero_OgreMagi: return "npc_dota_hero_ogre_magi";
		case Hero_Undying: return "npc_dota_hero_undying";
		case Hero_Rubick: return "npc_dota_hero_rubick";
		case Hero_Disruptor: return "npc_dota_hero_disruptor";
		case Hero_NyxAssassin: return "npc_dota_hero_nyx_assassin";
		case Hero_NagaSiren: return "npc_dota_hero_naga_siren";
		case Hero_KeeperOfTheLight: return "npc_dota_hero_keeper_of_the_light";
		case Hero_Wisp: return "npc_dota_hero_wisp";
		case Hero_Visage: return "npc_dota_hero_visage";
		case Hero_Slark: return "npc_dota_hero_slark";
		case Hero_Medusa: return "npc_dota_hero_medusa";
		case Hero_TrollWarlord: return "npc_dota_hero_troll";
		case Hero_CentaurWarchief: return "npc_dota_hero_centaur";
		case Hero_Magnus: return "npc_dota_hero_magnataur";
		case Hero_Timbersaw: return "npc_dota_hero_shredder";
		case Hero_Bristleback: return "npc_dota_hero_bristleback";
		case Hero_Tusk: return "npc_dota_hero_tusk";
		case Hero_SkywrathMage: return "npc_dota_hero_skywrath_mage";
		case Hero_Abaddon: return "npc_dota_hero_abaddon";
		case Hero_ElderTitan: return "npc_dota_hero_elder_titan";
		case Hero_LegionCommander: return "npc_dota_hero_legion_commander";
	}

	return NULL;
}

FUNCTION_M(MDota::heroIdToClassname)
	PINT(id);
	auto ret = HeroIdToClassname(id);
	if(ret == NULL) return v8::Null();
	return v8::String::New(ret);
END

FUNCTION_M(MDota::forceWin)
	PINT(team);
	if(team != 2 && team != 3) THROW_VERB("Invalid team %d", team);

	//WORKAROUND
	auto cmd = icvar->FindCommand("dota_kill_buildings");
	cmd->RemoveFlags(FCVAR_CHEAT);
	engine->ServerCommand("dota_kill_buildings\n");

	return v8::True();
END

FUNCTION_M(MDota::createUnit)
	PSTR(tmp);
	PINT(team);
	CBaseEntity *ent;
	char *clsname = *tmp;
	__asm {
		mov		eax, clsname
		push	0
		push	1
		push	team
		push	0		// Client
		call	CreateUnit
		mov		ent, eax
		add		esp, 10h
	}
	
	if(ent == NULL) return v8::Null();

	return GetEntityWrapper(ent)->GetWrapper(GetPluginRunning());
END