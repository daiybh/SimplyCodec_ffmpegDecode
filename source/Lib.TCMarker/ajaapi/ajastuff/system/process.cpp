/**
	@file		process.cpp
	@copyright	Copyright (C) 2011-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Implements the AJAProcess class.
**/

// include the system dependent implementation class

#if defined(AJA_WINDOWS)
	#include "ajastuff/system/windows/processimpl.h"
#endif

#if defined(AJA_MAC)
	#include "ajastuff/system/mac/processimpl.h"
#endif

#if defined(AJA_LINUX)
	#include "ajastuff/system/linux/processimpl.h"
#endif

#include "process.h"

AJAProcess::AJAProcess() : mpImpl(NULL)
{
	mpImpl = new AJAProcessImpl();
}

AJAProcess::~AJAProcess()
{
	if(mpImpl)
		delete mpImpl;
	mpImpl = NULL;
}

uint64_t
AJAProcess::GetPid()
{
	return AJAProcessImpl::GetPid();
}

bool
AJAProcess::IsValid(uint64_t pid)
{
	return AJAProcessImpl::IsValid(pid);
}

bool
AJAProcess::Activate(uint64_t handle)
{
	return AJAProcessImpl::Activate(handle);
}

bool
AJAProcess::Activate(const char* pWindow)
{
	return AJAProcessImpl::Activate(pWindow);
}
