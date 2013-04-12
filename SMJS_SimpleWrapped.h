#ifndef _INCLUDE_SMJS_SIMPLEWRAPPED_H_
#define _INCLUDE_SMJS_SIMPLEWRAPPED_H_

#include "k7v8macros.h"

#include "SMJS.h"
#include "SMJS_Base.h"

#include <unordered_map>

class SMJS_SimpleWrapped : public SMJS_Base, public IPluginDestroyedHandler {
private:
	SMJS_SimpleWrapped();

protected:
	v8::Persistent<v8::Value> wrapper;
	int refCount;
	SMJS_Plugin *plugin;

public:

	SMJS_SimpleWrapped(SMJS_Plugin *pl);

	virtual void OnWrapperAttached(){};

	virtual v8::Persistent<v8::Value> GetWrapper(){return v8::Persistent<v8::Value>::New(v8::Undefined());};

	void Destroy();

	virtual void OnPluginDestroyed(SMJS_Plugin *plugin){
		Destroy();
		this->plugin = NULL;
	}

	static v8::Persistent<v8::FunctionTemplate> temp;

	inline SMJS_Plugin* GetPlugin(){return plugin;}

	static void SetupTemplate(v8::Persistent<v8::FunctionTemplate> temp, v8::Persistent<v8::Template> proto) {}

	static v8::Persistent<v8::FunctionTemplate> GetTemplate(){
		if(!temp.IsEmpty()) return temp;
		
		auto nTemp = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New());;
		
		auto inst = nTemp->InstanceTemplate();
		inst->SetInternalFieldCount(1);
		
		auto proto = v8::Persistent<v8::ObjectTemplate>::New(nTemp->PrototypeTemplate());
		
		SetupTemplate(nTemp, proto);
		
		temp = nTemp;
		return temp;
	}
};

#define SIMPLE_WRAPPED_CLS(cls, super) \
	virtual v8::Persistent<v8::Value> GetWrapper(); \
	static v8::Persistent<FunctionTemplate> temp; \
	static v8::Persistent<v8::FunctionTemplate> GetTemplate(); \
	static v8::Persistent<v8::Value> CreateWrapper(cls* obj); \
	static void SetupTemplate(v8::Persistent<FunctionTemplate> temp, v8::Persistent<v8::ObjectTemplate> proto) \

#define SIMPLE_WRAPPED_CLS_CPP(cls, super) \
	v8::Persistent<FunctionTemplate> cls::temp; \
	v8::Persistent<v8::FunctionTemplate> cls::GetTemplate(){ \
		if(!temp.IsEmpty()) return temp; \
		 \
		auto nTemp = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New());; \
		 \
		auto inst = nTemp->InstanceTemplate(); \
		inst->SetInternalFieldCount(1); \
		 \
		auto proto = v8::Persistent<v8::ObjectTemplate>::New(nTemp->PrototypeTemplate()); \
		 \
		nTemp->Inherit(super::GetTemplate()); \
		SetupTemplate(nTemp, proto); \
		 \
		temp = nTemp; \
		return temp; \
	} \
	v8::Persistent<v8::Value> cls::CreateWrapper(cls* obj) { \
		auto v = GetTemplate()->GetFunction()->NewInstance(); \
		v->SetInternalField(0, v8::External::New((void*)obj)); \
		return v8::Persistent<v8::Value>::New(v); \
	} \
	v8::Persistent<v8::Value> cls::GetWrapper() { \
		if(!wrapper.IsEmpty()) return wrapper; \
		wrapper = CreateWrapper(this); \
		++refCount;	\
		OnWrapperAttached(); \
		return wrapper; \
	}

#endif