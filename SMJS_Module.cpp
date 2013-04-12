#include "SMJS_Module.h"
#include "SMJS_Plugin.h"
#include "smsdk_ext.h"
#include "metamod.h"

WRAPPED_CLS_CPP(SMJS_Module, SMJS_BaseWrapped)


Handle<Value> SMJS_Module::CallGlobalFunctionWithWrapped(const char* name, SMJS_BaseWrapped *wrapped, SMJS_Plugin **retPlugin, bool earlyReturn){
	v8::Persistent<v8::Value> ret;

	for(auto it = wrappers.begin(); it != wrappers.end(); ++it){
		auto plugin = GetPlugin(it->first);
		if(plugin == NULL) continue;

		HandleScope handle_scope(plugin->isolate);
		Context::Scope context_scope(plugin->context);

		auto global = plugin->context->Global();

		auto hooks = plugin->GetHooks(name);

		for(auto it = hooks->begin(); it != hooks->end(); ++it){
			auto value = (*it);
			
			if(value->IsUndefined() || value->IsNull()) continue;
			if(!value->IsFunction()) continue;

			auto func = v8::Handle<v8::Function>::Cast(value);
			auto funcResult = func->Call(global, 1, &wrapped->GetWrapper(plugin));

			if(!funcResult.IsEmpty() && !funcResult->IsUndefined() && !funcResult->IsNull()){
				if(retPlugin != NULL) *retPlugin = plugin;
				if(earlyReturn){
					return v8::Persistent<v8::Value>::New(funcResult);
				}else{
					ret = v8::Persistent<v8::Value>::New(funcResult);
				}
			}
		}
	}

	return v8::Persistent<v8::Value>::New(ret);
}

void SMJS_Module::CallGlobalFunction(const char* name){
	for(auto it = wrappers.begin(); it != wrappers.end(); ++it){
		auto plugin = GetPlugin(it->first);
		if(plugin == NULL) continue;

		HandleScope handle_scope(plugin->isolate);
		Context::Scope context_scope(plugin->context);

		auto global = plugin->context->Global();
		auto hooks = plugin->GetHooks(name);

		for(auto it = hooks->begin(); it != hooks->end(); ++it){
			auto value = (*it);
			if(value->IsUndefined() || value->IsNull()) continue;
			if(!value->IsFunction()) continue;

			v8::Handle<v8::Function>::Cast(value)->Call(global, 0, NULL);
		}
	}
}