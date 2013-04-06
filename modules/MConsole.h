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

	WRAPPED_CLS(MConsole, SMJS_Module) {
		temp->SetClassName(v8::String::New("ConsoleModule"));
		
		WRAPPED_FUNC(addServerCommand);
		WRAPPED_FUNC(addClientCommand);

	}


private:
	bool InvokePluginsCommand(const CCommand &command);
	bool InvokePluginsCommand(int client, int argc, const char **argArray, const char *argString);

	static void CommandCallback(const CCommand &command);
	static void CommandCallback2(const CCommand &command);
	static void AddCommand(SMJS_Plugin *pl, const char *cmdName, v8::Handle<v8::Function> func, bool serverOnly);
	static void OnSayCommand_Pre(const CCommand &command);
	static void OnSayCommand_Post(const CCommand &command);
};
