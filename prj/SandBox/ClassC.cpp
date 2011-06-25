#include "ClassC.h"
#include "TMsgMierder.h"
#include <stdio.h>

void ClassC::OnTopotaMadre(const TMsgMierder* aMsg)
{
	printf("TopotaMadre %d\n", aMsg->mValue*10);	
}