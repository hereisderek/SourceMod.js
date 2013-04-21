/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include <conio.h>
#include "extension.h"
#include "dirent.h"

#include "SMJS_ClientWrapper.h"
#include "modules/MPlugin.h"
#include "modules/MConsole.h"
#include "modules/MServer.h"
#include "modules/MClient.h"
#include "modules/MGame.h"
#include "modules/MSocket.h"
#include "modules/MEntities.h"
#include "modules/MKeyValue.h"
#include "modules/MDota.h"
#include "metamod.h"
#include "SMJS_Interfaces.h"

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

SMJS g_SMJS;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_SMJS);

bool FindAndRunPlugins();
bool FindAndRunAutoloadPlugins();
bool LoadTrustedList();

void CmdJS(const CCommand &command);
void CmdLoadPlugin(const CCommand &command);
void CmdUnloadPlugin(const CCommand &command);
void CmdReloadPlugin(const CCommand &command);

void RegCommand(const char *cmdName, void(*func)(const CCommand&));

ConCommand jsCmd("js", CmdJS, "SourceMod.js");

std::vector<std::string> trustedPlugins;

bool SMJS::SDK_OnLoad(char *error, size_t maxlength, bool late){
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	sharesys->AddDependency(myself, "bintools.ext", true, true);

	if(!SMJS_LoadConfs(error, maxlength, late)) return false;

	v8::V8::Initialize();
	SMJS_Init();

	icvar->RegisterConCommand(&jsCmd);
	
	RegCommand("js_load", CmdLoadPlugin);
	RegCommand("js_unload", CmdUnloadPlugin);
	RegCommand("js_reload", CmdReloadPlugin);

	SMJS_AddModule(new MPlugin());
	SMJS_AddModule(new MConsole());
	SMJS_AddModule(new MServer());
	SMJS_AddModule(new MEntities());
	SMJS_AddModule(new MClient());
	SMJS_AddModule(new MGame());
	SMJS_AddModule(new MSocket());
	SMJS_AddModule(new MKeyValue());
	SMJS_AddModule(new MDota());

	return true;
}

void SMJS::SDK_OnUnload(){

}

void SMJS::SDK_OnAllLoaded(){
	SMJS_InitLateInterfaces();
	LoadTrustedList();
	FindAndRunAutoloadPlugins();
}

bool SMJS::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late){
	return SMJS_InitInterfaces(ismm, error, maxlength, late);
}

bool LoadTrustedList(){
	char trustedListPath[512];
	smutils->BuildPath(Path_SM, trustedListPath, sizeof(trustedListPath), "plugins.js/trusted.txt");

	FILE *trustedListFile = fopen(trustedListPath, "r");
	if(trustedListFile == NULL) return false;

	char line[256];
	bool allSuccess = true;
	while(fgets(line, sizeof(line), trustedListFile) != NULL){
		int len = strlen(line);
		while(len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')){
			line[len - 1] = '\0'; // Remove newline
			--len;
		}

		trustedPlugins.push_back(std::string(line));
	}

	fclose(trustedListFile);
	return true;
}

bool FindAndRunPlugins(){
	char pluginsPath[512];
	smutils->BuildPath(Path_SM, pluginsPath, sizeof(pluginsPath), "plugins.js");

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(pluginsPath)) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			if(strcmp(ent->d_name, "") == 0) continue;
			if(strcmp(ent->d_name, ".") == 0) continue;
			if(strcmp(ent->d_name, "..") == 0) continue;
			if(strcmp(ent->d_name, "disabled") == 0) continue;

			LoadPlugin(ent->d_name);
		}
		closedir(dir);
	} else {
		smutils->LogError(myself, "Couldn't open \"plugins.js\" folder");
		return false;
	}

	return true;
}

bool FindAndRunAutoloadPlugins(){
	char autoloadPath[512];
	smutils->BuildPath(Path_SM, autoloadPath, sizeof(autoloadPath), "plugins.js/autoload.txt");

	FILE *autoloadFile = fopen(autoloadPath, "r");
	if(autoloadFile == NULL) return false;

	char line[256];
	bool allSuccess = true;
	while(fgets(line, sizeof(line), autoloadFile) != NULL){
		int len = strlen(line);
		while(len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')){
			line[len - 1] = '\0'; // Remove newline
			--len;
		}

		if(LoadPlugin(line) == NULL) allSuccess = false;
	}

	fclose(autoloadFile);

	return allSuccess;
}

SMJS_Plugin *LoadPlugin(const char *dir){
	auto plugin = SMJS_Plugin::GetPluginByDir(dir);
	if(plugin != NULL) return plugin;

	plugin = new SMJS_Plugin(dir);

	char path[512];
	smutils->BuildPath(Path_SM, path, sizeof(path), "plugins.js/%s", dir);


	plugin->SetDir(dir);
	plugin->SetPath(path);
	plugin->CheckApi();

	for(auto it = trustedPlugins.begin(); it != trustedPlugins.end(); ++it){
		if(strcmp(dir, it->c_str()) == 0){
			plugin->isSandboxed = false;
			break;
		}
	}

	plugin->LoadModules();

	if(!plugin->LoadFile("Main.js", true)){
		delete plugin;
		return NULL;
	}

	// Late loading
	if(smutils->IsMapRunning()){
		HandleScope handle_scope(plugin->GetIsolate());
		Context::Scope context_scope(plugin->GetContext());

		auto hooks = plugin->GetHooks("OnMapStart");
		for(auto it = hooks->begin(); it != hooks->end(); ++it){
			(*it)->Call(plugin->GetContext()->Global(), 0, NULL);
		}

		for(int i = 0; i < sizeof(clients) / sizeof(clients[0]); ++i){
			if(clients[i] == NULL) continue;
			v8::Handle<v8::Value> arg = clients[i]->GetWrapper(plugin);

			hooks = plugin->GetHooks("OnClientConnected");
			for(auto it = hooks->begin(); it != hooks->end(); ++it){
				(*it)->Call(plugin->GetContext()->Global(), 1, &arg);
			}

			if(clients[i]->inGame){
				hooks = plugin->GetHooks("OnClientPutInGame");
				for(auto it = hooks->begin(); it != hooks->end(); ++it){
					(*it)->Call(plugin->GetContext()->Global(), 1, &arg);
				}
			}
		}
	}

	return plugin;
}

void CmdJS(const CCommand &command){

}

void CmdLoadPlugin(const CCommand &command){
	if(command.ArgC() != 2){
		META_CONPRINT("Usage: js_load [plugin_dir]\n");
		return;
	}

	auto pl = SMJS_Plugin::GetPluginByDir(command.Arg(1));
	if(pl != NULL){
		META_CONPRINTF("Plugin \"%s\" is already loaded!\n", command.Arg(1));
		return;
	}

	pl = LoadPlugin(command.Arg(1));
	if(pl == NULL){
		META_CONPRINTF("Plugin \"%s\" failed to load!\n", command.Arg(1));
	}else{
		META_CONPRINTF("Plugin \"%s\" loaded successfully!\n", command.Arg(1));
	}
}

void CmdUnloadPlugin(const CCommand &command){
	if(command.ArgC() != 2){
		META_CONPRINT("Usage: js_unload [plugin_dir]\n");
		return;
	}

	auto pl = SMJS_Plugin::GetPluginByDir(command.Arg(1));
	if(pl == NULL){
		META_CONPRINTF("Plugin \"%s\" is not loaded!\n", command.Arg(1));
		return;
	}

	delete pl;
	META_CONPRINTF("Plugin \"%s\" unloaded successfully!\n", command.Arg(1));
}

void CmdReloadPlugin(const CCommand &command){
	if(command.ArgC() != 2){
		META_CONPRINT("Usage: js_reload [plugin_dir]\n");
		return;
	}

	CmdUnloadPlugin(command);
	CmdLoadPlugin(command);
	
}

void RegCommand(const char *cmdName, void(*func)(const CCommand&)){
	ConCommand *conCmd = icvar->FindCommand(cmdName);
	if(!conCmd){
		conCmd = new ConCommand(cmdName, func);
		icvar->RegisterConCommand(conCmd);
		/*if(!g_SMAPI->RegisterConCommandBase(g_PLAPI, conCmd)){
			smutils->LogError(myself, "Couldn't register command %s", cmdName);
		}*/
	}else{
		smutils->LogError(myself, "Couldn't register command %s", cmdName);
	}
}