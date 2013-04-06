#include "SMJS_Client.h"
#include "SMJS_Plugin.h"

WRAPPED_CLS_CPP(SMJS_Client, SMJS_Entity)

SMJS_Client::SMJS_Client(edict_t *edict) : SMJS_Entity(NULL) {
	this->edict = edict;
	inGame = false;
	authStage = 0;
	
	entIndex = gamehelpers->IndexOfEdict(edict);
}

void SMJS_Client::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	SMJS_Entity::OnWrapperAttached(plugin, wrapper);
}

void SMJS_Client::ReattachEntity(){
	auto tmp = edict->GetNetworkable();
	if(tmp != NULL){
		this->ent = tmp->GetBaseEntity();
		this->valid = true;
	}
}

FUNCTION_M(SMJS_Client::printToChat)
	GET_INTERNAL(SMJS_Client*, self);
	PSTR(str);

	gamehelpers->TextMsg(self->entIndex, TEXTMSG_DEST_CHAT, *str);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Client::printToConsole)
	GET_INTERNAL(SMJS_Client*, self);
	PSTR(str);

	gamehelpers->TextMsg(self->entIndex, TEXTMSG_DEST_CONSOLE, *str);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Client::isInGame)
	GET_INTERNAL(SMJS_Client*, self);
	return v8::Boolean::New(self->inGame);
END

FUNCTION_M(SMJS_Client::isFake)
	GET_INTERNAL(SMJS_Client*, self);
	return v8::Boolean::New(playerhelpers->GetGamePlayer(self->edict)->IsFakeClient());
END

FUNCTION_M(SMJS_Client::isReplay)
	GET_INTERNAL(SMJS_Client*, self);
	return v8::Boolean::New(playerhelpers->GetGamePlayer(self->edict)->IsReplay());
END

FUNCTION_M(SMJS_Client::isSourceTV)
	GET_INTERNAL(SMJS_Client*, self);
	return v8::Boolean::New(playerhelpers->GetGamePlayer(self->edict)->IsSourceTV());
END

FUNCTION_M(SMJS_Client::fakeCommand)
	GET_INTERNAL(SMJS_Client*, self);
	PSTR(str);
	engine->ClientCommand(self->edict, *str);
	RETURN_UNDEF;
END

FUNCTION_M(SMJS_Client::getAuthString)
	GET_INTERNAL(SMJS_Client*, self);
	return v8::String::New(engine->GetPlayerNetworkIDString(self->edict));
END

FUNCTION_M(SMJS_Client::kick)
	GET_INTERNAL(SMJS_Client*, self);
	PSTR(str);
	playerhelpers->GetGamePlayer(self->edict)->Kick(*str);
	RETURN_UNDEF;
END