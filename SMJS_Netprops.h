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
class SMJS_GameRulesProps;

class SMJS_NPValueCacher {
public:
	std::map<PLUGIN_ID, std::map<std::string, v8::Persistent<v8::Value>>> cachedValues;

protected:
	void InitCacheForPlugin(SMJS_Plugin *pl);
	inline void InsertCachedValue(PLUGIN_ID plId, std::string key, v8::Persistent<v8::Value> value);
	inline v8::Persistent<v8::Value> FindCachedValue(PLUGIN_ID plId, std::string key);
	v8::Persistent<v8::Value> GenerateThenFindCachedValue(PLUGIN_ID plId, std::string key, void *ent, SendProp *p, size_t offset);
};

class SMJS_Netprops : public SMJS_BaseWrapped, public SMJS_NPValueCacher {
public:
	bool valid;
	void *ent;

	std::map<PLUGIN_ID, std::map<std::string, v8::Persistent<v8::Value>>> cachedValues;

	SMJS_Netprops();
	~SMJS_Netprops();

	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);

	
	WRAPPED_CLS(SMJS_Netprops, SMJS_BaseWrapped) {
		temp->SetClassName(v8::String::New("Netprops"));
		temp->InstanceTemplate()->SetNamedPropertyHandler(SGetNetProp, SSetNetProp);
	}

	static bool GetEntityPropInfo(void *ent, const char *propName, sm_sendprop_info_t *propInfo);

protected:
	static v8::Handle<v8::Value> SGetNetProp(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> SSetNetProp(v8::Local<v8::String> prop, v8::Local<v8::Value> value, const v8::AccessorInfo &info);

	static v8::Handle<v8::Value> SGetNetProp(void *ent, SendProp *p, size_t offset, bool *isCacheable = NULL, const char *name = NULL, bool notAnEntity = false);
	static v8::Handle<v8::Value> SSetNetProp(void *ent, SendProp *p, size_t offset, v8::Local<v8::Value> value, boost::function<v8::Persistent<v8::Value> ()> getCache, const char *name = NULL);
	
	friend SMJS_NPValueCacher;
	friend SMJS_DataTable;
	friend SMJS_GameRulesProps;
};

class SMJS_DataTable : public SMJS_SimpleWrapped {
private:
	void *ent;
	SendTable *pTable;
	size_t offset;
	std::map<uint32_t, v8::Persistent<v8::Value>> cachedValues;

	static v8::Handle<v8::Value> DTGetter(uint32_t index, const AccessorInfo& info);
	static v8::Handle<v8::Value> DTSetter(uint32_t index, Local<Value> value, const AccessorInfo& info);

	SMJS_DataTable() : SMJS_SimpleWrapped(NULL) {};

	v8::Persistent<v8::Value> GenerateThenFindCachedValue(uint32_t key, void *ent, SendProp *p, size_t offset){
		bool isCacheable;
		auto res = v8::Persistent<v8::Value>::New(SMJS_Netprops::SGetNetProp(ent, p, offset, &isCacheable));

		if(isCacheable){
			cachedValues.insert(std::make_pair(key, res));
		}

		return res;
	}

public:


	SMJS_DataTable(SMJS_Plugin *pl, void *_ent, SendTable *_pTable, size_t _offset) : SMJS_SimpleWrapped(pl), pTable(_pTable), offset(_offset){
		ent = _ent;
	}

	SIMPLE_WRAPPED_CLS(SMJS_DataTable, SMJS_SimpleWrapped){
		temp->SetClassName(v8::String::New("DataTable"));
		temp->InstanceTemplate()->SetIndexedPropertyHandler(DTGetter, DTSetter, NULL, NULL, NULL);
	}

	void OnWrapperAttached(){
		auto obj = wrapper->ToObject();
	}
};


#endif
