#pragma once
#include "SMJS_Module.h"
#include "SMJS_GameRules.h"
#include "filesystem.h"

class MEngine : public SMJS_Module {
public:
	MEngine();

	void Init();

	//FUNCTION_DECL(resume);

	WRAPPED_CLS(MEngine, SMJS_Module) {
		temp->SetClassName(v8::String::New("EngineModule"));
		
		//WRAPPED_FUNC(resume);
	}

private:
	static FileHandle_t FSOpen(const char *pFileName, const char *pOptions, const char *pathID = NULL);
	static bool FSReadFile(const char *pFileName, const char *pPath, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL);
	static int FSReadFileEx(const char *pFileName, const char *pPath, void **ppBuf, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL);
	static FileHandle_t FSOpenEx(const char *pFileName, const char *pOptions, unsigned flags = 0, const char *pathID = 0, char **ppszResolvedFilename = NULL);
};
