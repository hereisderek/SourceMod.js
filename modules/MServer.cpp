#include "modules/MServer.h"
#include "SMJS_Plugin.h"
#include "MClient.h"

WRAPPED_CLS_CPP(MServer, SMJS_Module);

FUNCTION_M(MServer::print)
	ARG_COUNT(1);
	PSTR(str);

	int len = str.length();
	char *buffer = new char[len + 2];
	memcpy(buffer, *str, len);
	buffer[len] = '\n';
	buffer[len + 1] = '\0';

	META_CONPRINT(buffer);
	return JS_undefined;
END

FUNCTION_M(MServer::command)
	if(GetPluginRunning()->isSandboxed) THROW("Function not available in sandboxed plugins");

	ARG_COUNT(1);
	PSTR(str);

	int len = str.length();
	char *buffer = new char[len + 2];
	memcpy(buffer, *str, len);
	buffer[len] = '\n';
	buffer[len + 1] = '\0';

	engine->ServerCommand(buffer);
	return JS_undefined;
END

FUNCTION_M(MServer::execute)
	if(GetPluginRunning()->isSandboxed) THROW("Function not available in sandboxed plugins");
	
	ARG_COUNT(0);
	engine->ServerExecute();
	return JS_undefined;
END

FUNCTION_M(MServer::getPort)
	ARG_COUNT(0);
	auto cvar = icvar->FindVar("hostport");
	if(cvar == NULL) return JS_undefined;
	return v8::Int32::New(cvar->GetInt());
END

FUNCTION_M(MServer::getIP)
	ARG_COUNT(0);
	auto cvar = icvar->FindVar("ip");
	if(cvar == NULL) return JS_undefined;
	return v8::String::New(cvar->GetString());
END

FUNCTION_M(MServer::userIdToClient)
	PINT(userid);
	int client = playerhelpers->GetClientOfUserId(userid);
	if(client <= 0) return v8::Null();
	if(clients[client] == NULL) return v8::Null();
	return clients[client]->GetWrapper(GetPluginRunning());
END

FUNCTION_M(MServer::changeMap)
	PSTR(mapName);
	PSTR(reason);

	if(!gamehelpers->IsMapValid(*mapName)) THROW_VERB("Invalid map \"%s\"", *mapName);

	// Workaround
	char buffer[512];
	snprintf(buffer, sizeof(buffer), "map \"%s\"\n", *mapName);
	engine->ServerCommand(buffer);
	engine->ServerExecute();

	//engine->ChangeLevel(*mapName, *reason);

	RETURN_UNDEF;
END

FUNCTION_M(MServer::getMap)
	return v8::String::New(gamehelpers->GetCurrentMap());
END

FUNCTION_M(MServer::isMapValid)
	PSTR(mapName);
	return v8::Boolean::New(gamehelpers->IsMapValid(*mapName));
END

FUNCTION_M(MServer::log)
	PSTR(str);
	smutils->LogMessage(myself, "%s", str);
	RETURN_UNDEF;
END

class ClientArray : public SMJS_BaseWrapped {
public:

	ClientArray(){
		
	}

	WRAPPED_CLS(ClientArray, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("ClientArray"));
		proto->Set("length", v8::Int32::New(MAXCLIENTS));
		temp->InstanceTemplate()->SetIndexedPropertyHandler(GetClient);
	}

	static v8::Handle<v8::Value> GetClient(uint32_t index, const AccessorInfo& info){
		if(index >= MAXCLIENTS) return v8::Undefined();
		if(index == 0) return v8::Null();
		if(clients[index] == NULL) return v8::Null();
		return clients[index]->GetWrapper(GetPluginRunning());
	}
};

WRAPPED_CLS_CPP(ClientArray, SMJS_BaseWrapped);

ClientArray clientArray;

void MServer::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	wrapper->ToObject()->Set(v8::String::New("clients"), clientArray.GetWrapper(plugin));
}