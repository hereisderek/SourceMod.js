#include "extension.h"
#include "SMJS_SimpleWrapped.h"
#include "SMJS_Plugin.h"

v8::Persistent<FunctionTemplate> SMJS_SimpleWrapped::temp;

void SimpleDecWrappedRefCount(Isolate* isolate, v8::Persistent<v8::Value> object, void *parameter){
	SMJS_BaseWrapped *wrapped = (SMJS_BaseWrapped*) parameter;
	if(--wrapped->refCount == 0){
		delete wrapped;
	}

	object.Dispose();
    object.Clear();
}

SMJS_SimpleWrapped::SMJS_SimpleWrapped(SMJS_Plugin *pl) : plugin(pl){
	refCount = 0;
	plugin->RegisterDestroyCallback(this);
}

void SMJS_SimpleWrapped::Destroy(){
	if(refCount == 0){
		delete this;
	}else{
		wrapper.MakeWeak(plugin->GetIsolate(), (void*) this, SimpleDecWrappedRefCount);
	}
}