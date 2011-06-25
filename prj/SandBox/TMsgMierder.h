#pragma once

#include <pragma/utils/Channel.h>

struct TMsgDirectCall
{
	TMsgDirectCall() { }
	TMsgDirectCall( Channel* ) { }
};

struct TMsgMierder : public TMsgDirectCall
{
	TMsgMierder() { }
	TMsgMierder(Channel* aChannel, int aValue) : TMsgDirectCall(aChannel), mValue(aValue) { aChannel->Call(this); }
	int mValue;
};

