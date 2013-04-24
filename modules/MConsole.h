#pragma once
#include "SMJS_Module.h"
#include <map>
#include <algorithm>


struct CommandHook {
	SMJS_Plugin *pl;
	v8::Persistent<v8::Function> func;
	bool serverOnly;
};

struct CommandHookInfo {
	std::vector<CommandHook*> hooks;
	bool isSMJS;
};

class MConsole : public SMJS_Module {
public:
	std::map<std::string, CommandHookInfo*> pluginCmds2;


	int cmdClient;
	ConCommand* cmdSay;
	ConCommand* cmdSayTeam;

	MConsole();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void OnPluginDestroyed(SMJS_Plugin *plugin);

	void MConsole::SetCommandClient(int client){
		cmdClient = client + 1;
	}

	FUNCTION_DECL(addServerCommand);
	FUNCTION_DECL(addClientCommand);
	FUNCTION_DECL(findConVar);

	WRAPPED_CLS(MConsole, SMJS_Module) {
		temp->SetClassName(v8::String::New("ConsoleModule"));
		
#define FCVAR_TYPE(type) proto->Set(#type, v8::Int32::New(type));

		FCVAR_TYPE(FCVAR_NONE);
		FCVAR_TYPE(FCVAR_UNREGISTERED);
		FCVAR_TYPE(FCVAR_DEVELOPMENTONLY);
		FCVAR_TYPE(FCVAR_GAMEDLL);
		FCVAR_TYPE(FCVAR_CLIENTDLL);
		FCVAR_TYPE(FCVAR_HIDDEN);
		FCVAR_TYPE(FCVAR_PROTECTED);
		FCVAR_TYPE(FCVAR_SPONLY);
		FCVAR_TYPE(FCVAR_ARCHIVE);
		FCVAR_TYPE(FCVAR_NOTIFY);
		FCVAR_TYPE(FCVAR_USERINFO);
		FCVAR_TYPE(FCVAR_PRINTABLEONLY);
		FCVAR_TYPE(FCVAR_UNLOGGED);
		FCVAR_TYPE(FCVAR_NEVER_AS_STRING);
		FCVAR_TYPE(FCVAR_REPLICATED);
		FCVAR_TYPE(FCVAR_CHEAT);
		FCVAR_TYPE(FCVAR_SS);
		FCVAR_TYPE(FCVAR_DEMO);
		FCVAR_TYPE(FCVAR_DONTRECORD);
		FCVAR_TYPE(FCVAR_SS_ADDED);
		FCVAR_TYPE(FCVAR_RELEASE);
		FCVAR_TYPE(FCVAR_RELOAD_MATERIALS);
		FCVAR_TYPE(FCVAR_RELOAD_TEXTURES);
		FCVAR_TYPE(FCVAR_NOT_CONNECTED);
		FCVAR_TYPE(FCVAR_MATERIAL_SYSTEM_THREAD);
		FCVAR_TYPE(FCVAR_ARCHIVE_XBOX);
		FCVAR_TYPE(FCVAR_SERVER_CAN_EXECUTE);
		FCVAR_TYPE(FCVAR_SERVER_CANNOT_QUERY);
		FCVAR_TYPE(FCVAR_CLIENTCMD_CAN_EXECUTE);

#undef FCVAR_TYPE

		WRAPPED_FUNC(addServerCommand);
		WRAPPED_FUNC(addClientCommand);
		WRAPPED_FUNC(findConVar);
	}


private:
	bool InvokePluginsCommand(const CCommand &command);
	bool InvokePluginsCommand(int client, int argc, const char **argArray, const char *argString);

	static void CommandCallback(void *pUnknown, const CCommand &command);
	static void CommandCallback2(void *pUnknown, const CCommand &command);
	static void AddCommand(SMJS_Plugin *pl, const char *cmdName, v8::Handle<v8::Function> func, bool serverOnly);
	static void OnSayCommand_Pre(void *pUnknown, const CCommand &command);
	static void OnSayCommand_Post(void *pUnknown, const CCommand &command);
};
