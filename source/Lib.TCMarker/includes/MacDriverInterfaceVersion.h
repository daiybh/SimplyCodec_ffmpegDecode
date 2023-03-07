/*****************************************************************************************
*
*	MacDriverInterfaceVersion.h
*
*
*	Copyright (C) 2012 AJA Video Systems, Inc.  Proprietary and Confidential information.
*	All rights reserved.
*
*****************************************************************************************/

//	plist	key:	"AJADriverInterfaceVersion"
//			value:	the numeric value represented by the symbol "AJA_MAC_DRIVER_INTERFACE_VERSION"

//	MacDriverInterfaceVersion		Description of Changes
//	-------------------------		----------------------------------------------------------------------
//				1					???
//				2					???
//				3					???
//				4					???
//				5					???
//				6					???
//				7					???
//				8					Valid since 10.2. Added reference-counted Acquire/Release calls.
//				9					Valid since NTV2 SDK 12.1
//
//	This important number is used in each driver's Info.plist, as well as the NTV2 Mac public interface.
//	CNTV2Card::Open will fail if the client (SDK) driver interface version number does not match the
//	driver version number that is reported by the driver.
//
#define	AJA_MAC_DRIVER_INTERFACE_VERSION	9
