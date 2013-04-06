#pragma once
#include "SMJS_Module.h"
#include <map>

class MKeyValue : public SMJS_Module {
public:
	MKeyValue();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void OnPluginDestroyed(SMJS_Plugin *plugin){};


	FUNCTION_DECL(parseKVFile);

	WRAPPED_CLS(MKeyValue, SMJS_Module) {
		temp->SetClassName(v8::String::New("KeyValueModule"));

		WRAPPED_FUNC(parseKVFile);
		
	}

	
};
