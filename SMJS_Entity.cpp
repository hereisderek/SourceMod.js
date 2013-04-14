#include "SMJS_Entity.h"
#include "SMJS_Plugin.h"

WRAPPED_CLS_CPP(SMJS_Entity, SMJS_BaseWrapped)

SMJS_Entity::SMJS_Entity(CBaseEntity *ent){
	this->ent = NULL;

	datamaps.entWrapper = this;
	netprops.entWrapper = this;
	isEdict = false;
	entIndex = -1;
	
	SetEntity(ent);

	keyvalues.entWrapper = this;
}

void SMJS_Entity::SetEntity(CBaseEntity *ent){
	if(this->ent != NULL){
		if(this->ent != ent){
			throw "Cannot set entity twice";
		}

		return;
	}

	if(ent == NULL) return;

	this->ent = ent;

	IServerUnknown *pUnk = (IServerUnknown *)ent;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	if(pNet){
		edict = pNet->GetEdict();
		entIndex = gamehelpers->IndexOfEdict(edict);
	}

	this->valid = true;
}

Handle<Value> GetEntityIndex(Local<String> prop, const AccessorInfo& info){
	Local<Value> _intfld = info.This()->GetInternalField(0); \
	SMJS_Entity* self = dynamic_cast<SMJS_Entity*>((SMJS_Base*)Handle<External>::Cast(_intfld)->Value());

	return v8::Int32::New(self->entIndex);
}

void SMJS_Entity::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	obj->SetAccessor(v8::String::New("index"), GetEntityIndex);

	obj->Set(v8::String::New("netprops"), netprops.GetWrapper(plugin));
	obj->Set(v8::String::New("keyvalues"), keyvalues.GetWrapper(plugin));
	obj->Set(v8::String::New("datamaps"), datamaps.GetWrapper(plugin));

}

FUNCTION_M(SMJS_Entity::isValid)
	GET_INTERNAL(SMJS_Entity*, self);
	return v8::Boolean::New(self->valid);
END

FUNCTION_M(SMJS_Entity::getClassname)
	GET_INTERNAL(SMJS_Entity*, self);
	if(!self->valid) THROW("Invalid entity");
	
	return v8::String::New(gamehelpers->GetEntityClassname(self->ent));
END