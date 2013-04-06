#pragma once
#include "SMJS_Module.h"

class MSample : public SMJS_Module {
public:
	static void StaticInit();

	MSample(){
		identifier = "console";
	}
	void Init();
};
