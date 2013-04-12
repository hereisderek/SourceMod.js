#include "MEntities.h"
#include "SMJS_Plugin.h"
#include "SMJS_Entity.h"

WRAPPED_CLS_CPP(MEntities, SMJS_Module);


std::unordered_map<cell_t, SMJS_Entity*> refs;
SMJS_Entity* GetEntityWrapper(int32_t ref){
	if(ref == -1) return NULL;

	auto it = refs.find(ref);
	if(it != refs.end()) return it->second;


	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(ref);

	if(pEntity == NULL) return NULL;

	IServerUnknown *pUnk = (IServerUnknown *)pEntity;
	IServerNetworkable *pNet = pUnk->GetNetworkable();

	SMJS_Entity *entity = new SMJS_Entity(pEntity);
	refs.insert(std::pair<cell_t, SMJS_Entity*>(ref, entity));
	return entity;
}

SMJS_Entity* GetEntityWrapper(CBaseEntity *pEntity){
	if(pEntity == NULL) return NULL;

	auto ref = gamehelpers->EntityToReference(pEntity);
	auto it = refs.find(ref);
	if(it != refs.end()) return it->second;

	IServerUnknown *pUnk = (IServerUnknown *)pEntity;
	IServerNetworkable *pNet = pUnk->GetNetworkable();

	SMJS_Entity *wrapper = new SMJS_Entity(pEntity);
	refs.insert(std::make_pair(ref, wrapper));
	return wrapper;
}

void SetEntityWrapper(CBaseEntity *pEntity, SMJS_Entity *wrapper){
	auto ref = gamehelpers->EntityToReference(pEntity);
	refs.insert(std::make_pair(ref, wrapper));
}

void ClearEntityWrappers(){
	for(auto it = refs.begin(); it != refs.end(); ++it){
		it->second->Destroy();
	}
	refs.clear();
}

MEntities::MEntities(){
	identifier = "entities";
}

void MEntities::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	
}
