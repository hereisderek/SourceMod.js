#ifndef _INCLUDE_SMJS_ENTITY_H_
#define _INCLUDE_SMJS_ENTITY_H_

#include "SMJS.h"
#include "SMJS_BaseWrapped.h"
#include "SMJS_Netprops.h"
#include "SMJS_EntKeyValues.h"

class CBaseEntity;

class SMJS_Entity : public SMJS_BaseWrapped {
public:
	CBaseEntity *ent;
	int entIndex;
	bool valid;
	bool isEdict;
	SMJS_Netprops netprops;
	SMJS_EntKeyValues keyvalues;

	SMJS_Entity(CBaseEntity *ent);
	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	FUNCTION_DECL(isValid);
	FUNCTION_DECL(getClassname);

	WRAPPED_CLS(SMJS_Entity, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("Entity"));
		proto->Set("netprops", v8::Null());
		proto->Set("keyvalues", v8::Null());

		WRAPPED_FUNC(isValid);
		WRAPPED_FUNC(getClassname);
	}

private:
	SMJS_Entity();
};

#endif
