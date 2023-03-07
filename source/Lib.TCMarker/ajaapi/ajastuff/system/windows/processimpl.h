/**
	@file		windows/processimpl.h
	@copyright	Copyright (C) 2009-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares the AJAProcessImpl class.
**/

#ifndef AJA_PROCESS_IMPL_H
#define AJA_PROCESS_IMPL_H

#include "ajastuff/system/system.h"
#include "ajastuff/common/common.h"
#include "ajastuff/system/process.h"


class AJAProcessImpl
{
public:

						AJAProcessImpl();
virtual					~AJAProcessImpl();

static		uint64_t	GetPid();
static		bool		IsValid(uint64_t pid);

static		bool		Activate(const char* pWindow);
static		bool		Activate(uint64_t handle);
};


#endif	//	AJA_PROCESS_IMPL_H
