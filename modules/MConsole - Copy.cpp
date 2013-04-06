#include "modules/MConsole.h"
#include "MClient.h"
#include "SMJS_Plugin.h"
#include "extension.h"

WRAPPED_CLS_CPP(MConsole, SMJS_Module)


#if SOURCE_ENGINE >= SE_ORANGEBOX
	SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand &);
#else
	SH_DECL_HOOK0_void(ConCommand, Dispatch, SH_NOATTRIB, false);
#endif

SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, false, int);

MConsole *g_MConsole;

MConsole::MConsole(){
	g_MConsole = this;
	identifier = "console";

	SH_ADD_HOOK(IServerGameClients, SetCommandClient, serverClients, SH_MEMBER(this, &MConsole::SetCommandClient), false);

	cmdSay = icvar->FindCommand("say");
	cmdSayTeam = icvar->FindCommand("say_team");

	Assert(cmdSay != NULL);
	Assert(cmdSayTeam != NULL);

	SH_ADD_HOOK(ConCommand, Dispatch, cmdSay, SH_STATIC(&MConsole::OnSayCommand_Pre), false);
	SH_ADD_HOOK(ConCommand, Dispatch, cmdSay, SH_STATIC(&MConsole::OnSayCommand_Post), true);

	SH_ADD_HOOK(ConCommand, Dispatch, cmdSayTeam, SH_STATIC(&MConsole::OnSayCommand_Pre), false);
	SH_ADD_HOOK(ConCommand, Dispatch, cmdSayTeam, SH_STATIC(&MConsole::OnSayCommand_Post), true);
}

void MConsole::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();

}

void MConsole::OnPluginDestroyed(SMJS_Plugin *plugin){
	for(auto it = pluginCmds2.begin(); it != pluginCmds2.end(); ++it){
		for(auto it2 = it->second->hooks.begin(); it2 != it->second->hooks.end(); ++it2){
			if((*it2)->pl == plugin){
				auto p = *it2;
				it2 = it->second->hooks.erase(it2);
				delete p;
			}
		}
	}
}

bool safestrcat(char *buffer, size_t maxSize, const char *addition){
   if (strlen(buffer) + strlen(addition) + sizeof(char) >= maxSize) return false;
   strcat(buffer,addition);
   return true;
}

void MConsole::OnSayCommand_Pre(const CCommand &command){
	int client = g_MConsole->cmdClient;
	auto player = playerhelpers->GetGamePlayer(client);

	printf("1\n");

	if(client == 0 || player == NULL) RETURN_META(MRES_IGNORED);
	if(!player->IsConnected()) RETURN_META(MRES_IGNORED);

	printf("2\n");

	const char *argsTmp = command.ArgS();
	char *args = (char*) alloca(strlen(argsTmp) + 1);
	strcpy(args, argsTmp);

	// Remove quotes
	if(args[0] == '"' && strlen(args) > 2){
		args = &args[1];
		int len = strlen(args);
		if(args[len - 1] == '"') args[len - 1] = '\0';
	}

	printf("cmd: %s\n", args);

	if(args == NULL) RETURN_META(MRES_IGNORED);

	printf("3\n");

	bool isTrigger = false;
	bool isSilent = false;

	if(args[0] == '-'){
		isTrigger = true;
	}else if(args[0] == '/'){
		isTrigger = true;
		isSilent = true;
	}

	// Args: ["say" "-command" "arg1" "arg2"]

	if(isTrigger){
		printf("4\n");

		args = &args[1]; // Skip the - or /

		int NUM_ARGS = 16;
		char **argArray = malloc();
		int numArgs = 1;
		int curArgPos = 0;

		argArray[0][0] = '\0';
		int len = strlen(args);
		bool inQuotes = false;
		bool quotesType = -1;
		for(int i = 0; i < len; ++i){
			if(quotesType == 0 && args[i] == '"'){
				inQuotes = false;
			}else if(quotesType == 1 && args[i] == '\''){
				inQuotes = false;
			}else if(args[i] == '"'){
				inQuotes = true;
			}else if(args[i] == '\''){
				inQuotes = true;
			}else if(args[i] == ' ' && !inQuotes){
				argArray[numArgs - 1][curArgPos] = '\0';
				++numArgs;
				curArgPos = 0;
			}else{
				argArray[numArgs - 1][curArgPos++] = args[i];
			}
		}

		argArray[numArgs - 1][curArgPos] = '\0';

		for(int i = 0; i < numArgs; ++i){
			printf("%d = %s\n", i, argArray[i]);
		}

		if(g_MConsole->InvokePluginsCommand(numArgs, (const char **) argArray, args) && isSilent){
			RETURN_META(MRES_SUPERCEDE);
		}
	}
}

void MConsole::OnSayCommand_Post(const CCommand &command){

}

bool MConsole::InvokePluginsCommand(const CCommand &command){
	int len = command.ArgC();
	const char **argArray = (const char **) alloca(sizeof(char*) * len);
	for(int i=0; i<len; ++i){
		argArray[i] = command.Arg(i);
	}

	return InvokePluginsCommand(command.ArgC(), argArray, command.ArgS());
}

bool MConsole::InvokePluginsCommand(int argc, const char **argArray, const char *argString){
	int client = g_MConsole->cmdClient;

	if(client != 0){
		auto p = playerhelpers->GetGamePlayer(client);
		if(!p || !p->IsConnected()){
			return false;
		}
	}

	const char *cmdName = argArray[0];

	bool foundAHook = false;
	bool isDedicated = engine->IsDedicatedServer();

	std::string stdCmdName(cmdName);
	std::transform(stdCmdName.begin(), stdCmdName.end(), stdCmdName.begin(), ::tolower);

	SMJS_Client *clientWrapper = clients[client];

	for(auto it = g_MConsole->pluginCmds2.begin(); it != g_MConsole->pluginCmds2.end(); ++it){
		if(stdCmdName != it->first) continue;

		for(auto it2 = it->second->hooks.begin(); it2 != it->second->hooks.end(); ++it2){
			auto h = (*it2);
			if(client != 0 && h->serverOnly && isDedicated) continue;

			foundAHook = true;
			HandleScope handle_scope(h->pl->GetIsolate());
			Context::Scope context_scope(h->pl->GetContext());

			auto &func = (*it2)->func;

			auto arr = v8::Array::New(argc);
			for(int i=1;i<argc;++i){
				arr->Set(i - 1, v8::String::New(argArray[i + 1]));
			}

			arr->Set(v8::String::New("string"), v8::String::New(argString));

			v8::Handle<v8::Value> args[2];
			args[0] = clientWrapper->GetWrapper(h->pl);
			args[1] = arr;

			func->Call(h->pl->GetContext()->Global(), 2, args);
		}
	}

	return foundAHook;
}

void MConsole::CommandCallback(const CCommand &command){
	if(g_MConsole->InvokePluginsCommand(command)){
		RETURN_META(MRES_SUPERCEDE);
	}
}

void MConsole::AddCommand(SMJS_Plugin *pl, const char *cmdName, v8::Handle<v8::Function> func, bool serverOnly){
	std::string stdCmdName(cmdName);
	std::transform(stdCmdName.begin(), stdCmdName.end(), stdCmdName.begin(), ::tolower);

	CommandHook *wrapper = new CommandHook;
	wrapper->pl = pl;
	wrapper->func = v8::Persistent<v8::Function>::New(func);
	wrapper->serverOnly = serverOnly;

	auto infoIt = g_MConsole->pluginCmds2.find(stdCmdName);
	if(infoIt == g_MConsole->pluginCmds2.end()){
		CommandHookInfo *info = new CommandHookInfo();

		g_MConsole->pluginCmds2.insert(std::make_pair(stdCmdName, info));
		infoIt = g_MConsole->pluginCmds2.find(stdCmdName);

		ConCommand *conCmd = icvar->FindCommand(cmdName);
		if(!conCmd){
			conCmd = new ConCommand(cmdName, MConsole::CommandCallback);
			g_SMAPI->RegisterConCommandBase(g_PLAPI, conCmd);
			info->isSMJS = true;
		}else{
			SH_ADD_HOOK(ConCommand, Dispatch, conCmd, SH_STATIC(MConsole::CommandCallback), false);
			info->isSMJS = false;
		}
	}

	

	infoIt->second->hooks.push_back(wrapper);
}

FUNCTION_M(MConsole::addServerCommand)
	GET_INTERNAL(MConsole*, self);
	ARG_COUNT(2);
	PSTR(cmdName);
	PFUN(callback);

	AddCommand(GetPluginRunning(), *cmdName, callback, true);
	
	return v8::Undefined();
END

FUNCTION_M(MConsole::addClientCommand)
	GET_INTERNAL(MConsole*, self);
	ARG_COUNT(2);
	PSTR(cmdName);
	PFUN(callback);

	AddCommand(GetPluginRunning(), *cmdName, callback, false);
	
	return v8::Undefined();
END