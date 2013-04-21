#include "MPlugin.h"
#include "SMJS_Plugin.h"
#include "extension.h"

WRAPPED_CLS_CPP(MPlugin, SMJS_Module);

MPlugin::MPlugin(){
	identifier = "plugin";
}

void MPlugin::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	
}


FUNCTION_M(MPlugin::isSandboxed)
	return v8::Boolean::New(GetPluginRunning()->isSandboxed);
END

	
FUNCTION_M(MPlugin::loadPlugin)
	if(GetPluginRunning()->isSandboxed) THROW("This function is not allowed to be called in sandboxed plugins");
	
	PSTR(dir);
	return v8::Boolean::New(LoadPlugin(*dir) != NULL);
END


FUNCTION_M(MPlugin::exists)
	PSTR(dir);
	return v8::Boolean::New(SMJS_Plugin::GetPluginByDir(*dir) != NULL);
END