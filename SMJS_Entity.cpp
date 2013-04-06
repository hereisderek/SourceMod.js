#include "SMJS_Entity.h"
#include "SMJS_Plugin.h"

WRAPPED_CLS_CPP(SMJS_Entity, SMJS_BaseWrapped)

SMJS_Entity::SMJS_Entity(CBaseEntity *ent){
	this->ent = ent;
	valid = ent != NULL;
	isEdict = false;
	entIndex = -1;

	netprops.entWrapper = this;
	keyvalues.entWrapper = this;

	if(ent != NULL){
		IServerUnknown *pUnk = (IServerUnknown *)ent;
		IServerNetworkable *pNet = pUnk->GetNetworkable();
		if(pNet){
			entIndex = gamehelpers->IndexOfEdict(pNet->GetEdict());
		}
	}
}

void SMJS_Entity::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	obj->Set(v8::String::New("index"), v8::Int32::New(entIndex), ReadOnly);

	obj->Set(v8::String::New("netprops"), netprops.GetWrapper(plugin));
	obj->Set(v8::String::New("keyvalues"), keyvalues.GetWrapper(plugin));

}

FUNCTION_M(SMJS_Entity::isValid)
	GET_INTERNAL(SMJS_Entity*, self);
	return v8::Boolean::New(self->valid);
END

FUNCTION_M(SMJS_Entity::getClassname)
	GET_INTERNAL(SMJS_Entity*, self);
 
	return v8::String::New(gamehelpers->GetEntityClassname(self->ent));
END