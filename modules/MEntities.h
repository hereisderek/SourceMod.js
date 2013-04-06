#pragma once

#include "SMJS.h"
#include "SMJS_Module.h"
#include "SMJS_Entity.h"
#include <vector>
#include <map>

class SMJS_Entity;
SMJS_Entity* GetEntityWrapper(int32_t ref);
SMJS_Entity* GetEntityWrapper(CBaseEntity *pEntity);
void SetEntityWrapper(CBaseEntity *pEntity, SMJS_Entity *wrapper);
void ClearEntityWrappers();

class MEntities : public SMJS_Module {
public:
	MEntities();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void Init();


	WRAPPED_CLS(MEntities, SMJS_Module) {
		temp->SetClassName(v8::String::New("EntitiesModule"));
	}

	
};
