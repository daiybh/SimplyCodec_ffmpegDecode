/*****************************************************************************************
* 
*	NTV2DaemonAppleEvents.h
*
*	NTV2Daemon Apple Event constansts
*
*	by Andy Peterman
*
*	Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
*	All rights reserved.
*
*****************************************************************************************/

#ifndef _NTV2DaemonAppleEvents_H_
#define _NTV2DaemonAppleEvents_H_

#include <ApplicationServices/ApplicationServices.h>

#define kNTV2DaemonPIDFile			"/tmp/.ntv2Daemon.pid"

const AEEventClass	kNTV2Suite		= 'NTV2';	// Custom events for NTV2 Daemon

// NTV2 Event IDs

const AEEventID		kPrefDesktopID	= 'desk';	// Desktop display preference
const AEEventID		kPrefRegMaskID	= 'regp';	// Register preference with mask
const AEEventID		kPrefRemoveID	= 'remv';	// Remove preference entry

// NTV2 descriptor keys for parameter options

const AEKeyword		kKeyBoardType	= 'brdt';	// Board type
const AEKeyword		kKeyBoardNum	= 'brdn';	// Board number
const AEKeyword		kKeyRegNum		= 'regn';	// Register number
const AEKeyword		kKeyRegMask		= 'regm';	// Register mask
const AEKeyword		kKeyRegValue	= 'regv';	// Register value

const AEKeyword		kKeyPrefGetFlag	= 'getf';	// Flag to return data

// NTV2 descriptor types

const DescType		kTypeBoardType	= 'tbrd';	// Board type type

// NTV2 enumerated types

typedef enum
{
	kBoardTypeKSD					= 'nksd',
	kBoardTypeKHD					= 'nkhd',
	kBoardTypeHDNTV					= 'nhdn',
	kBoardTypeAll					= 'nall'
} BoardType;

enum
{
	kBoardNumAll = 999
};

#endif	//	NTV2DaemonAppleEvents_H_
