#ifndef _INCLUDE_SMJSI_CFG_H_
#define _INCLUDE_SMJSI_CFG_H_

__pragma(warning(disable:4005) )
#include <stdint.h>
#define HAVE_STDINT_
#define HAVE_STDINT_H
#include "platform.h"
#define V8STDINT_H_
#include "v8.h"
#include "k7v8macros.h"

#include <IShareSys.h>

__pragma(warning(default:4005) )

typedef int PLUGIN_ID;

class SMJS_Plugin;
class SMJS_BaseWrapped;

#endif
