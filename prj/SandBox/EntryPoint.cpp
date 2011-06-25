/**
 *  @file   EntryPoint.cpp
 *  @author Victor Soria
 */

//- Headers -
#include <stdio.h>
#include "Channel.h"
#include "TMsgMierder.h"
#include "ClassC.h"

class A
{
public:
	void OnMierder(const TMsgMierder* aMsg)
	{
		printf("mierder %d\n", aMsg->mValue);
	}
};

class B
{
public:
	void OnCaca(const TMsgMierder* aMsg)
	{
		printf("caca %d\n", aMsg->mValue*2);
	}
};


int main(int argc, char* argv[])
{
	Channel lChannel;
	A lA;
	B lB;
	ClassC lC;
	lChannel.RegisterCallback(&lA, &A::OnMierder);
	lChannel.RegisterCallback(&lB, &B::OnCaca);
	{
		lChannel.RegisterCallback(&lC, &ClassC::OnTopotaMadre);
		NotifyOnDestroy<ClassC> lNotifier(&lChannel, &lC);
		TMsgMierder(&lChannel, 1);	// Se manda un mensaje como si fuera una llamada a funcion, solo que en realidad puede que se esten llamando a varias funciones
	}
	lChannel.RemoveCallbacks(&lA);

	TMsgMierder lMierder;
	lMierder.mValue = 2;
	lChannel.Call(&lMierder);	// Manera menos guay de hacer la llamada

	return 1;
}