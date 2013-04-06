#pragma once
#include "SMJS.h"
#include "SMJS_Module.h"
#include "ActiveSocket.h"

class CommandCallbackWrapper;
class JSSocket;

class MSocket : public SMJS_Module {
public:
	MSocket();

	std::vector<JSSocket*> sockets;

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void OnPluginDestroyed(SMJS_Plugin *plugin);

	FUNCTION_DECL(create);
	FUNCTION_DECL(createBuffer);

	WRAPPED_CLS(MSocket, SMJS_Module) {
		temp->SetClassName(v8::String::New("SocketModule"));
		
		WRAPPED_FUNC(create);
		WRAPPED_FUNC(createBuffer);
	}

	static void OnThink(bool finalTick);
	static void OnGameFrame(bool simulating);
};

extern MSocket *g_MSocket;