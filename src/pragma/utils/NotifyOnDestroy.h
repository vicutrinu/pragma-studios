/**
 *  @file   NotifyOnDestroy.h
 *  @author Victor Soria
 */


#include <pragma/utils/Channel.h>

template <typename T>
class NotifyOnDestroy
{
public:
	NotifyOnDestroy( Channel* aChannel, const T* aObject)
	: mChannel(aChannel)
	, mObject(aObject)
	{ }
	
	~NotifyOnDestroy()
	{
		mChannel->RemoveCallbacks(mObject);
	}
	
private:
	Channel* mChannel;
	const T* mObject;
};