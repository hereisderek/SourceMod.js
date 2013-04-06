#ifndef _INCLUDE_SMJSI_H_
#define _INCLUDE_SMJSI_H_

#include "ISMJS_config.h"
#include "SMJS_Plugin.h"
#include "SMJS_BaseWrapped.h"
#include <IShareSys.h>

#define SMINTERFACE_SMJS_NAME		"ISMJS"
#define SMINTERFACE_SMJS_VERSION	2

typedef void (*SMJS_Plugin_Created)(SMJS_Plugin*);

class ISMJS : public SMInterface{
public:
	virtual const char *GetInterfaceName() = 0;
	virtual unsigned int GetInterfaceVersion() = 0;
public:

	virtual void AddPluginCreatedCallback(SMJS_Plugin_Created callback);
	virtual void RemovePluginCreatedCallback(SMJS_Plugin_Created callback);
	virtual SMJS_Plugin* GetPluginRunning();
	virtual SMJS_Plugin* GetPlugin(PLUGIN_ID id);
	virtual int GetNumPlugins();
	virtual SMJS_BaseWrapped* GetEntityWrapper(CBaseEntity *ent);


};

#endif
