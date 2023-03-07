/**
	@file		pnp/pnp.cpp
	@copyright	Copyright (C) 2011-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Implements the AJAPnp (plug-and-play) class.
**/

#include <assert.h>
#include "ajastuff/pnp/pnp.h"

#if defined(AJA_WINDOWS)
	#include "ajastuff/pnp/windows/pnpimpl.h"
#endif

#if defined(AJA_LINUX)
	#include <ajastuff/pnp/linux/pnpimpl.h>
#endif

#if defined(AJA_MAC)
	#include "ajastuff/pnp/mac/pnpimpl.h"
#endif



AJAPnp::AJAPnp()
{
	mpImpl = new AJAPnpImpl();
}


AJAPnp::~AJAPnp()
{
	delete mpImpl;
}


AJAStatus 
AJAPnp::Install(AJAPnpCallback callback, void* refCon, uint32_t devices)
{
	return mpImpl->Install(callback, refCon, devices);
}


AJAStatus
AJAPnp::Uninstall()
{
	return mpImpl->Uninstall();
}


AJAPnpCallback 
AJAPnp::GetCallback()
{
	return mpImpl->GetCallback();
}


void* 
AJAPnp::GetRefCon()
{
	return mpImpl->GetRefCon();
}


uint32_t
AJAPnp::GetPnpDevices()
{
	return mpImpl->GetPnpDevices();
}


AJAPnp::AJAPnp (const AJAPnp & inObjToCopy)
{
	(void) inObjToCopy;	assert (false && "hidden copy constructor");	//	mpImpl=inObjToCopy.mpImpl;
}


AJAPnp &
AJAPnp::operator= (const AJAPnp & inObjToCopy)
{
	if (&inObjToCopy != this)
		assert (false && "hidden assignment operator");	//	mpImpl = inObjToCopy.mpImpl;

	return *this;

}	//	copy constructor
