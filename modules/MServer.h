#pragma once
#include "SMJS_Module.h"

class MServer : public SMJS_Module {
public:

	MServer(){
		identifier = "server";
	}

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	FUNCTION_DECL(print);
	FUNCTION_DECL(command);
	FUNCTION_DECL(execute);
	FUNCTION_DECL(getPort);
	FUNCTION_DECL(getIP);

	WRAPPED_CLS(MServer, SMJS_Module) {
		temp->SetClassName(v8::String::New("ServerModule"));

		proto->Set(v8::String::New("isDedicated"), v8::Boolean::New(engine->IsDedicatedServer()), ReadOnly);
		proto->Set(v8::String::New("clients"), v8::Null());

		WRAPPED_FUNC(print);
		WRAPPED_FUNC(command);
		WRAPPED_FUNC(execute);
		WRAPPED_FUNC(getPort);
		WRAPPED_FUNC(getIP);
	}
};
