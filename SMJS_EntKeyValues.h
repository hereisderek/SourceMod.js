#ifndef _INCLUDE_SMJS_ENTKEYVALUES_H_
#define _INCLUDE_SMJS_ENTKEYVALUES_H_

#include "SMJS.h"
#include "smsdk_ext.h"
#include <map>


#include "SMJS_BaseWrapped.h"

class SMJS_Entity;
class SMJS_EntKeyValues : public SMJS_BaseWrapped {
public:
	SMJS_Entity *entWrapper;

	SMJS_EntKeyValues();
	~SMJS_EntKeyValues();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	static v8::Handle<v8::Value> GetKeyValue(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> SetKeyValue(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

	WRAPPED_CLS(SMJS_EntKeyValues, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("EntityKeyValues"));
		temp->InstanceTemplate()->SetNamedPropertyHandler(GetKeyValue, SetKeyValue);
	}

};

#endif
