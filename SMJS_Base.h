#ifndef _INCLUDE_SMJS_BASE_H_
#define _INCLUDE_SMJS_BASE_H_

class SMJS_Base {
private:
	virtual void Nothing(){}
};

#define WRAPPED_FUNC(name) proto->Set(v8::String::New(#name), v8::FunctionTemplate::New(name));

#endif