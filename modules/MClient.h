#pragma once
#include "SMJS_Module.h"
#include "SMJS_ClientWrapper.h"
#include <vector>
#include <map>

extern SMJS_Client *clients[65];



class MClient : public SMJS_Module {
public:
	MClient();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	WRAPPED_CLS(MClient, SMJS_Module) {
		temp->SetClassName(v8::String::New("ClientModule"));
		
	}

	static void RunAuthChecks();
	static void OnGameFrame(bool simulating);
};
