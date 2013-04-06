#pragma once
#include "SMJS_Module.h"

class MGame : public SMJS_Module {
public:
	MGame();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void Init();

	static v8::Handle<v8::Value> FindEntityByClassname(int startIndex, char *searchname);
	static v8::Handle<v8::Value> NativeFindEntityByClassname(int startIndex, char *searchname);

	FUNCTION_DECL(hook);
	FUNCTION_DECL(getTeamClientCount);
	FUNCTION_DECL(precacheModel);
	FUNCTION_DECL(findEntityByClassname);
	FUNCTION_DECL(getGameRules);

	WRAPPED_CLS(MGame, SMJS_Module) {
		temp->SetClassName(v8::String::New("GameModule"));

		WRAPPED_FUNC(hook);
		WRAPPED_FUNC(getTeamClientCount);
		WRAPPED_FUNC(precacheModel);
		WRAPPED_FUNC(findEntityByClassname);
		WRAPPED_FUNC(getGameRules);
	}

private:
	void InitTeamNatives();
	void OnServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void OnPreServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	void OnThink(bool finalTick);
	static void OnGameFrame(bool simulating);
};
