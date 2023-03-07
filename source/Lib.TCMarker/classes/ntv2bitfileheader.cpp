/**
	@file		bitfileheader.cpp
	@deprecated	Use CNTV2Bitfile instead.
	@copyright	(C) 2011 AJA Video Systems, Inc.  Proprietary and Confidential information.  All rights reserved.
**/
#if !defined (NTV2_DEPRECATE)

#include "ntv2bitfileheader.h"
#include "ntv2endian.h"
#if defined(AJA_LINUX)
	#include <string.h>
#endif

// first 13 bytes of a bit file
static char head13[]		= { 0x00, 0x09, 0x0f, 0xf0, 
								0x0f, 0xf0, 0x0f, 0xf0, 
								0x0f, 0xf0, 0x00, 0x00, 
								0x01 };

static char syncwordV1[]	= { 0xff, 0xff, 0xff, 0xff, 
								0xaa, 0x99, 0x55, 0x66 };

static char syncwordV2[]	= { 0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xaa, 0x99, 0x55, 0x66 };
								
static char syncwordV3[]	= { 0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0x00, 0x00, 0x00, 0xbb,
								0x11, 0x22, 0x00, 0x44,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xaa, 0x99, 0x55, 0x66 };
								
static char syncwordV4[]	= { 0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff,
								0xaa, 0x99, 0x55, 0x66 };


// Default Constructor
BitfileHeader::BitfileHeader()
{
	m_Length   = 0;
	m_HeaderOK = false;
}


// Constructor
BitfileHeader::BitfileHeader (void *pData)
{
	BitfileHeader();
	parse(pData);
}


// Destructor
BitfileHeader::~BitfileHeader()
{
}


// parse header - return 'true' if header looks OK
bool BitfileHeader::parse(void *pData)
{
	m_HeaderOK = false;
	uint16_t fieldLen;
	char *p = (char *)pData;
	
	do
	{
		// make sure 1st 13 bytes are what we expect
		if ( strncmp(p, head13, 13) != 0 )
			break;
			
		p += 13;												// skip over header header
			
		// the next byte should be 'a'
		if (*p++ != 'a')
			break;
			
		fieldLen = NTV2EndianSwap16BtoH(*((uint16_t *)p));		// the next 2 bytes are the length of the FileName (including /0)
		p += 2;													// now pointing at the beginning of the file name
		m_FileName = p;											// grab file name
		p += fieldLen;											// skip over file name - now pointing to beginning of 'b' field
		
		// the next byte should be 'b'
		if (*p++ != 'b')
			break;
			
		fieldLen = NTV2EndianSwap16BtoH(*((uint16_t *)p));		// the next 2 bytes are the length of the Part Name (including /0)
		p += 2;													// now pointing at the beginning of the part name
		m_Part = p;												// grab file name
		p += fieldLen;											// skip over part name - now pointing to beginning of 'c' field
		
		// the next byte should be 'c'
		if (*p++ != 'c')
			break;
			
		fieldLen = NTV2EndianSwap16BtoH(*((uint16_t *)p));		// the next 2 bytes are the length of the date string (including /0)
		p += 2;													// now pointing at the beginning of the date string
		m_Date = p;												// grab date string
		p += fieldLen;											// skip over date string - now pointing to beginning of 'd' field
		
		// the next byte should be 'd'
		if (*p++ != 'd')
			break;
			
		fieldLen = NTV2EndianSwap16BtoH(*((uint16_t *)p));		// the next 2 bytes are the length of the time string (including /0)
		p += 2;													// now pointing at the beginning of the time string
		m_Time = p;												// grab time string
		p += fieldLen;											// skip over time string - now pointing to beginning of 'e' field
		
		// the next byte should be 'e'
		if (*p++ != 'e')
			break;
			
		m_Length = NTV2EndianSwap16BtoH(*((uint32_t *)p));		// the next 4 bytes are the length of the raw program data
		p += 4;													// now pointing at the beginning of the identifier
		
		// make sure the sync word is what we expect - must by V1 or V2 style sync word
		if ( strncmp(p, syncwordV1, 8) != 0 )
		{
			// another chance - is this V2 style
			if ( strncmp(p, syncwordV2, 36) != 0 )
			{
				// one more try this might be new LHi/IoExpress flash 
				if ( strncmp(p, syncwordV3, 52) != 0 )
				{
					if ( strncmp(p, syncwordV4, 20) != 0 )
						break;
				}
			}
		}
		
		// if we made it this far it must be an OK Header - and it is parsed
		m_HeaderOK = true;
		
	} while(false);
	
	return m_HeaderOK;
}

#endif	//	!defined (NTV2_DEPRECATE)
