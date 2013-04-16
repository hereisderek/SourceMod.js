#ifndef _INCLUDE_SMJS_CONVAR_H_
#define _INCLUDE_SMJS_CONVAR_H_

#include "SMJS.h"
#include "SMJS_SimpleWrapped.h"
#include "SMJS_Interfaces.h"

class SMJS_ConVar : public SMJS_SimpleWrapped {
private:
	SMJS_ConVar() : SMJS_SimpleWrapped(NULL) {};

public:
	ConVar *cv;
	SMJS_ConVar(SMJS_Plugin *pl, ConVar *cv) : SMJS_SimpleWrapped(pl), cv(cv) {
		
	}

	FUNCTION_DECL(getName);

	FUNCTION_DECL(getBool);
	FUNCTION_DECL(getInt);
	FUNCTION_DECL(getNumber);
	FUNCTION_DECL(getString);

	FUNCTION_DECL(setBool);
	FUNCTION_DECL(setInt);
	FUNCTION_DECL(setNumber);
	FUNCTION_DECL(setString);
	
	SIMPLE_WRAPPED_CLS(SMJS_ConVar, SMJS_SimpleWrapped){
		temp->SetClassName(v8::String::New("ConVar"));

		WRAPPED_FUNC(getName);
		temp->InstanceTemplate()->SetAccessor(v8::String::New("flags"), GetFlags, SetFlags);

		WRAPPED_FUNC(getBool);
		WRAPPED_FUNC(getInt);
		WRAPPED_FUNC(getNumber);
		WRAPPED_FUNC(getString);

		WRAPPED_FUNC(setBool);
		WRAPPED_FUNC(setInt);
		WRAPPED_FUNC(setNumber);
		WRAPPED_FUNC(setString);
	}

	void OnWrapperAttached(){
		auto obj = wrapper->ToObject();

		
	}

	static Handle<Value> GetFlags(Local<String> prop, const AccessorInfo& info);
	static void SetFlags(Local<String> prop, Local<Value> value, const AccessorInfo& info);
};

#endif