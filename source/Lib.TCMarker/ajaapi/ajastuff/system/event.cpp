/**
	@file		event.cpp
	@copyright	Copyright (C) 2009-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Implements the AJAEvent class.
**/

// include the system dependent implementation class
#if defined(AJA_WINDOWS)
	#include "ajastuff/system/windows/eventimpl.h"
#endif
#if defined(AJA_LINUX)
	#include <ajastuff/system/linux/eventimpl.h>
#endif

#if defined(AJA_MAC)
	#include "ajastuff/system/mac/eventimpl.h"
#endif

using std::string;

AJAEvent::AJAEvent(bool manualReset, const string& name) :
	mpImpl(NULL)
{
	mpImpl = new AJAEventImpl(manualReset, name);
}


AJAEvent::~AJAEvent()
{
	if( mpImpl )
	{
		delete mpImpl;
		mpImpl = NULL;
	}
}


// interface to the implementation class

AJAStatus 
AJAEvent::Signal()
{
	return mpImpl->Signal();
}


AJAStatus
AJAEvent::Clear()
{
	return mpImpl->Clear();
}


AJAStatus 
AJAEvent::SetState(bool signaled)
{
	return mpImpl->SetState(signaled);
}


AJAStatus
AJAEvent::GetState(bool* pSignaled)
{
	return mpImpl->GetState(pSignaled);
}


AJAStatus 
AJAEvent::SetManualReset(bool manualReset)
{
	return mpImpl->SetManualReset(manualReset);
}


AJAStatus
AJAEvent::GetManualReset(bool* pManualReset)
{
	return mpImpl->GetManualReset(pManualReset);
}


AJAStatus 
AJAEvent::WaitForSignal(uint32_t timeout)
{
	return mpImpl->WaitForSignal(timeout);
}


AJAStatus 
AJAEvent::GetEventObject(uint64_t* pEventObject)
{
	return mpImpl->GetEventObject(pEventObject);
}
