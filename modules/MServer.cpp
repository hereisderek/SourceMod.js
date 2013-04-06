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
	ARG_COUNT(0);
	engine->ServerExecute();
	return JS_undefined;
END

class ClientArray : public SMJS_BaseWrapped {
public:

	ClientArray(){
		
	}

	WRAPPED_CLS(ClientArray, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("ClientArray"));
		proto->Set("length", v8::Int32::New(65));
		temp->InstanceTemplate()->SetIndexedPropertyHandler(GetClient);
	}

	static v8::Handle<v8::Value> GetClient(uint32_t index, const AccessorInfo& info){
		if(index < 0 || index >= 65) return v8::Undefined();
		if(clients[index] == NULL) return v8::Undefined();
		return clients[index]->GetWrapper(GetPluginRunning());
	}
};

WRAPPED_CLS_CPP(ClientArray, SMJS_BaseWrapped);

ClientArray clientArray;

void MServer::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	wrapper->ToObject()->Set(v8::String::New("clients"), clientArray.GetWrapper(plugin));
}