#include "SMJS_Event.h"
#include "SMJS_Plugin.h"

WRAPPED_CLS_CPP(SMJS_Event, SMJS_BaseWrapped);

FUNCTION_M(SMJS_Event::getBool)
	GET_INTERNAL(SMJS_Event*, self);
	PSTR(name);
	RETURN_SCOPED(v8::Boolean::New(self->ev->GetBool(*name)));
END

FUNCTION_M(SMJS_Event::getInt)
	GET_INTERNAL(SMJS_Event*, self);
	PSTR(name);
	RETURN_SCOPED(v8::Int32::New(self->ev->GetInt(*name)));
END

FUNCTION_M(SMJS_Event::getFloat)
	GET_INTERNAL(SMJS_Event*, self);
	PSTR(name);
	RETURN_SCOPED(v8::Number::New(self->ev->GetFloat(*name)));
END

FUNCTION_M(SMJS_Event::getString)
	GET_INTERNAL(SMJS_Event*, self);
	PSTR(name);
	RETURN_SCOPED(v8::String::New(self->ev->GetString(*name)));
END

FUNCTION_M(SMJS_Event::getUint64)
	GET_INTERNAL(SMJS_Event*, self);
	PSTR(name);

	uint64 v = self->ev->GetUint64(*name);
	auto arr = v8::Array::New(2);
	arr->Set(0, v8::Number::New(v & 0xFFFFFFFF));
	arr->Set(1, v8::Number::New(v >> 32));
	RETURN_SCOPED(arr);
END

FUNCTION_M(SMJS_Event::setBool)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't set values in posthooks");

	PSTR(name);
	PBOL(value);
	self->ev->SetBool(*name, value);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Event::setInt)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't set values in posthooks");
	
	PSTR(name);
	PINT(value);
	self->ev->SetInt(*name, value);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Event::setFloat)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't set values in posthooks");
	
	PSTR(name);
	PNUM(value);
	self->ev->SetFloat(*name, value);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Event::setString)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't set values in posthooks");

	PSTR(name);
	PUTF8(value);
	self->ev->SetString(*name, *value);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Event::setUint64)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't set values in posthooks");

	ARG_COUNT(2);
	PSTR(name);

	auto value = args[1];
	if(!value->IsArray()) THROW("Value must be an array");
	auto obj = value->ToObject();

	if(obj->Get(v8::String::New("length"))->Uint32Value() != 2) THROW("Value must be an array with 2 elements");

	auto lowBits = obj->Get(0);
	auto highBits = obj->Get(1);
			
	if(!lowBits->IsInt32() || !highBits->IsInt32()) THROW("Value must be an array with 2 int32 elements");

	self->ev->SetUint64(*name, ((int64)highBits->Int32Value()) << 32 | lowBits->Int32Value());
	RETURN_UNDEF;
END


FUNCTION_M(SMJS_Event::block)
	GET_INTERNAL(SMJS_Event*, self);
	if(self->post) THROW("Can't block event in posthooks");
	self->blocked = true;
	RETURN_UNDEF;
END
Handle<Value> SMJS_Event::GetBroadcast(Local<String> prop, const AccessorInfo& info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_Event *self = dynamic_cast<SMJS_Event*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	return v8::Boolean::New(self->broadcast);
}

void SMJS_Event::SetBroadcast(Local<String> prop, Local<Value> value, const AccessorInfo& info){
	Local<Value> _intfld = info.This()->GetInternalField(0); 
	SMJS_Event *self = dynamic_cast<SMJS_Event*>((SMJS_BaseWrapped*)Handle<External>::Cast(_intfld)->Value());

	self->broadcast = value->BooleanValue();
}
