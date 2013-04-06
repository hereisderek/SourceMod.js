#ifndef _INCLUDE_SMJS_MODULE_H_
#define _INCLUDE_SMJS_MODULE_H_

#include <vector>
#include "SMJS.h"
#include "SMJS_BaseWrapped.h"
#include "SMJS_Interfaces.h"

#include "k7v8macros.h"

class SMJS_Plugin;
class SMJS_Module;

extern std::vector<SMJS_Module*> SMJS_Modules;

class SMJS_Module : public SMJS_BaseWrapped {
public:
	std::string identifier;
	
	bool sandboxed; // If this is true (default) the module will be loaded on sandboxed plugins

	SMJS_Module(){
		sandboxed = true;
	}

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){};

	WRAPPED_CLS(SMJS_Module, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("Module"));
	}

	v8::Handle<Value> CallGlobalFunctionWithWrapped(const char* name, SMJS_BaseWrapped *wrapped, SMJS_Plugin **retPlugin = NULL, bool earlyReturn = false);
	void CallGlobalFunction(const char* name);

};

#endif