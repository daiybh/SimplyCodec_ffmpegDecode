/**
	@file		bitfileheader.h
	@deprecated	Use CNTV2Bitfile instead.
	@copyright	(C) 2011 AJA Video Systems, Inc.  Proprietary and Confidential information.  All rights reserved.
**/

#ifndef BITFILEHEADER_H
#define BITFILEHEADER_H

#if !defined (NTV2_DEPRECATE)

#include "ajatypes.h"
#include <string>
#if defined(AJA_LINUX)
	#include <stdint.h>
#endif

class BitfileHeader
{
public:
	BitfileHeader();
	BitfileHeader(void *pData);
	virtual ~BitfileHeader();

	bool  parse(void *pData);
	bool  headerOK(void)				{ return m_HeaderOK; }
	
	const char *getFileNameString(void) { return m_FileName.c_str(); }
	const char *getPartString(void)		{ return m_Part.c_str();     }
	const char *getDateString(void)		{ return m_Date.c_str();     }
	const char *getTimeString(void)		{ return m_Time.c_str();     }
	
	int32_t getLength(void)				{ return m_Length; }

protected:    
	std::string		m_FileName;
	std::string		m_Part;
	std::string		m_Date;
	std::string		m_Time;
	int32_t			m_Length;
	bool			m_HeaderOK;
};

#endif	//	!defined (NTV2_DEPRECATE)

#endif	// BITFILEHEADER_H
