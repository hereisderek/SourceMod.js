#ifndef _INCLUDE_SMJS_EVENT_H_
#define _INCLUDE_SMJS_EVENT_H_

#include "SMJS.h"
#include "SMJS_BaseWrapped.h"
#include "SMJS_Interfaces.h"

class SMJS_Event : public SMJS_BaseWrapped {
public:
	bool broadcast;
	bool blocked;
	bool post;

	IGameEvent *ev;
	SMJS_Event(IGameEvent *ev, bool broadcast, bool post) :
		ev(ev),
		broadcast(broadcast),
		blocked(false),
		post(post)
	{
		
	};
	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
		auto obj = wrapper->ToObject();
		obj->Set(v8::String::New("name"), v8::String::New(ev->GetName()), v8::ReadOnly);

		if(post){
			obj->SetAccessor(v8::String::New("broadcast"), GetBroadcast);
		}else{
			obj->SetAccessor(v8::String::New("broadcast"), GetBroadcast, SetBroadcast);
		}
	}


	FUNCTION_DECL(getBool);
	FUNCTION_DECL(getInt);
	FUNCTION_DECL(getFloat);
	FUNCTION_DECL(getString);
	FUNCTION_DECL(getUint64);

	FUNCTION_DECL(setBool);
	FUNCTION_DECL(setInt);
	FUNCTION_DECL(setFloat);
	FUNCTION_DECL(setString);
	FUNCTION_DECL(setUint64);

	FUNCTION_DECL(block);

	WRAPPED_CLS(SMJS_Event, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("Event"));

		proto->Set("name", v8::Null());

		WRAPPED_FUNC(getBool);
		WRAPPED_FUNC(getInt);
		WRAPPED_FUNC(getFloat);
		WRAPPED_FUNC(getString);
		WRAPPED_FUNC(getUint64);

		WRAPPED_FUNC(setBool);
		WRAPPED_FUNC(setInt);
		WRAPPED_FUNC(setFloat);
		WRAPPED_FUNC(setString);
		WRAPPED_FUNC(setUint64);

		WRAPPED_FUNC(block);
	}

private:
	SMJS_Event();
	static Handle<Value> GetBroadcast(Local<String> prop, const AccessorInfo& info);
	static void SetBroadcast(Local<String> prop, Local<Value> value, const AccessorInfo& info);
};

#endif
