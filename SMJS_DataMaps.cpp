#include "SMJS_DataMaps.h"
#include "SMJS_Plugin.h"
#include "SMJS_Entity.h"

WRAPPED_CLS_CPP(SMJS_DataMaps, SMJS_BaseWrapped);

v8::Handle<v8::Value> SMJS_DataMaps::SGetDataMap(v8::Local<v8::String> prop, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_DataMaps *self = dynamic_cast<SMJS_DataMaps*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	auto ent = self->entWrapper->ent;

	datamap_t *datamap = gamehelpers->GetDataMap(ent);
	if(datamap == NULL) THROW("Entity does not have a datamap");

	v8::String::AsciiValue str(prop);

	typedescription_t *desc = gamehelpers->FindInDataMap(datamap, *str);
	if(desc == NULL) return v8::Undefined();

	

	switch(desc->fieldType){
		case FIELD_INTEGER: return v8::Int32::New(*(int32_t *)((intptr_t)ent + desc->fieldOffset));
		case FIELD_FLOAT:  return v8::Number::New(*(float *)((intptr_t)ent + desc->fieldOffset));

		case FIELD_STRING:
		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME: {
				string_t idx = *(string_t *)((intptr_t)ent + desc->fieldOffset);
				return v8::String::New((idx == NULL_STRING) ? "" : STRING(idx));
		} break;

		default: THROW("Unsupported datamap type");
	}
}

v8::Handle<v8::Value> SMJS_DataMaps::SSetDataMap(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_DataMaps *self = dynamic_cast<SMJS_DataMaps*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	auto ent = self->entWrapper->ent;

	datamap_t *datamap = gamehelpers->GetDataMap(ent);
	if(datamap == NULL) THROW("Entity does not have a datamap");

	v8::String::AsciiValue str(prop);

	typedescription_t *desc = gamehelpers->FindInDataMap(datamap, *str);
	if(desc == NULL) return v8::Undefined();

	

	switch(desc->fieldType){
		case FIELD_INTEGER: 
			*(int32_t *)((intptr_t) ent + desc->fieldOffset) = value->ToInt32()->Int32Value();
			break;
		case FIELD_FLOAT:
			*(float *)((intptr_t) ent + desc->fieldOffset) = (float) value->ToNumber()->NumberValue();
		break;
		case FIELD_STRING:
		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME:
			{
				v8::String::Utf8Value str(value);
				strncpy((char *)((intptr_t)ent + desc->fieldOffset), *str, desc->fieldSize);
			} break;
		default: THROW("Unsupported datamap type");
	}

	return value;
}