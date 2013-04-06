#include "SMJS.h"

v8::Isolate *mainIsolate;
void SMJS_Init(){
	mainIsolate = v8::Isolate::GetCurrent();
}