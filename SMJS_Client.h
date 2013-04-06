#ifndef _INCLUDE_SMJS_CLIENT_H_
#define _INCLUDE_SMJS_CLIENT_H_

#include "SMJS.h"
#include "SMJS_BaseWrapped.h"
#include "SMJS_Entity.h"

class SMJS_Client : public SMJS_Entity {
public:
	bool inGame;
	edict_t *edict;
	int authStage;

	SMJS_Client(edict_t *edict);
	void OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper);
	void ReattachEntity();


	FUNCTION_DECL(printToChat);
	FUNCTION_DECL(printToConsole);
	FUNCTION_DECL(isInGame);
	FUNCTION_DECL(fakeCommand);
	FUNCTION_DECL(isFake);
	FUNCTION_DECL(isReplay);
	FUNCTION_DECL(isSourceTV);
	FUNCTION_DECL(getAuthString);
	FUNCTION_DECL(kick);

	WRAPPED_CLS(SMJS_Client, SMJS_Entity) {
		temp->SetClassName(v8::String::New("Client"));
		WRAPPED_FUNC(printToChat);
		WRAPPED_FUNC(printToConsole);
		WRAPPED_FUNC(isInGame);
		WRAPPED_FUNC(fakeCommand);
		WRAPPED_FUNC(isFake);
		WRAPPED_FUNC(isReplay);
		WRAPPED_FUNC(isSourceTV);
		WRAPPED_FUNC(getAuthString);
		WRAPPED_FUNC(kick);

	}

private:
	SMJS_Client();
};

#endif
