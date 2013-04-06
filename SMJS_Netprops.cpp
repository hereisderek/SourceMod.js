#include "SMJS_Netprops.h"
#include "SMJS_Entity.h"
#include "SMJS_Plugin.h"
#include "k7v8macros.h"
#include "modules/MEntities.h"
#include "boost/bind.hpp"

WRAPPED_CLS_CPP(SMJS_Netprops, SMJS_BaseWrapped)

struct SMJS_Netprops_CachedValueData {
	SMJS_Entity *entWrapper;
	void *addr;
	edict_t *edict;
	size_t actual_offset;
	SendProp *prop;
};

struct SMJS_Netprops_CachedDataTable : public SMJS_Netprops_CachedValueData {
	SendTable *pTable;
};

void DestroyDataCallback(Isolate* isolate, v8::Persistent<v8::Value> object, void *parameter){
	delete parameter;
}

SMJS_Netprops::SMJS_Netprops() {
	
}

SMJS_Netprops::~SMJS_Netprops(){
	for(auto plIt = cachedValues.begin(); plIt != cachedValues.end(); ++plIt){
		auto pl = GetPlugin(plIt->first);
		for(auto it = plIt->second.begin(); it != plIt->second.end(); ++it){
			auto obj = v8::Handle<v8::Object>::Cast(it->second);
			if(obj.IsEmpty()) continue;
			auto hiddenValue = obj->GetHiddenValue(v8::String::New("SMJS::dataPtr"));
			if(hiddenValue.IsEmpty() || hiddenValue->IsUndefined() || hiddenValue->IsNull()) continue;
			SMJS_Netprops_CachedValueData *data = (SMJS_Netprops_CachedValueData *) v8::Handle<External>::Cast(hiddenValue)->Value();

			it->second.MakeWeak(pl->GetIsolate(), data, DestroyDataCallback);
		}
	}
}


void SMJS_Netprops::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	SMJS_BaseWrapped::OnWrapperAttached(plugin, wrapper);
	cachedValues.insert(std::make_pair(plugin->id, std::map<std::string, v8::Persistent<v8::Value>>()));
}

v8::Handle<v8::Value> VectorGetter(Local<String> prop,  const AccessorInfo& info){
	SMJS_Netprops_CachedValueData *data = (SMJS_Netprops_CachedValueData *) v8::Handle<External>::Cast(info.Data())->Value();
	if(!data->entWrapper->valid) return v8::Undefined();

	Vector *vec = (Vector*)data->addr;
	if(prop == v8::String::New("x")){
		return v8::Number::New(vec->x);
	}else if(prop == v8::String::New("y")){
		return v8::Number::New(vec->y);
	}else if(prop == v8::String::New("z")){
		return v8::Number::New(vec->z);
	}

	return v8::Undefined();
}

void VectorSetter(Local<String> prop, Local<Value> value, const AccessorInfo& info){
	SMJS_Netprops_CachedValueData *data = (SMJS_Netprops_CachedValueData *) v8::Handle<External>::Cast(info.Data())->Value();
	if(!data->entWrapper->valid) return;

	Vector *vec = (Vector*)data->addr;
	if(prop == v8::String::New("x")){
		vec->x = value->ToNumber()->NumberValue();
	}else if(prop == v8::String::New("y")){
		vec->y = value->ToNumber()->NumberValue();
	}else if(prop == v8::String::New("z")){
		vec->z = value->ToNumber()->NumberValue();
	}

	gamehelpers->SetEdictStateChanged(data->edict, data->actual_offset);
}

v8::Handle<v8::Value> VectorToString(const v8::Arguments& args) {
	char buffer[256];
	auto t = args.This();
	snprintf(buffer, sizeof(buffer), "[Vector %f %f %f]",
		(float) t->Get(v8::String::New("x"))->ToNumber()->NumberValue(),
		(float) t->Get(v8::String::New("y"))->ToNumber()->NumberValue(),
		(float) t->Get(v8::String::New("z"))->ToNumber()->NumberValue()
	);
	return v8::String::New(buffer);
}

unsigned int strncopy(char *dest, const char *src, size_t count){
	if (!count){
		return 0;
	}

	char *start = dest;
	while ((*src) && (--count)){
		*dest++ = *src++;
	}
	*dest = '\0';

	return (dest - start);
}

v8::Handle<v8::Value> SMJS_Netprops::SGetNetProp(v8::Local<v8::String> prop, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_Netprops *self = dynamic_cast<SMJS_Netprops*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	v8::String::AsciiValue str(prop);
	return self->GetNetProp(*str);
}


v8::Handle<v8::Value> SMJS_Netprops::SSetNetProp(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_Netprops *self = dynamic_cast<SMJS_Netprops*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	if(!self->entWrapper->valid) return v8::Undefined();
	
	std::string propNameStdString(*v8::String::AsciiValue(prop));
	sm_sendprop_info_t propInfo;
	
	IServerUnknown *pUnk = (IServerUnknown *)self->entWrapper->ent;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	if(!pNet){
		THROW("Entity is not networkable");
	}

	edict_t *edict = pNet->GetEdict();

	
	if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), *v8::String::AsciiValue(prop), &propInfo)){
		return v8::Undefined();
	}

	//GenerateThenFindCachedValue(PLUGIN_ID plId, std::string key, SMJS_Entity *entWrapper, SendProp *p, size_t offset)
	boost::function<v8::Persistent<v8::Value> ()> f(boost::bind(&SMJS_Netprops::GenerateThenFindCachedValue, self, GetPluginRunning()->id, propNameStdString, self->entWrapper, propInfo.prop, propInfo.actual_offset));

	return SSetNetProp(self->entWrapper, propInfo.prop, propInfo.actual_offset, value, f);
}

v8::Handle<v8::Value> SMJS_Netprops::SSetNetProp(SMJS_Entity *entWrapper, SendProp *p, size_t offset, v8::Local<v8::Value> value, boost::function<v8::Persistent<v8::Value> ()> getCache){
	std::string propNameStdString(p->GetName());

	IServerUnknown *pUnk = (IServerUnknown *)entWrapper->ent;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	if(!pNet){
		THROW("Entity is not networkable");
	}

	edict_t *edict = pNet->GetEdict();

	int bit_count = p->m_nBits;
	bool is_unsigned = ((p->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);
	void *addr = (void*) ((intptr_t) entWrapper->ent + offset);


	switch(p->GetType()){
	case DPT_Int: {
		// If it's an integer with 21 bits and starts with m_h, it MUST be an entity, if it's not...
		//  oh well... return an integer
		if(bit_count == 21 && strlen(p->GetName()) >= 3 && std::strncmp(propNameStdString.c_str(), "m_h", 3) == 0){
			CBaseHandle &hndl = *(CBaseHandle *)addr;
			if(value->IsNull() || value->IsUndefined()){
				hndl.Set(NULL);
			}else{
				auto obj = value->ToObject();
				if(!obj.IsEmpty()){
					Local<Value> _intfld = obj->GetInternalField(0);
					SMJS_Entity* ent = dynamic_cast<SMJS_Entity*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());
					if(ent == NULL){
						return v8::ThrowException(v8::Exception::TypeError(v8::String::New("Entity field can only contain an entity")));
					}

					
					IHandleEntity *pHandleEnt = (IHandleEntity *)ent->ent;
					hndl.Set(pHandleEnt);
				}
			}
		}
		
		if (bit_count < 1){
			*(int32_t * )addr = value->ToInt32()->Int32Value();
		}else if (bit_count >= 17){
			*(int32_t * )addr = value->ToInt32()->Int32Value();
		}else if (bit_count >= 9){
			if (is_unsigned){
				*(uint16_t * )addr = value->ToInt32()->Int32Value();
			}else{
				*(int16_t * )addr = value->ToInt32()->Int32Value();
			}
		}else if (bit_count >= 2){
			if (is_unsigned){
				*(uint8_t * )addr = value->ToInt32()->Int32Value();
			}else{
				*(int8_t * )addr = value->ToInt32()->Int32Value();
			}
		}else{
			*(bool *)addr = value->BooleanValue();
		}

		*(int32_t * )addr = value->ToInt32()->Int32Value();
	}; break;
	case DPT_Float:
		*(float*)addr = (float) value->ToNumber()->NumberValue();
		break;
	case DPT_Vector: {
		auto obj = value->ToObject();
		if(obj.IsEmpty()) return v8::ThrowException(v8::Exception::TypeError(v8::String::New("You can only set a vector field to a vector object")));
		if(!obj->Has(v8::String::New("x")) || !obj->Has(v8::String::New("y")) || !obj->Has(v8::String::New("z"))){
			 return v8::ThrowException(v8::Exception::TypeError(v8::String::New("You can only set a vector field to a vector object that has the properties x, y and z")));
		}

		auto tmp = getCache();
		if(tmp.IsEmpty()){
			SGetNetProp(entWrapper, p, offset, NULL);
			tmp = getCache();
		}

		auto vec = tmp->ToObject();

		vec->Set(v8::String::New("x"), obj->Get(v8::String::New("x")));
		vec->Set(v8::String::New("y"), obj->Get(v8::String::New("y")));
		vec->Set(v8::String::New("z"), obj->Get(v8::String::New("z")));
	}; break;

	case DPT_String: {
		int maxlen = DT_MAX_STRING_BUFFERSIZE;
		v8::String::AsciiValue param1(value->ToString());
		strncopy((char*)addr, *param1, maxlen);
	}; break;

	default: THROW("Invalid Netprop Type");
	}

	gamehelpers->SetEdictStateChanged(edict, offset);
	return value;
}

v8::Handle<v8::Value> SMJS_Netprops::GetNetProp(const char *prop){
	if(!entWrapper->valid) return v8::Undefined();
	
	std::string propNameStdString(prop);

	auto cachedValue = FindCachedValue(GetPluginRunning()->id, propNameStdString);
	if(!cachedValue.IsEmpty()) return cachedValue;

	sm_sendprop_info_t propInfo;
	
	IServerUnknown *pUnk = (IServerUnknown *)entWrapper->ent;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	if(!pNet){
		THROW("Entity is not networkable");
	}

	edict_t *edict = pNet->GetEdict();
	
	if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &propInfo)){
		return v8::Undefined();
	}

	bool isCacheable;
	auto ret = SGetNetProp(entWrapper, propInfo.prop, propInfo.actual_offset, &isCacheable);

	if(isCacheable){
		InsertCachedValue(GetPluginRunning()->id, propNameStdString, v8::Persistent<v8::Value>::New(ret));
	}

	return ret;
}

v8::Handle<v8::Value> SMJS_Netprops::SGetNetProp(SMJS_Entity *entWrapper, SendProp *p, size_t offset, bool *isCacheable){
	if(!entWrapper->valid) return v8::Undefined();
	
	std::string propNameStdString(p->GetName());
	
	IServerUnknown *pUnk = (IServerUnknown *)entWrapper->ent;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	if(!pNet){
		THROW("Entity is not networkable");
	}

	edict_t *edict = pNet->GetEdict();
	int bit_count = p->m_nBits;
	bool is_unsigned = ((p->GetFlags() & SPROP_UNSIGNED) == SPROP_UNSIGNED);
	void *addr = (void*) ((intptr_t) entWrapper->ent + offset);

	if(isCacheable != NULL) *isCacheable = false;

	switch(p->GetType()){
	case DPT_Int: {
		int v;
		// If it's an integer with 21 bits and starts with m_h, it MUST be an entity, if it's not...
		//  oh well... return an integer
		if(bit_count == 21 && strlen(p->GetName()) >= 3 && std::strncmp(propNameStdString.c_str(), "m_h", 3) == 0){
			CBaseHandle &hndl = *(CBaseHandle *)addr;
			CBaseEntity *pHandleEntity = gamehelpers->ReferenceToEntity(hndl.GetEntryIndex());
			
			if (pHandleEntity && hndl == reinterpret_cast<IHandleEntity *>(pHandleEntity)->GetRefEHandle()){
				SMJS_Entity *entity = GetEntityWrapper(pHandleEntity);
				return entity->GetWrapper(GetPluginRunning());
			}else{
				return v8::Null();
			}
		}
		
		if (bit_count < 1){
			v = *(int32_t * )addr;
		}else if (bit_count >= 17){
			v = *(int32_t *) addr;
		}else if (bit_count >= 9){
			if (is_unsigned){
				v = *(uint16_t *) addr;
			}else{
				v = *(int16_t *) addr;
			}
		}else if (bit_count >= 2){
			if (is_unsigned){
				v = *(uint8_t *) addr;
			}else{
				v = *(int8_t *) addr;
			}
		}else{
			return v8::Boolean::New((*(bool *)addr));
		}
		return v8::Int32::New(v);
	}; break;
	case DPT_Float: return v8::Number::New(*(float*)addr);
	case DPT_Vector: {
		auto obj = v8::Object::New();
		auto data = new SMJS_Netprops_CachedValueData();
		data->entWrapper = entWrapper;
		data->addr = addr;
		data->actual_offset = offset;
		data->edict = edict;
		data->prop = p;

		auto ext = v8::External::New(data);
		obj->SetHiddenValue(v8::String::New("SMJS::dataPtr"), ext);
		obj->SetAccessor(v8::String::New("x"), VectorGetter, VectorSetter, ext);
		obj->SetAccessor(v8::String::New("y"), VectorGetter, VectorSetter, ext);
		obj->SetAccessor(v8::String::New("z"), VectorGetter, VectorSetter, ext);
		obj->Set(v8::String::New("toString"), v8::FunctionTemplate::New(VectorToString)->GetFunction());

		if(isCacheable != NULL) *isCacheable = true;
		return obj;
	}; break;

	case DPT_String: return v8::String::New((char *)addr);

	case DPT_DataTable: {
		SendTable *pTable = p->GetDataTable();
		if (!pTable){
			THROW_VERB("Error looking up DataTable for prop %s", p->GetName());
		}

		SMJS_DataTable *table = new SMJS_DataTable(GetPluginRunning(), entWrapper, pTable, offset);
		if(isCacheable != NULL) *isCacheable = true;
		return table->GetWrapper();
	}break;

	default: THROW("Invalid Netprop Type");
	}

	return v8::Undefined();
}

SIMPLE_WRAPPED_CLS_CPP(SMJS_DataTable, SMJS_SimpleWrapped);

v8::Handle<v8::Value> SMJS_DataTable::DTGetter(uint32_t index, const AccessorInfo& info){
	SMJS_DataTable *self = (SMJS_DataTable*) v8::Handle<v8::External>::Cast(info.This()->GetInternalField(0))->Value();
	if(!self->entWrapper->valid) return v8::Undefined();

	// Index can't be < 0, don't even test for it
	if(index >= (uint32_t) self->pTable->GetNumProps()) THROW_VERB("Index %d out of bounds", index);

	auto it = self->cachedValues.find(index);
	if(it != self->cachedValues.end()){
		return it->second;
	}

	auto pProp = self->pTable->GetProp(index);

	bool isCacheable;
	auto res = SMJS_Netprops::SGetNetProp(self->entWrapper, pProp, self->offset + pProp->GetOffset(), &isCacheable);
	if(isCacheable){
		self->cachedValues.insert(std::make_pair(index, res));
	}

	return res;
}

v8::Handle<v8::Value> SMJS_DataTable::DTSetter(uint32_t index, Local<Value> value, const AccessorInfo& info){
	SMJS_DataTable *self = (SMJS_DataTable*) v8::Handle<v8::External>::Cast(info.This()->GetInternalField(0))->Value();
	if(!self->entWrapper->valid) return v8::Undefined();

	if(index >= (uint32_t) self->pTable->GetNumProps()) THROW_VERB("Index %d out of bounds", index);

	auto pProp = self->pTable->GetProp(index);
	boost::function<v8::Persistent<v8::Value> ()> f(boost::bind(&SMJS_DataTable::GenerateThenFindCachedValue, self, index, self->entWrapper, pProp, self->offset + pProp->GetOffset()));

	return SMJS_Netprops::SSetNetProp(self->entWrapper, pProp, self->offset + pProp->GetOffset(), value, f);
}