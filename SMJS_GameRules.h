#ifndef _INCLUDE_SMJS_GAMERULES_H_
#define _INCLUDE_SMJS_GAMERULES_H_

#include "SMJS.h"
#include "SMJS_BaseWrapped.h"
#include "SMJS_GameRulesProps.h"

class SMJS_GameRules : public SMJS_BaseWrapped {
public:
	SMJS_GameRulesProps rulesProps;

	SMJS_GameRules(){
		
	}

	WRAPPED_CLS(SMJS_GameRules, SMJS_BaseWrapped){
		proto->Set("props", v8::Null());
	}

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
		auto obj = wrapper->ToObject();

		obj->Set(v8::String::New("props"), rulesProps.GetWrapper(plugin));
	}
};

#endif