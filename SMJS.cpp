#include "SMJS.h"
#include "extension.h"

v8::Isolate *mainIsolate;

void SMJS_Init(){
	mainIsolate = v8::Isolate::GetCurrent();

	char smjsPath[512];
	smutils->BuildPath(Path_SM, smjsPath, sizeof(smjsPath), "sourcemod.js");
}