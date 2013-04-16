#include "SMJS_EntKeyValues.h"
#include "SMJS_Entity.h"
#include "SMJS_Plugin.h"
#include "modules/MEntities.h"
#include "SMJS_Interfaces.h"

#include <conio.h>

WRAPPED_CLS_CPP(SMJS_EntKeyValues, SMJS_BaseWrapped)

SMJS_EntKeyValues::SMJS_EntKeyValues(){
	
}

SMJS_EntKeyValues::~SMJS_EntKeyValues(){
	
}


void SMJS_EntKeyValues::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	SMJS_BaseWrapped::OnWrapperAttached(plugin, wrapper);

}

v8::Handle<v8::Value> SMJS_EntKeyValues::GetKeyValue(v8::Local<v8::String> prop, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_EntKeyValues *self = dynamic_cast<SMJS_EntKeyValues*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	char tmp[128];
	v8::String::AsciiValue propStr(prop);

	if(!serverTools->GetKeyValue(self->entWrapper->ent, *propStr, &tmp[0], sizeof(tmp))){
		return v8::Undefined();
	}

	if(tmp[0] != '\0'){
		auto value = *((char**) tmp);
		return v8::String::New(value);
	}
	return v8::Undefined();
}

v8::Handle<v8::Value> SMJS_EntKeyValues::SetKeyValue(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_EntKeyValues *self = dynamic_cast<SMJS_EntKeyValues*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	v8::String::AsciiValue propStr(prop);

	if(value->IsString()){
		v8::String::AsciiValue valueStr(value->ToString());
		serverTools->SetKeyValue(self->entWrapper->ent, *propStr, *valueStr);
		return value;
	}else if(value->IsNumber()){
		serverTools->SetKeyValue(self->entWrapper->ent, *propStr, (float) value->NumberValue());
		return value;
	}

	THROW("Entity KeyValues can only be strings or floats, for other types, vector for example, use: \"0.0 0.0 0.0\"");
}