/**
	@file		windows/eventimpl.h
	@copyright	Copyright (C) 2009-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares the AJAEventImpl class.
**/

#ifndef AJA_EVENT_IMPL_H
#define AJA_EVENT_IMPL_H

#include "ajastuff/system/system.h"
#include "ajastuff/common/common.h"
#include "ajastuff/system/event.h"


class AJAEventImpl
{
public:

	AJAEventImpl(bool manualReset, const std::string& name);
	virtual ~AJAEventImpl();

	AJAStatus Signal();
	AJAStatus Clear();

	AJAStatus SetState(bool signaled = true);
	AJAStatus GetState(bool* pSignaled);
	
	AJAStatus SetManualReset(bool manualReset);
	AJAStatus GetManualReset(bool* pManualReset);

	AJAStatus WaitForSignal(uint32_t timeout = 0xffffffff);

	virtual AJAStatus GetEventObject(uint64_t* pEventObject);

	HANDLE mEvent;

private:
	bool   mManualReset;
};

#endif	//	AJA_EVENT_IMPL_H
