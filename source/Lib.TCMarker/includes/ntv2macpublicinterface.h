/**
	@file		ntv2macpublicinterface.h
	@copyright	Copyright (C) 2012-2015 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares MacOS-only enums used by the Mac driver and the SDK.
**/

#ifndef NTV2MACPUBLICINTERFACE_H
#define NTV2MACPUBLICINTERFACE_H

#include "MacDriverInterfaceVersion.h"					//	Defines AJA_MAC_DRIVER_INTERFACE_VERSION


//	Define this to respect the "regShift" parameter in ReadRegister & WriteRegister calls:
//#define	AJA_MAC_DRIVER_RW_REGISTER_USES_SHIFT


const uint32_t	kDriverInterfaceVers	= AJA_MAC_DRIVER_INTERFACE_VERSION;
const uint32_t	kSoftwareVersMajor		= AJA_NTV2_SDK_VERSION_MAJOR;
const uint32_t	kSoftwareVersMinor		= AJA_NTV2_SDK_VERSION_MINOR;
const uint32_t	kSoftwareVersPoint		= AJA_NTV2_SDK_VERSION_POINT;


//	Our custom driver IOReturn codes:
const IOReturn	kNTV2DriverBadDMA		= 0x04430011;	// Bad dmaEngine number
const IOReturn	kNTV2DriverDMABusy		= 0x04430012;	// Selected DMA busy or none available
const IOReturn	kNTV2DriverParamErr		= 0x04430013;	// Bad parameter (out of range)
const IOReturn	kNTV2DriverPgmXilinxErr	= 0x04430014;	// Xilinx programming error - see system log for details
const IOReturn	kNTV2DriverNotReadyErr	= 0x04430015;	// Xilinx not programmed yet
const IOReturn	kNTV2DriverPrepMemErr	= 0x04430016;	// Error preparing memory (no room),
const IOReturn	kNTV2DriverPgmPPCErr	= 0x04430017;	// PPC programming error - see system log for details
const IOReturn	kNTV2DriverDMATooLarge	= 0x04430018;	// DMA transfer is too large, or completing (dataPtr not found),
														// or attempt to use outside of range
const IOReturn	kNTV2DriverBadHeaderTag	= 0x04430019;	//	Bad header tag
const IOReturn	kNTV2UnknownStructType	= 0x0443001A;	//	Unknown struct type
const IOReturn	kNTV2HeaderVersionErr	= 0x0443001B;	//	Bad or unsupported header version
const IOReturn	kNTV2DriverBadTrailerTag= 0x0443001C;	//	Bad trailer tag
const IOReturn	kNTV2DriverMapperErr	= 0x0443001D;	//	Failure while mapping NTV2 struct ptrs
const IOReturn	kNTV2DriverUnmapperErr	= 0x0443001E;	//	Failure while unmapping NTV2 struct ptrs


// Driver command function codes
typedef enum
{
	kDriverReadRegister									= 0,
	kDriverWriteRegister								= 1,
	kDriverGetDrvrVersion								= 2,
	kDriverStartDriver									= 3,
	kDriverAcquireStreamForApplication					= 4,
	kDriverReleaseStreamForApplication					= 5,
	kDriverSetStreamForApplication						= 6,
	kDriverGetStreamForApplication						= 7,
	kDriverLockFormat									= 8,
	kDriverSetAVSyncPattern								= 9,
	kDriverTriggerAVSync								= 10,
	kDriverWaitForInterrupt								= 11,
	kDriverGetInterruptCount							= 12,
	kDriverWaitForChangeEvent							= 13,
	kDriverGetTime										= 14,
	kDriverDMATransfer									= 15,
	kDriverRestoreProcAmpRegisters						= 16,
	kDriverSetBitFileInformation						= 17,
	kDriverGetBitFileInformation						= 18,
 	kDriverSetDebugFilterStrings						= 19,
	kDriverGetDebugFilterStrings						= 20,
    kDriverAutoCirculateControl							= 21,
	kDriverAutoCirculateStatus							= 22,
    kDriverAutoCirculateTransfer						= 23,
    kDriverAutoCirculateFramestamp						= 24,
    kDriverSetDefaultDeviceForPID						= 25,
    kDriverIsDefaultDeviceForPID						= 26,
	kDriverSystemControl								= 27,
	kDriverSystemStatus									= 28,
	kDriverDMATransferEx								= 29,
	kDriverAcquireStreamForApplicationWithReference		= 30,
	kDriverReleaseStreamForApplicationWithReference		= 31,
	kDriverKernelLog									= 32,
	kDriverReadAudioBuffer								= 33,
	kDriverNTV2Message									= 34,
	kNumberUserClientCommands	// number of driver commands
} UserClientCommandCodes;


// Memory types for IOConnectMapMemory

typedef enum
{
	kRegisterMemory,
	kFrameBufferMemory,
	kXena2FlashMemory,

	kNumberMemoryType	// number of Memory types
} MemoryType;

typedef enum
{
	kInterruptOutputVertical= 1,
	kInterruptInput1Vertical,
	kInterruptInput2Vertical,
	kInterruptAudio,
	kInterruptAudioOutWrap,
	kInterruptAudioInWrap,

	kInterruptDMA1,
	kInterruptDMA2,
	kInterruptDMA3,
	kInterruptDMA4
} InterruptType;

typedef enum
{
	kDMANone =	0,		// Use PIO
	kDMA1 =		1,
	kDMA2 =		2,
	kDMA3 =		3,
	kDMA4 =		4,
	kDMAAuto =	99		// Auto DMA selection
} DMAEngine;


// Virtual registers
#include "ntv2virtualregisters.h"

#define AUDIO_MAX_SAMPLING_RATE_PER_MS 48

// Some IoHD specific registers
enum
{
	// isoch global
	kRegIoHDGlobalStatus		= 2048,
	kRegIoHDGlobalControl		= 2049,
	kRegLocalRegBaseAddrHi		= 2050,				// our local base register used for delivery of interrupts
	kRegLocalRegBaseAddrLo		= 2051,				// our local base register used for delivery of interrupts

	// ischo ch1 video
	kRegIsochVideoCh1Status		= 2052,
	kRegIsochVideoCh1Control	= 2053,
	kRegIsochVideoCh1Packets	= 2054,

	// isoch ch1 audio
	kRegIsochAudioCh1Status		= 2056,				// hardwired to be the audio playback channel
	kRegIsochAudioCh1Control	= 2057,
	kRegIsochAudioCh1Packets	= 2058,

	// isoch ch2 audio
	kRegIsochAudioCh2Status		= 2060,				// hardwired to be the audio capture channel
	kRegIsochAudioCh2Control	= 2061,
	kRegIsochAudioCh2Packets	= 2062,

	// misc
	kRegFireWireErrors			= 2064,				// Read - crc errors [0:15], runt packets [16:31]
	kRegAudioBufferSize			= 2065,				// audio buffer size
	kRegAudioPlaybackControl	= 2066,				// audio playuback control

	// AV bitfile select
	kRegAVBitFileCSR			= 2068,				// AV bitfile control and status - currently loaded AV bitfile (read / write)
	kRegBitFileBusy				= 2069,				// Set true when busy during bitfile loading (read only)
	kRegGetBitFileInfoSelect	= 2070,				// which bitfile we want info (write only)
	kRegUserState1				= 2071,				// contains user state info (Primary/Secondary format, Input select)

	kRegVideoDelay				= 2072,				// video delay register

	// firmware updates
	kRegUpdateControl			= 2112,				// Update control register
	//kRegUpdateSoftwareVersion	= 2113,				// Software version
	//kRegUpdateProductVersion	= 2114,				// Product version
	kRegUpdateBitFileCommand	= 2115,				// Which bitfile will be updated (write only)
	kRegUpdateCRC				= 2116,				// CRC register
	kRegUpdateStatus			= 2117,				// Status register
	kRegSerialNumber			= 2118,				// Serial register 2 quadlet, 8 bytes
	kRegUpdateData				= 2120,				// data 128(0x80) quadlets, or 512(0x200) bytes, thru 0x8d0

	// NOTE: THIS VALUE HAS TO MATCH WHAT IS RETURNED BY NTV2BoardGetNumberRegisters
	kRegIoHDLast				= 2624
};


// isoch streams (channels)
enum IsochStreamType
{
	kIsochVideoStream			= 0,
	kIsochAudioPlaybackStream	= 1,
	kIsochAudioCaptureStream	= 2,
	kIsochMaxStreams			= 3
};

// update register values
enum
{
	//kMaskStandBusyStatus
	kStandAloneNormalStatus		= 0,
	kStandAloneBusyStatus		= 1,

	// kMaskStandAloneMode
	kStandAloneNoOp				= 0,
	kStandAloneFactoryBurn		= 1,
	kStandAloneUserBurn			= 2,
	kStandAlonePatternBurn		= 4
};

#endif	//	NTV2MACPUBLICINTERFACE_H
