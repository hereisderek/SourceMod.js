

#ifndef _INCLUDE_SMJS_PLUGIN_I_H_
#define _INCLUDE_SMJS_PLUGIN_I_H_

#include "ISMJS.h"
#include <vector>

/**
	Gets the plugin that's running in the current context
*/

typedef void(*DestroyCallback)(SMJS_Plugin*);

class SMJS_Plugin {
public: // Public variables
	virtual PLUGIN_ID GetId();
	virtual v8::Persistent<v8::Context> GetContext();
	virtual v8::Isolate *GetIsolate();
	virtual void RegisterDestroyCallback(DestroyCallback func) = 0;
	virtual void RegisterDestroyCallback(SMJS_BaseWrapped *ptr) = 0;
	virtual int GetApiVersion(char const *id, int curVersion) = 0;
	virtual std::vector<v8::Persistent<v8::Function>>* GetHooks(char const *type) = 0;
};

inline SMJS_Plugin *GetPluginRunning(){
	return (SMJS_Plugin*) v8::Handle<External>::Cast(v8::Context::GetCurrent()->GetEmbedderData(1))->Value();
}

#endif