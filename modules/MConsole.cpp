#include "modules/MConsole.h"
#include "MClient.h"
#include "SMJS_Plugin.h"
#include "extension.h"
#include "SMJS_ConVar.h"

WRAPPED_CLS_CPP(MConsole, SMJS_Module)


SH_DECL_HOOK2_void(ConCommand, Dispatch, SH_NOATTRIB, false, void *, const CCommand &);

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
		auto &hooks = it->second->hooks;
		for(auto it2 = hooks.begin(); it2 != hooks.end() && hooks.size() != 0; ++it2){
			if((*it2)->pl == plugin){
				auto p = *it2;
				//it2 = hooks.erase(it2);
				it2 = hooks.begin();
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

void MConsole::OnSayCommand_Pre(void *pUnknown, const CCommand &command){
	if(strcmp(command.Arg(0), "say") != 0 && strcmp(command.Arg(0), "say_team")){
		return;
	}

	int client = g_MConsole->cmdClient;
	auto player = playerhelpers->GetGamePlayer(client);


	if(client == 0 || player == NULL) RETURN_META(MRES_IGNORED);
	if(!player->IsConnected()) RETURN_META(MRES_IGNORED);


	const char *argsTmp = command.ArgS();
	if(argsTmp == NULL) RETURN_META(MRES_IGNORED);

	char *args = (char*) alloca(strlen(argsTmp) + 1);
	strcpy(args, argsTmp);

	// Remove quotes
	if(args[0] == '"' && strlen(args) > 2){
		args = &args[1];
		int len = strlen(args);
		if(args[len - 1] == '"') args[len - 1] = '\0';
	}

	

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
		args = &args[1]; // Skip the - or /

		int MAX_ARGS = 16;
		char **argArray = new char*[MAX_ARGS];
		int numArgs = 1;
		int curArgPos = 0;

		argArray[0] = new char[256];
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
				quotesType = 0;
			}else if(args[i] == '\''){
				inQuotes = true;
				quotesType = 1;
			}else if(args[i] == ' ' && !inQuotes && numArgs < MAX_ARGS){
				argArray[numArgs - 1][curArgPos] = '\0';
				++numArgs;

				argArray[numArgs - 1] = new char[256];
				argArray[numArgs - 1][curArgPos] = '\0';
				curArgPos = 0;
			}else{
				argArray[numArgs - 1][curArgPos++] = args[i];
			}
		}

		argArray[numArgs - 1][curArgPos] = '\0';

		if(g_MConsole->InvokePluginsCommand(client, curArgPos == 0 ? numArgs - 1 : numArgs, (const char **) argArray, args) && isSilent){
			for(int i = 0; i < numArgs; ++i) delete[] argArray[i];
			delete[] argArray;
			RETURN_META(MRES_SUPERCEDE);
		}

		for(int i = 0; i < numArgs; ++i) delete[] argArray[i];
		delete[] argArray;
	}
}

void MConsole::OnSayCommand_Post(void *pUnknown, const CCommand &command){

}

bool MConsole::InvokePluginsCommand(const CCommand &command){
	int len = command.ArgC();
	const char **argArray = (const char **) alloca(sizeof(char*) * len);
	for(int i=0; i<len; ++i){
		argArray[i] = command.Arg(i);
	}

	return InvokePluginsCommand(g_MConsole->cmdClient, command.ArgC(), argArray, command.ArgS());
}

bool MConsole::InvokePluginsCommand(int client, int argc, const char **argArray, const char *argString){
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
			if(client != 0 && h->serverOnly && isDedicated){
				continue;
			}

			foundAHook = true;
			HandleScope handle_scope(h->pl->GetIsolate());
			Context::Scope context_scope(h->pl->GetContext());

			auto &func = (*it2)->func;

			auto arr = v8::Array::New(argc - 1);
			for(int i=1;i<argc;++i){
				arr->Set(i - 1, v8::String::New(argArray[i]));
			}

			arr->Set(v8::String::New("string"), v8::String::New(argString));

			v8::Handle<v8::Value> args[2];
			if(clientWrapper == NULL){
				args[0] = v8::Null();
			}else{
				args[0] = clientWrapper->GetWrapper(h->pl);
			}
			args[1] = arr;

			func->Call(h->pl->GetContext()->Global(), 2, args);
		}
	}

	return foundAHook;
}

void MConsole::CommandCallback(void *pUnknown, const CCommand &command){
	if(g_MConsole->InvokePluginsCommand(command)){
		RETURN_META(MRES_SUPERCEDE);
	}
}

void MConsole::CommandCallback2(void *pUnknown, const CCommand &command){
	g_MConsole->InvokePluginsCommand(command);
}

char *dupstring(const char *str){
	char *newStr = new char[strlen(str) + 1];
	strcpy(newStr, str);
	return newStr;
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
			conCmd = new ConCommand(dupstring(cmdName), MConsole::CommandCallback2);
			icvar->RegisterConCommand(conCmd);
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
	
	RETURN_UNDEF;
END

FUNCTION_M(MConsole::addClientCommand)
	GET_INTERNAL(MConsole*, self);
	ARG_COUNT(2);
	PSTR(cmdName);
	PFUN(callback);

	AddCommand(GetPluginRunning(), *cmdName, callback, false);
	
	RETURN_UNDEF;
END

FUNCTION_M(MConsole::findConVar)
	ARG_COUNT(1);
	PSTR(cvarName);

	auto plugin = GetPluginRunning();

	if(plugin->isSandboxed){
		if(strcmp(*cvarName, "sv_password") == 0) return v8::Null();
		if(strcmp(*cvarName, "rcon_password") == 0) return v8::Null();
	}

	ConVar *cv = icvar->FindVar(*cvarName);
	if(cv == NULL) return v8::Null();

	if(plugin->isSandboxed){
		int flags = cv->GetFlags();
		if(flags & FCVAR_PROTECTED){
			return v8::Null();
		}
	}

	auto smjsCv = new SMJS_ConVar(plugin, cv);
	auto wrap = smjsCv->GetWrapper();
	smjsCv->Destroy();
	RETURN_SCOPED(wrap);
END