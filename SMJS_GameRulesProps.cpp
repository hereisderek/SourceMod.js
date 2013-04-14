#include "SMJS_GameRulesProps.h"
#include "SMJS_Netprops.h"
#include "SMJS_Plugin.h"


WRAPPED_CLS_CPP(SMJS_GameRulesProps, SMJS_BaseWrapped);

SMJS_GameRulesProps *g_GameRulesProp;

SMJS_GameRulesProps::SMJS_GameRulesProps(){
	g_GameRulesProp = this;
}

void SMJS_GameRulesProps::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	SMJS_BaseWrapped::OnWrapperAttached(plugin, wrapper);
	InitCacheForPlugin(plugin);
}

Handle<Value> SMJS_GameRulesProps::GetRulesProp(Local<String> propName, const AccessorInfo& info){
	v8::String::AsciiValue str(propName);
	std::string propNameStdString(*str);

	auto cachedValue = g_GameRulesProp->FindCachedValue(GetPluginRunning()->id, propNameStdString);
	if(!cachedValue.IsEmpty()) return cachedValue;

	
	sm_sendprop_info_t propInfo;
	if(!SMJS_Netprops::GetEntityPropInfo(g_GameRulesProp->proxy, *str, &propInfo)){
		return v8::Undefined();
	}

	bool isCacheable;
	auto ret = SMJS_Netprops::SGetNetProp(g_GameRulesProp->gamerules, NULL, propInfo.prop, propInfo.actual_offset, &isCacheable, NULL);

	if(isCacheable){
		g_GameRulesProp->InsertCachedValue(GetPluginRunning()->id, propNameStdString, v8::Persistent<v8::Value>::New(ret));
	}

	return ret;
}

Handle<Value> SMJS_GameRulesProps::SetRulesProp(Local<String> propName, Local<Value> value, const AccessorInfo& info){
	v8::String::AsciiValue str(propName);
	std::string propNameStdString(*str);

	sm_sendprop_info_t propInfo;
	if(!SMJS_Netprops::GetEntityPropInfo(g_GameRulesProp->proxy, *str, &propInfo)){
		return v8::Undefined();
	}

	boost::function<v8::Persistent<v8::Value> ()> f(boost::bind(&SMJS_GameRulesProps::GenerateThenFindCachedValue, g_GameRulesProp, GetPluginRunning()->id, propNameStdString, propInfo.prop, propInfo.actual_offset));
	
	IServerUnknown *pUnk = (IServerUnknown *)g_GameRulesProp->proxy;
	IServerNetworkable *pNet = pUnk->GetNetworkable();
	edict_t *edict;
	if(!pNet){
		throw "Invalid game rules proxy";
	}
	
	edict = pNet->GetEdict();

	auto res = SMJS_Netprops::SSetNetProp(g_GameRulesProp->gamerules, NULL, propInfo.prop, propInfo.actual_offset, value, f);

	// FIXME: This may actually not send the changes if the prop is a vector or datatable
	// because it's going to get the cached value from the real gamerules and change the gamerules
	// offsets instead of the proxy
	SMJS_Netprops::SSetNetProp(g_GameRulesProp->proxy, edict, propInfo.prop, propInfo.actual_offset, value, f);
	return res;
}