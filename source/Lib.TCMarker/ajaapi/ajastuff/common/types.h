/**
	@file		types.h
	@copyright	Copyright (C) 2009-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares common types used in the ajastuff library.
**/

#ifndef AJA_TYPES_H
#define AJA_TYPES_H

#if defined(AJA_WINDOWS)

	#if !defined(NULL)
		#define NULL (0)
	#endif

	#define AJA_PAGE_SIZE (4096)

	typedef signed __int8    int8_t;
	typedef signed __int16   int16_t;
	typedef signed __int32   int32_t;
	typedef signed __int64   int64_t;
	typedef unsigned __int8  uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;

	#if defined(_WIN64)
		typedef signed __int64    intptr_t;
		typedef unsigned __int64  uintptr_t;
        #define AJA_OS_64
	#else
		typedef signed __int32    intptr_t;
		typedef unsigned __int32  uintptr_t;
        #define AJA_OS_32
	#endif
    #define AJA_LITTLE_ENDIAN

	// Macros for integer constants
#if !defined(INT8_C)
	#define INT8_C(v)  v##i8
#endif
#if !defined(INT16_C)
	#define INT16_C(v) v##i16
#endif
#if !defined(INT32_C)
	#define INT32_C(v) v##i32
#endif
#if !defined(INT64_C)
	#define INT64_C(v) v##i64
#endif
#if !defined(UINT8_C)
	#define UINT8_C(v)  v##ui8
#endif
#if !defined(UINT16_C)
	#define UINT16_C(v) v##ui16
#endif
#if !defined(UINT32_C)
	#define UINT32_C(v) v##ui32
#endif
#if !defined(UINT64_C)
	#define UINT64_C(v) v##ui64
#endif

	// These macros conform to C99 convention
	#define PRId64 "I64d"
	#define PRIi64 "I64i"
	#define PRIu64 "I64u"

	// Synonyms for library functions
	#define ajasnprintf(a, b, c, d) _snprintf( (a), (b), (c), (d) )
	#define ajavnsprintf(a, b, c, d) vsprintf_s( (a), (b), (c), (d) )
	#define ajastrcasecmp(a, b) _stricmp( (a), (b) )
	#define ajawcscasecmp(a, b) _wcsicmp( (a), (b) )

#endif  // defined(AJA_WINDOWS)

#if defined(AJA_LINUX)

      #if !defined(NULL)
              #define NULL (0)
      #endif

      #define AJA_PAGE_SIZE (4096)

      #if defined(MODULE)
         // We're building the code as a kernel module
         #include <linux/kernel.h>
         #include <linux/string.h>  // memset, etc.
         #include <linux/version.h>
         #if defined(x86_64)
            typedef long int intptr_t;
            // Not sure which version number should be used here
            #if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	           typedef unsigned long int uintptr_t;
            #endif
         #elif !defined(powerpc)
            typedef int intptr_t;
            typedef unsigned int uintptr_t;
         #endif
      #else
         #include <endian.h>
         #include <stdint.h>  // Pull in userland defines

        #ifdef x86_64
           #define AJA_OS_64
        #else
           #define AJA_OS_32
        #endif

        #if __BYTE_ORDER == __LITTLE_ENDIAN
           #define AJA_LITTLE_ENDIAN
        #else
           #define AJA_BIG_ENDIAN
        #endif

        // This adds the ability to format 64-bit entities
        #if CPU_ARCH == x86_64
        # define __PRI64_PREFIX   "l"
        # define __PRIPTR_PREFIX  "l"
        #else
        # define __PRI64_PREFIX   "ll"
        # define __PRIPTR_PREFIX
        #endif

        // Macros for printing format specifiers.
        // Decimal notation.
        #define PRId64	__PRI64_PREFIX "d"
#if defined(powerpc) || defined(CRUSHER)
	// On Crusher scanf %I64i works with a 32-bit int
	// TODO: Change this macro to platform-specific if possible
	#define PRIi64 "lli"
#else
        #define PRIi64	__PRI64_PREFIX "i"
#endif
        #define PRIu64	__PRI64_PREFIX "u"
      #endif  // MODULE

	// Synonyms for library functions with different names on different platforms
	#define ajasnprintf(a, b, c, d) snprintf( (a), (b), (c), (d) )
	#define ajavnsprintf(a, b, c, d) vsnprintf( (a), (b), (c), (d) )
	#define ajastrcasecmp(a, b) strcasecmp( (a), (b) )
	#define ajawcscasecmp(a, b) wcscasecmp( (a), (b) )

#endif  // defined(AJA_LINUX)

// NOTE: 
// In order to get universal binaries compiling need to add the following defines
// to the project that uses this file.
// PER_ARCH_CFLAGS_i386   = -Dx86=1
// PER_ARCH_CFLAGS_x86_64 = -Dx86_64=1
// see: http://developer.apple.com/library/mac/#documentation/Darwin/Conceptual/64bitPorting/building/building.html
#if defined(AJA_MAC)

	#define AJA_PAGE_SIZE (4096)
	#define AJA_LITTLE_ENDIAN

	#include <stdint.h>
    #ifdef __LP64__
	    #define AJA_OS_64
	#else
		#define AJA_OS_32
	#endif

	// This adds the ability to format 64-bit entities
    #ifdef x86_64
	# define __PRI64_PREFIX   "l"
	# define __PRIPTR_PREFIX  "l"
	#else
	# define __PRI64_PREFIX   "ll"
	# define __PRIPTR_PREFIX
	#endif

	// Macros for printing format specifiers.
	// Decimal notation.
    //#define PRId64	__PRI64_PREFIX "d"
    //#define PRIi64	__PRI64_PREFIX "i"
    //#define PRIu64	__PRI64_PREFIX "u"

	// Synonyms for library functions
	#define ajasnprintf(a, b, c, d) snprintf( (a), (b), (c), (d) )
	#define ajavnsprintf(a, b, c, d) vsnprintf( (a), (b), (c), (d) )
	#define ajastrcasecmp(a, b) strcasecmp( (a), (b) )
	#define ajawcscasecmp(a, b) wcscasecmp( (a), (b) )

	#ifndef EXCLUDE_WCHAR
		#include <wchar.h>

		inline wchar_t wcstoupper(wchar_t c)
		{
			// probably needs to be more robust
			wchar_t newChar = c;
			
			if (c >= (wchar_t)L'a' && c <= (wchar_t)L'z')
				newChar = c - (L'a' - L'A');
			
			return newChar;
		}

		inline int wcscasecmp(const wchar_t *string1,const wchar_t *string2)
		{
			int retVal = 0;
				
			int str1Len = (int)wcslen(string1);
			int str2Len = (int)wcslen(string2);
			
			if(str1Len > str2Len)
				retVal = 1;
			else if(str1Len < str2Len)
				retVal = -1;
			else //same length
			{
				bool match = true;
				for(int i=0;i<str1Len;i++)
				{
					if(wcstoupper(string1[i]) != wcstoupper(string2[i]))
					{
						match = false;
						break;
					}
				}
				
				if(!match)
					retVal = -1; //or could be 1 not sure which is best
			}
			return retVal;
		}
	#endif

#endif  // defined(AJA_MAC)

#if !defined(NULL_PTR)
	#define NULL_PTR (0)
#endif


/** 
 *	Macro to define a FourCC. Example: AJA_FCC("dvc ")
 */
#if defined(AJA_LITTLE_ENDIAN)
#define AJA_FCC(a) \
( ((uint32_t)(((uint8_t *)(a))[3]) << 0)  + \
  ((uint32_t)(((uint8_t *)(a))[2]) << 8)  + \
  ((uint32_t)(((uint8_t *)(a))[1]) << 16) + \
  ((uint32_t)(((uint8_t *)(a))[0]) << 24) )
#else
#define AJA_FCC(a) \
( ((uint32_t)(((uint8_t *)(a))[0]) << 0)  + \
  ((uint32_t)(((uint8_t *)(a))[1]) << 8)  + \
  ((uint32_t)(((uint8_t *)(a))[2]) << 16) + \
  ((uint32_t)(((uint8_t *)(a))[3]) << 24) )
#endif

/** 
 *	Macro to define a FourCC. Example: AJA_FOURCC('d','v','c',' ')
 *                                     AJA_FOURCC_2(0x24)
 */
#if defined(AJA_LITTLE_ENDIAN)
#define AJA_FOURCC(a,b,c,d)  \
( (((uint32_t)(a)) << 24)  + \
  (((uint32_t)(b)) << 16)  + \
  (((uint32_t)(c)) <<  8)  + \
  (((uint32_t)(d)) <<  0) )
#else
#define AJA_FOURCC(a,b,c,d)  \
( (((uint32_t)(a)) <<  0)  + \
  (((uint32_t)(b)) <<  8)  + \
  (((uint32_t)(c)) << 16)  + \
  (((uint32_t)(d)) << 24) )
#endif
#define AJA_FOURCC_2(a) (AJA_FOURCC(a,0,0,0))


/** 
 *	Standard api return code
 */

#define AJA_SUCCESS(_status_) (_status_ > 0)
#define AJA_FAILURE(_status_) (_status_ <= 0)

/**
	@defgroup	AJAGroupStatus	AJA_STATUS
	Defines return code status.  Successful return codes are greater than 0.
**/
///@{
typedef enum
{
	AJA_STATUS_SUCCESS 			=2,		/**< Function succeeded */
	AJA_STATUS_TRUE				=1,		/**< Result was true */
	AJA_STATUS_FALSE			=0,		/**< Result was false */
	AJA_STATUS_UNKNOWN			=-1,	/**< Unknown status */
	AJA_STATUS_FAIL				=-2,	/**< Function failed */
	AJA_STATUS_TIMEOUT			=-3,	/**< A wait timed out */
	AJA_STATUS_RANGE			=-4,	/**< A parameter was out of range */
	AJA_STATUS_INITIALIZE		=-5,	/**< The object has not been initialized */
	AJA_STATUS_NULL				=-6,	/**< A pointer parameter was NULL */
	AJA_STATUS_OPEN				=-7,	/**< Something failed to open */
	AJA_STATUS_IO				=-8,	/**< An IO operation failed */
	AJA_STATUS_DISABLED			=-9,	/**< Device is disabled */
	AJA_STATUS_BUSY				=-10,	/**< Device is busy */
	AJA_STATUS_BAD_PARAM		=-11,	/**< Bad parameter value */
	AJA_STATUS_FEATURE			=-12,	/**< A required device feature does not exist */
	AJA_STATUS_UNSUPPORTED		=-13,	/**< Parameters are valid but not supported */
	AJA_STATUS_READONLY			=-14,	/**< Write is not supported */
	AJA_STATUS_WRITEONLY		=-15,	/**< Read is not supported */
	AJA_STATUS_MEMORY			=-16,	/**< Memory allocation failed */
	AJA_STATUS_ALIGN			=-17,	/**< Parameter not properly aligned */
	AJA_STATUS_FLUSH			=-18,	/**< Something has been flushed */	
	AJA_STATUS_NOINPUT			=-19,	/**< No input detected */
	AJA_STATUS_SURPRISE_REMOVAL	=-20,	/**< Hardware communication failed */

// Sequence errors

	AJA_STATUS_NOBUFFER			=-100,	/**< No free buffers, all are scheduled or in use */
	AJA_STATUS_INVALID_TIME		=-101,	/**< Invalid schedule time */
	AJA_STATUS_NOSTREAM			=-102,	/**< No stream found */
	AJA_STATUS_TIMEEXPIRED		=-103,	/**< Scheduled time is too late */
	AJA_STATUS_BADBUFFERCOUNT	=-104,	/**< Buffer count out of bounds */
	AJA_STATUS_BADBUFFERSIZE	=-105,	/**< Buffer size out of bounds */
	AJA_STATUS_STREAMCONFLICT	=-106,	/**< Another stream is using resources */
	AJA_STATUS_NOTINITIALIZED	=-107,	/**< Streams not initialized */
	AJA_STATUS_STREAMRUNNING	=-108	/**< Streams is running, should be stopped */
} AJAStatus;
///@}

#ifndef NUMELMS
   #define NUMELMS(aa) (sizeof(aa)/sizeof((aa)[0]))
#endif

#define AJA_ENDIAN_SWAP16(_data_) (((_data_<<8)&0xff00)|((_data_>>8)&0x00ff))
#define AJA_ENDIAN_SWAP32(_data_) (((_data_<<24)&0xff000000)|((_data_<<8)&0x00ff0000)|((_data_>>8)&0x0000ff00)|((_data_>>24)&0x000000ff))
#define AJA_ENDIAN_SWAP64(_data_) (\
	((_data_<<56)&0xff00000000000000)| \
	((_data_<<40)&0x00ff000000000000)| \
	((_data_<<24)&0x0000ff0000000000)| \
	((_data_<< 8)&0x000000ff00000000)| \
	((_data_>> 8)&0x00000000ff000000)| \
	((_data_>>24)&0x0000000000ff0000)| \
	((_data_>>40)&0x000000000000ff00)| \
	((_data_>>56)&0x00000000000000ff))

#endif	//	AJA_TYPES_H
