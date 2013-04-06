#pragma once

#include "SMJS.h"
#include "SMJS_Module.h"
#include <vector>
#include <map>

class MPlugin : public SMJS_Module {
public:
	MPlugin();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void Init();

	FUNCTION_DECL(isSandboxed);
	FUNCTION_DECL(loadPlugin);


	WRAPPED_CLS(MPlugin, SMJS_Module) {
		temp->SetClassName(v8::String::New("PluginModule"));

		WRAPPED_FUNC(isSandboxed);
		WRAPPED_FUNC(loadPlugin);
	}

	
};
