#include "SMJS.h"
#include "extension.h"

v8::Isolate *mainIsolate;


time_t lastPing;
SourceMod::IMutex *pingMutex;

class SMJS_CheckerThread : public SourceMod::IThread{
	void RunThread(IThreadHandle *pHandle){
		while(1){
			pingMutex->Lock();

#ifndef _DEBUG
			int now = time(NULL);
			if(now - lastPing > 30){
				v8::V8::TerminateExecution(mainIsolate);
				printf("SERVER STOPPED THINKING, QUITTING\n");
				threader->ThreadSleep(3000);
				throw "Script timed out";
			}
#endif

			pingMutex->Unlock();
			threader->ThreadSleep(1000);
		}
	}

	void OnTerminate(IThreadHandle *pHandle, bool cancel){

	}
};

void SMJS_Init(){
	mainIsolate = v8::Isolate::GetCurrent();

	char smjsPath[512];
	smutils->BuildPath(Path_SM, smjsPath, sizeof(smjsPath), "sourcemod.js");

	pingMutex = threader->MakeMutex();
	SMJS_Ping();
	threader->MakeThread(new SMJS_CheckerThread());
}

void SMJS_Ping(){
	pingMutex->Lock();
	lastPing = time(NULL);
	pingMutex->Unlock();
}

