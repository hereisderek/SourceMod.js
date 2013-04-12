#pragma once
#include "SMJS_Module.h"
#include "SMJS_GameRules.h"

class MGame : public SMJS_Module {
public:
	SMJS_GameRules rules;

	MGame();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void Init();

	static CBaseEntity* FindEntityByClassname(int startIndex, char *searchname);
	static CBaseEntity* NativeFindEntityByClassname(int startIndex, char *searchname);

	FUNCTION_DECL(hook);
	FUNCTION_DECL(getTeamClientCount);
	FUNCTION_DECL(precacheModel);
	FUNCTION_DECL(findEntityByClassname);
	FUNCTION_DECL(getTime);

	WRAPPED_CLS(MGame, SMJS_Module) {
		temp->SetClassName(v8::String::New("GameModule"));

		proto->Set("rules", v8::Null());

		WRAPPED_FUNC(hook);
		WRAPPED_FUNC(getTeamClientCount);
		WRAPPED_FUNC(precacheModel);
		WRAPPED_FUNC(findEntityByClassname);
		WRAPPED_FUNC(getTime);
	}

private:
	void InitTeamNatives();
	void OnServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void OnPreServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void OnThink(bool finalTick);
	static void LevelShutdown();
	static void OnGameFrame(bool simulating);
};
