#ifndef _INCLUDE_SMJS_NETPROPS_H_
#define _INCLUDE_SMJS_NETPROPS_H_

#include "SMJS.h"
#include "smsdk_ext.h"
#include <map>


#include "SMJS_BaseWrapped.h"
#include "SMJS_SimpleWrapped.h"
#include "dt_send.h"
#include "server_class.h"

#include "boost/function.hpp"
#include "boost/bind.hpp"


class SMJS_Entity;
class SendProp;
class SMJS_DataTable;

class SMJS_BaseNetprop {
public:
	SMJS_Entity *entWrapper;
};

class SMJS_Netprops : public SMJS_BaseWrapped, public SMJS_BaseNetprop {
public:
	
	std::map<PLUGIN_ID, std::map<std::string, v8::Persistent<v8::Value>>> cachedValues;

	SMJS_Netprops();
	~SMJS_Netprops();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	v8::Handle<v8::Value> GetNetProp(const char *prop);

	
	WRAPPED_CLS(SMJS_Netprops, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("Netprops"));
		temp->InstanceTemplate()->SetNamedPropertyHandler(SGetNetProp, SSetNetProp);
	}

protected:
	inline void InsertCachedValue(PLUGIN_ID plId, std::string key, v8::Persistent<v8::Value> value){
		auto &vec = cachedValues.find(plId)->second;
		vec.insert(std::make_pair(key, value));
	}

	inline v8::Persistent<v8::Value> FindCachedValue(PLUGIN_ID plId, std::string key){
		auto &vec = cachedValues.find(plId)->second;
		auto it = vec.find(key);
		if(it != vec.end()){
			return it->second;
		}

		return v8::Persistent<v8::Value>();
	}

	v8::Persistent<v8::Value> GenerateThenFindCachedValue(PLUGIN_ID plId, std::string key, SMJS_Entity *entWrapper, SendProp *p, size_t offset){
		bool isCacheable;
		auto res = v8::Persistent<v8::Value>::New(SGetNetProp(entWrapper, p, offset, &isCacheable));

		if(isCacheable){
			InsertCachedValue(plId, key, res);
		}

		return res;
	}

	static v8::Handle<v8::Value> SGetNetProp(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> SSetNetProp(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

	static v8::Handle<v8::Value> SGetNetProp(SMJS_Entity *entWrapper, SendProp *p, size_t offset, bool *isCacheable = NULL);
	static v8::Handle<v8::Value> SSetNetProp(SMJS_Entity *entWrapper, SendProp *p, size_t offset, v8::Local<v8::Value> value, boost::function<v8::Persistent<v8::Value> ()> getCache);
	friend SMJS_DataTable;
};

class SMJS_DataTable : public SMJS_SimpleWrapped, public SMJS_BaseNetprop {
private:
	SendTable *pTable;
	size_t offset;
	std::map<uint32_t, v8::Persistent<v8::Value>> cachedValues;

	static v8::Handle<v8::Value> DTGetter(uint32_t index, const AccessorInfo& info);
	static v8::Handle<v8::Value> DTSetter(uint32_t index, Local<Value> value, const AccessorInfo& info);

	SMJS_DataTable() : SMJS_SimpleWrapped(NULL) {};

	v8::Persistent<v8::Value> GenerateThenFindCachedValue(uint32_t key, SMJS_Entity *entWrapper, SendProp *p, size_t offset){
		bool isCacheable;
		auto res = v8::Persistent<v8::Value>::New(SMJS_Netprops::SGetNetProp(entWrapper, p, offset, &isCacheable));

		if(isCacheable){
			cachedValues.insert(std::make_pair(key, res));
		}

		return res;
	}

public:


	SMJS_DataTable(SMJS_Plugin *pl, SMJS_Entity *_entWrapper, SendTable *_pTable, size_t _offset) : SMJS_SimpleWrapped(pl), pTable(_pTable), offset(_offset){
		entWrapper = _entWrapper;
	}

	SIMPLE_WRAPPED_CLS(SMJS_DataTable, SMJS_SimpleWrapped){
		proto->SetIndexedPropertyHandler(DTGetter, DTSetter, NULL, NULL, NULL);
	}

	void OnWrapperAttached(){
		auto obj = wrapper->ToObject();
	}
};


#endif
