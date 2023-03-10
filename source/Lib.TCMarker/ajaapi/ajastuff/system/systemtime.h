/**
	@file		systemtime.h
	@copyright	Copyright (C) 2009-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares the AJATime class.
**/

#ifndef AJA_TIME_H
#define AJA_TIME_H

#include "ajastuff/common/public.h"

/** 
	@brief		Collection of platform-independent host system clock time functions.
	@ingroup	AJAGroupSystem
**/
class AJA_EXPORT AJATime
{
public:

	/**
		@brief		Returns the current value of the host system's low-resolution clock, in milliseconds.
		@return		The current value of the host system's low-resolution clock, in milliseconds.
	**/
	static int64_t	GetSystemTime (void);

	/**
		@brief		Returns the current value of the host system's high-resolution time counter.
		@return		The current value of the host system's high-resolution time counter.
	**/
	static int64_t	GetSystemCounter (void);

	/**
		@brief		Returns the frequency of the host system's high-resolution time counter.
		@return		The high resolution counter frequency.
	**/
	static int64_t	GetSystemFrequency (void);

	/**
		@brief		Returns the current value of the host's high-resolution clock, in milliseconds.
		@return		Current value of the host's clock, in milliseconds, based on GetSystemCounter() and GetSystemFrequency().
	**/
	static uint64_t	GetSystemMilliseconds (void);

	/**
		@brief		Returns the current value of the host's high-resolution clock, in microseconds.
		@return		Current value of the host's clock, in microseconds, based on GetSystemCounter() and GetSystemFrequency().
	**/
	static uint64_t	GetSystemMicroseconds (void);

	/**
		@brief		Suspends execution of the current thread for a given number of milliseconds.
		@param		inMilliseconds		Specifies the sleep time, in milliseconds.
	**/
	static void		Sleep (const int32_t inMilliseconds);

	/**
		@brief		Suspends execution of the current thread for a given number of microseconds.
		@param		inMicroseconds		Time to sleep (microseconds).
	**/
	static void		SleepInMicroseconds (const int32_t inMicroseconds);

};	//	AJATime

#endif	//	AJA_TIME_H
