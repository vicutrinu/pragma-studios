#pragma once

#include <map>
#include <vector>

template <typename T>
void* GetTypeID()
{
	static int sObjectType;
	return (void*)&sObjectType;		
}

class Channel
{
public:
	
	struct functor_base
	{
		template<typename T>
		functor_base(T* aObject) : mObject((void*)aObject) { }
		void* mObject;
	};
	
	template<typename T, typename TMsg>
	void RegisterCallback( T* aObject, void(T::*aMethod)(const TMsg* ) )
	{
		struct functor : public functor_base
		{
			typedef void(T::*FunctionPtr)(const TMsg* );
			functor(T* aObject, FunctionPtr aMethod )
			: functor_base(aObject), mMethod(aMethod) { }
			FunctionPtr mMethod;

			inline void DoCall(const TMsg* aMsg) { ((T*)mObject->*mMethod)(aMsg); }
			
			static inline void Call(void* aObj, const TMsg* aMsg)
			{
				((functor*)aObj)->DoCall(aMsg);
			}
			
		};
		void* lFun = (void*)&functor::Call;
		functor* lFunctorObject = (functor*)malloc(sizeof(functor));
		new (lFunctorObject) functor(aObject, aMethod);
		mMap[GetTypeID<TMsg>()].push_back( std::pair<void*,void*>(lFun, (void*)lFunctorObject) );
	}
	
	template<typename T>
	void RemoveCallbacks( T* aObject )
	{
		for ( Flipada::iterator lIter = mMap.begin()
		    ; lIter != mMap.end()
			; ++lIter )
		{
			std::vector< std::pair<void*,void* > >& lVector = lIter->second;
			for(size_t i = 0; i < lVector.size(); )
			{
				if( ((functor_base*)(lVector[i].second))->mObject == (void*)aObject )
				{
					free( lVector[i].second );
					lVector.erase( lVector.begin() + i);
				}
				else
					i++;
			}
		}
	}

	template<typename TMsg>
	void Call(const TMsg* aMessage)
	{
		Flipada::iterator lIter = mMap.find(GetTypeID<TMsg>());
		if(lIter != mMap.end())
		{
			std::vector< std::pair<void*,void* > >& lVector = lIter->second;
			for(size_t i = 0; i < lVector.size(); i++)
			{
				void(*Function)(void* aObj, const TMsg* aMsg) = reinterpret_cast<void(*)(void* , const TMsg* )>( lVector[i].first );
				Function(lVector[i].second, aMessage);
			}
		}
	}

	typedef std::map< const void*, std::vector< std::pair<void*,void* > > > Flipada;

	Flipada mMap;

};

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
