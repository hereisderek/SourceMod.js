
#ifndef _INCLUDE_SMJS_H_
#define _INCLUDE_SMJS_H_

__pragma(warning(disable:4005) )
#include <stdint.h>
#define HAVE_STDINT_
#define HAVE_STDINT_H
#include "platform.h"
#define V8STDINT_H_
#include "v8.h"
#include "k7v8macros.h"

__pragma(warning(default:4005) )

//#include "sp_file_headers.h"

#define SMJS_API_VERSION 1

typedef int PLUGIN_ID;

class SMJS_Plugin;
class SMJS_BaseWrapped;
class SMJS_Module;

class IPluginDestroyedHandler {
public:
	virtual void OnPluginDestroyed(SMJS_Plugin *plugin) = 0;
};

void SMJS_Init();
void SMJS_Ping();

extern v8::Isolate *mainIsolate;


#endif