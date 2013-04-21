#include "SMJS_Interfaces.h"
#include "extension.h"

IServerGameClients *serverClients;
ICvar *icvar;
ISmmPluginManager *g_pMMPlugins;
CGlobalVars *gpGlobals;
IGameEventManager2 *gameevents;
SourceHook::CallClass<IVEngineServer> *enginePatch;
SourceHook::CallClass<IServerGameDLL> *gamedllPatch;
IUniformRandomStream *engrandom;
IPlayerInfoManager *playerinfo;
IBaseFileSystem *basefilesystem;
IEngineSound *enginesound;
// IServerPluginHelpers *serverpluginhelpers;
IServerPluginCallbacks *vsp_interface;
IServerTools *serverTools;
SourceMod::ISDKTools *sdkTools;
SourceMod::IBinTools *binTools;
IGameConfig *sdkToolsConf = NULL;

bool SMJS_InitInterfaces(ISmmAPI *ismm, char *error, size_t maxlen, bool late){
	GET_V_IFACE_CURRENT(GetServerFactory, serverClients, IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
	GET_V_IFACE_CURRENT(GetEngineFactory, engrandom, IUniformRandomStream, VENGINE_SERVER_RANDOM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetFileSystemFactory, basefilesystem, IBaseFileSystem, BASEFILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, enginesound, IEngineSound, IENGINESOUND_SERVER_INTERFACE_VERSION);
	//GET_V_IFACE_CURRENT(GetEngineFactory, serverpluginhelpers, IServerPluginHelpers, INTERFACEVERSION_ISERVERPLUGINHELPERS);
	GET_V_IFACE_CURRENT(GetServerFactory, serverTools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);

	/* :TODO: Make this optional and... make it find earlier versions [?] */
	GET_V_IFACE_CURRENT(GetServerFactory, playerinfo, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);

	gpGlobals = ismm->GetCGlobals();

	if ((g_pMMPlugins = (ISmmPluginManager *)g_SMAPI->MetaFactory(MMIFACE_PLMANAGER, NULL, NULL)) == NULL){
		if (error){
			snprintf(error, maxlen, "Unable to find interface %s", MMIFACE_PLMANAGER);
		}
		return false;
	}

	

	return true;
}

bool SMJS_LoadConfs(char *error, size_t maxlength, bool late){
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &sdkToolsConf, error, maxlength)){
		return false;
	}
}

void SMJS_InitLateInterfaces(){
	SM_GET_LATE_IFACE(SDKTOOLS, sdkTools);
	SM_GET_LATE_IFACE(BINTOOLS, binTools);
}