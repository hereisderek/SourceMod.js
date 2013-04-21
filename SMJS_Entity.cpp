#include "SMJS_Entity.h"
#include "SMJS_Plugin.h"
#include "datamap.h"

#define SIZEOF_VARIANT_T		20

#define ENTWRAPPER_TO_CBASEENTITY(arg, buffer) \
	auto _obj##arg = args[arg]->ToObject(); \
	if(_obj##arg.IsEmpty()) THROW_VERB("Argument %d is not an entity", arg); \
	Local<Value> _intfld##arg = _obj##arg->GetInternalField(0); \
	SMJS_Entity* _ptr##arg = dynamic_cast<SMJS_Entity*>((SMJS_Base*)Handle<External>::Cast(_intfld)->Value()); \
	if(!_ptr##arg) THROW_VERB("Argument %d is not an entity", arg); \
	buffer = _ptr##arg->ent;
	


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

FUNCTION_M(SMJS_Entity::input)
	static ICallWrapper *g_pAcceptInput = NULL;
	if (!g_pAcceptInput){
		int offset;
		if (!sdkToolsConf->GetOffset("AcceptInput", &offset)){
			THROW("\"AcceptEntityInput\" not supported by this mod");
		}
	
		PassInfo pass[6];
		pass[0].type = PassType_Basic;
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].size = sizeof(const char *);
		pass[1].type = pass[2].type = PassType_Basic;
		pass[1].flags = pass[2].flags = PASSFLAG_BYVAL;
		pass[1].size = pass[2].size = sizeof(CBaseEntity *);
		pass[3].type = PassType_Object;
		pass[3].flags = PASSFLAG_BYVAL|PASSFLAG_OCTOR|PASSFLAG_ODTOR|PASSFLAG_OASSIGNOP;
		pass[3].size = SIZEOF_VARIANT_T;
		pass[4].type = PassType_Basic;
		pass[4].flags = PASSFLAG_BYVAL;
		pass[4].size = sizeof(int);
		pass[5].type = PassType_Basic;
		pass[5].flags = PASSFLAG_BYVAL;
		pass[5].size = sizeof(bool);
	
		if (!(g_pAcceptInput=binTools->CreateVCall(offset, 0, 0, &pass[5], pass, 5))){
			THROW("\"AcceptEntityInput\" wrapper failed to initialized");
		}
	}

	ARG_BETWEEN(1, 4);

	CBaseEntity *pActivator, *pCaller, *pDest;
	
	GET_INTERNAL(SMJS_Entity*, self);

	char *inputname;
	unsigned char vstk[sizeof(void *) + sizeof(const char *) + sizeof(CBaseEntity *)*2 + SIZEOF_VARIANT_T + sizeof(int)];
	unsigned char *vptr = vstk;
	
	pDest = self->ent;

	PSTR(inputNameTmp);
	inputname = *inputNameTmp;

	if(args.Length() >= 3){
		ENTWRAPPER_TO_CBASEENTITY(2, pActivator);
	}else{
		pActivator = NULL;
	}

	if(args.Length() >= 4){
		ENTWRAPPER_TO_CBASEENTITY(3, pCaller);
	}else{
		pCaller = NULL;
	}

	unsigned char variantTmp[SIZEOF_VARIANT_T] = {0};
	unsigned char *variant = variantTmp;

	if(args.Length() < 2){
		*(int *)variant = 0;
		variant += sizeof(int)*3;
		*(unsigned long *)variant = INVALID_EHANDLE_INDEX;
		variant += sizeof(unsigned long);
		*(fieldtype_t *)variant = FIELD_VOID;
	}else{
		auto &value = args[1];
		if(value->IsString()){
			v8::String::Utf8Value str(value);
			*(string_t *)variant = MAKE_STRING(*str);
			vptr += sizeof(int)*3 + sizeof(unsigned long);
			*(fieldtype_t *)variant = FIELD_STRING;
		}else if(value->IsInt32()){
			*(int *)variant = value->Int32Value();
			variant += sizeof(int)*3 + sizeof(unsigned long);
			*(fieldtype_t *)variant = FIELD_INTEGER;
		}else if(value->IsNumber()){
			*(float *)variant = (float) value->NumberValue();
			variant += sizeof(int)*3 + sizeof(unsigned long);
			*(fieldtype_t *)variant = FIELD_FLOAT;
		}else if(value->IsBoolean()){
			*(bool *)variant = value->BooleanValue();
			variant += sizeof(int)*3 + sizeof(unsigned long);
			*(fieldtype_t *)variant = FIELD_BOOLEAN;
		}else if(value->IsObject()){
			auto obj = value->ToObject();
			if(obj.IsEmpty()) THROW("Invalid input type");
			SMJS_Entity* smjsEnt = dynamic_cast<SMJS_Entity*>((SMJS_Base*)Handle<External>::Cast(obj->GetInternalField(0))->Value());
		
			if(smjsEnt != NULL){
				CBaseHandle bHandle = reinterpret_cast<IHandleEntity *>(smjsEnt->ent)->GetRefEHandle();
			
				variant += sizeof(int)*3;
				*(unsigned long *)variant = (unsigned long)(bHandle.ToInt());
				variant += sizeof(unsigned long);
				*(fieldtype_t *)variant = FIELD_EHANDLE;

			// TODO Add support for vectors
			}else{
				THROW("Invalid input type");
			}
		}else{
			THROW("Invalid input type");
		}
	}

	*(void **)vptr = pDest;
	vptr += sizeof(void *);
	*(const char **)vptr = inputname;
	vptr += sizeof(const char *);
	*(CBaseEntity **)vptr = pActivator;
	vptr += sizeof(CBaseEntity *);
	*(CBaseEntity **)vptr = pCaller;
	vptr += sizeof(CBaseEntity *);
	memcpy(vptr, variantTmp, SIZEOF_VARIANT_T);
	vptr += SIZEOF_VARIANT_T;
	*(int *)vptr = 0; // Unknown
	
	bool ret;
	g_pAcceptInput->Execute(vstk, &ret);
	return v8::Boolean::New(ret);
END

FUNCTION_M(SMJS_Entity::removeEdict)
	GET_INTERNAL(SMJS_Entity*, self);

	if(self->edict == NULL) THROW("Entity does not have an edict");

	engine->RemoveEdict(self->edict);

	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Entity::setData)
	GET_INTERNAL(SMJS_Entity*, self);

	PINT(offset);
	PINT(size);
	PINT(value);

	if(offset < 0 || offset > (2 << 15)){
		THROW_VERB("Invalid offset %d", offset);
	}

	switch(size){
	case 1:
		*(uint8_t*)((intptr_t) self->ent + offset) = value;
		break;
	case 2:
		*(uint16_t*)((intptr_t) self->ent + offset) = value;
		break;
	case 4:
		*(uint32_t*)((intptr_t) self->ent + offset) = value;
		break;

	default: THROW_VERB("Invalid size %d", size);
	}

	RETURN_UNDEF;
END