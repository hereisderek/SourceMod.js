#include "MGame.h"
#include "SMJS_Plugin.h"
#include "sh_vector.h"
#include "server_class.h"
#include "MEntities.h"
#include "MSocket.h"
#include "SMJS_Interfaces.h"

SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t *, int, int);
SH_DECL_HOOK1_void(IServerGameDLL, Think, SH_NOATTRIB, 0, bool);

WRAPPED_CLS_CPP(MGame, SMJS_Module)

struct TeamInfo{
	const char *ClassName;
	CBaseEntity *pEnt;
};

SourceHook::CVector<TeamInfo> g_Teams;
MGame *self;

MGame::MGame(){
	self = this;
	identifier = "game";

	SH_ADD_HOOK(IServerGameDLL, ServerActivate, gamedll, SH_MEMBER(this, &MGame::OnPreServerActivate), false);
	SH_ADD_HOOK(IServerGameDLL, ServerActivate, gamedll, SH_MEMBER(this, &MGame::OnServerActivate), true);
	SH_ADD_HOOK(IServerGameDLL, Think, gamedll, SH_MEMBER(this, &MGame::OnThink), true);
	smutils->AddGameFrameHook(MGame::OnGameFrame);
}

void MGame::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	
}

void MGame::OnPreServerActivate(edict_t *pEdictList, int edictCount, int clientMax){
	ClearEntityWrappers();
}

void MGame::OnServerActivate(edict_t *pEdictList, int edictCount, int clientMax){
	InitTeamNatives();

	self->CallGlobalFunction("OnMapStart");
}

void MGame::OnGameFrame(bool simulating){
	self->CallGlobalFunction("OnGameFrame");
}

void MGame::OnThink(bool finalTick){
	MSocket::OnThink(finalTick);
}

bool FindNestedDataTable(SendTable *pTable, const char *name){
	if (strcmp(pTable->GetName(), name) == 0){
		return true;
	}

	int props = pTable->GetNumProps();
	SendProp *prop;

	for (int i=0; i<props; i++){
		prop = pTable->GetProp(i);
		if (prop->GetDataTable()){
			if (FindNestedDataTable(prop->GetDataTable(), name)){
				return true;
			}
		}
	}

	return false;
}

void MGame::InitTeamNatives(){
	g_Teams.clear();
	g_Teams.resize(1);

	int edictCount = gpGlobals->maxEntities;

	for (int i=0; i<edictCount; i++){
		edict_t *pEdict = gamehelpers->EdictOfIndex(i);
		if (!pEdict || pEdict->IsFree()){
			continue;
		}
		if (!pEdict->GetNetworkable()){
			continue;
		}

		ServerClass *pClass = pEdict->GetNetworkable()->GetServerClass();

		if(FindNestedDataTable(pClass->m_pTable, "DT_Team")){
			SendProp *pTeamNumProp = gamehelpers->FindInSendTable(pClass->GetName(), "m_iTeamNum");

			if (pTeamNumProp != NULL){
				int offset = pTeamNumProp->GetOffset();
				CBaseEntity *pEnt = pEdict->GetUnknown()->GetBaseEntity();
				int TeamIndex = *(int *)((unsigned char *)pEnt + offset);

				if (TeamIndex >= (int)g_Teams.size()){
					g_Teams.resize(TeamIndex+1);
				}
				g_Teams[TeamIndex].ClassName = pClass->GetName();
				g_Teams[TeamIndex].pEnt = pEnt;
			}
		}
	}
}

v8::Handle<v8::Value> MGame::NativeFindEntityByClassname(int startIndex, char *searchname){
	CBaseEntity *pEntity;

	if (startIndex == -1){
		pEntity = (CBaseEntity *)serverTools->FirstEntity();
	}else{
		pEntity = gamehelpers->ReferenceToEntity(startIndex);
		if (!pEntity){
			THROW_VERB("Entity %d (%d) is invalid", gamehelpers->ReferenceToIndex(startIndex), startIndex);
		}

		pEntity = (CBaseEntity *)serverTools->NextEntity(pEntity);
	}

	if (!pEntity) return v8::Null();

	const char *classname;
	int lastletterpos = strlen(searchname) - 1;

	static int offset = -1;
	if (offset == -1){
		offset = gamehelpers->FindInDataMap(gamehelpers->GetDataMap(pEntity), "m_iClassname")->fieldOffset;
	}

	string_t s;


	while (pEntity){
		if ((s = *(string_t *)((uint8_t *)pEntity + offset)) == NULL_STRING){
			pEntity = (CBaseEntity *)serverTools->NextEntity(pEntity);
			continue;
		}

		classname = STRING(s);

		
		if (searchname[lastletterpos] == '*'){
			if (strncasecmp(searchname, classname, lastletterpos) == 0)
			{
				return GetEntityWrapper(gamehelpers->EntityToBCompatRef(pEntity))->GetWrapper(GetPluginRunning());
			}
		}else if (strcasecmp(searchname, classname) == 0){
			return GetEntityWrapper(gamehelpers->EntityToBCompatRef(pEntity))->GetWrapper(GetPluginRunning());
		}

		pEntity = (CBaseEntity *)serverTools->NextEntity(pEntity);
	}

	return v8::Null();
}

v8::Handle<v8::Value> MGame::FindEntityByClassname(int startIndex, char *searchname){
	return NativeFindEntityByClassname(startIndex, searchname);
}

FUNCTION_M(MGame::hook)
	ARG_COUNT(2);
	PSTR(str);
	PFUN(callback);
	GetPluginRunning()->GetHooks(*str)->push_back(v8::Persistent<v8::Function>::New(callback));
	RETURN_UNDEF;
END

FUNCTION_M(MGame::getTeamClientCount)
	ARG_COUNT(1);
	PINT(teamindex);

	if (teamindex >= (int)g_Teams.size() || !g_Teams[teamindex].ClassName){
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "Team index %d is invalid", teamindex);
		return v8::ThrowException(v8::Exception::Error(v8::String::New(buffer)));
	}

	SendProp *pProp = gamehelpers->FindInSendTable(g_Teams[teamindex].ClassName, "\"player_array\"");
	ArrayLengthSendProxyFn fn = pProp->GetArrayLengthProxy();

	return v8::Int32::New(fn(g_Teams[teamindex].pEnt, 0));
END

FUNCTION_M(MGame::precacheModel)
	ARG_BETWEEN(1, 2);
	PSTR(str);
	bool preload = false;
	
	if(args.Length() >= 2){
		preload = args[1]->BooleanValue();
	}


	return v8::Integer::New(engine->PrecacheModel(*str, preload));
END

FUNCTION_M(MGame::findEntityByClassname)
	ARG_COUNT(2);
	PINT(startIndex);
	PSTR(searchname);
	return FindEntityByClassname(startIndex, *searchname);
END


FUNCTION_M(MGame::getGameRules)
	if(sdkTools == NULL) return v8::Null();
	auto gameRules = (CBaseEntity*) sdkTools->GetGameRules();
	if(gameRules == NULL) return v8::Null();
	return GetEntityWrapper(gameRules)->GetWrapper(GetPluginRunning());
END