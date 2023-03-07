/**
	@file		ntv2virtualregisters.h
	@copyright	Copyright (C) 2011-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares enums for virtual registers used in all platform drivers and the SDK.
**/

#ifndef NTV2VIRTUALREGISTERS_H
#define NTV2VIRTUALREGISTERS_H

// Virtual registers.
#define VIRTUALREG_START 10000

// Virtual registers.  Used to get things like device driver version.
// Explicit values are attached to these values,
// to try to prevent anyone inadvertently changing old members (risking incompatibility with older apps)
// when adding new members!
typedef enum
{
	// Common to all platforms
	kRegLinuxDriverVersion				= VIRTUALREG_START,			/* Packed version of Linux driver, used in watcher */

	// Windows platform custom section
	kRegRelativeVideoPlaybackDelay		= VIRTUALREG_START,			// Video Delay relative to audio, for Windows Media playback
	kRegAudioRecordPinDelay				= VIRTUALREG_START+1,		// Audio Delay relative to video, for Windows Media capture
	kRegDriverVersion					= VIRTUALREG_START+2,
	kRegGlobalAudioPlaybackMode			= VIRTUALREG_START+3,		// Shared with Linux, but not Mac
	kRegFlashProgramKey					= VIRTUALREG_START+4,
	kRegStrictTiming					= VIRTUALREG_START+5,		// Drift Correction requires Strict Frame Timing for Windows Media playback;  Required for BackHaul;  Correlate Presentation Time Stamp with Graph Clock;  Turn off (default) to allow Playback even when Graph Manager gives us a Bogus Clcok!

	// COMMON_VIRTUAL_REGS_KONA2
	kK2RegInputSelect					= VIRTUALREG_START+20,		/* Input 1, Input 2, DualLink	*/
	kK2RegSecondaryFormatSelect			= VIRTUALREG_START+21,		/* NTV2VideoFormats				*/
	kK2RegDigitalOutput1Select			= VIRTUALREG_START+22,		/* Primary, Secondary			*/
	kK2RegDigitalOutput2Select			= VIRTUALREG_START+23,		/* Primary, Secondary, DualLink */
	kK2RegAnalogOutputSelect			= VIRTUALREG_START+24,		/* Primary, Secondary			*/
	kK2RegAnalogOutputType				= VIRTUALREG_START+25,		/* Analog output type			*/
	kK2RegAnalogOutBlackLevel			= VIRTUALREG_START+26,		/* Analog output black level	*/

	// COMMON_VIRTUAL_REGS_MISC
	kVideoOutPauseMode					= VIRTUALREG_START+40,		/* whether we pause on a frame or a field */
	kPulldownPattern					= VIRTUALREG_START+41,		/* which 3:2 pulldown pattern to use */
	kColorSpaceMode						= VIRTUALREG_START+42,		/* which color space matrix (Rec601, Rec709, ...) to use */
	kGammaMode							= VIRTUALREG_START+43,		/* which gamma LUT (Rec601, Rec709, ...) to use */
	kLUTType							= VIRTUALREG_START+44,		/* the current LUT function loaded into hardware */
	kRGB10Range							= VIRTUALREG_START+45,		/* the user-selected 10-bit RGB range (SMPTE 64-940 or Full 0-1023) */
	kRGB10Endian						= VIRTUALREG_START+46,		/* the user selected 10-bit RGB endian */

	// Windows platform custom section
	kRegBitFileDownload					= VIRTUALREG_START+50,		// NTV2BitfileType
	kRegSaveRegistersToRegistry			= VIRTUALREG_START+51,		// no argument				
	kRegRecallRegistersFromRegistry		= VIRTUALREG_START+52,		// same address as above, on purpose
	kRegClearAllSubscriptions			= VIRTUALREG_START+53,		// NTV2BitfileType
	kRegRestoreHardwareProcampRegisters	= VIRTUALREG_START+54,
	kRegAcquireReferenceCount			= VIRTUALREG_START+55,		// Acquire the board with a reference count on acquire
	kRegReleaseReferenceCount			= VIRTUALREG_START+56,

	kRegDTAudioMux0						= VIRTUALREG_START+60,		// Firmware desired
	kRegDTAudioMux1						= VIRTUALREG_START+61,		// Firmware desired
	kRegDTAudioMux2						= VIRTUALREG_START+62,		// Firmware desired
	kRegDTFirmware						= VIRTUALREG_START+63,		// Firmware desired
	kRegDTVersionAja					= VIRTUALREG_START+64,		// Driver version (Aja)
	kRegDTVersionDurian					= VIRTUALREG_START+65,		// Driver version (Durian)
	kRegDTAudioCapturePinConnected		= VIRTUALREG_START+66,		// Audio Capture Pin Connected ?

	kRegTimeStampMode					= VIRTUALREG_START+70,		// 0-Scaled timestamps(100ns), 1- Raw PerformanceCounter
	kRegTimeStampLastOutputVerticalLo	= VIRTUALREG_START+71,		// Lower 32 bits.
	kRegTimeStampLastOutputVerticalHi	= VIRTUALREG_START+72,		// Hi 32 bits.
	kRegTimeStampLastInput1VerticalLo	= VIRTUALREG_START+73,
	kRegTimeStampLastInput1VerticalHi	= VIRTUALREG_START+74,
	kRegTimeStampLastInput2VerticalLo	= VIRTUALREG_START+75,
	kRegTimeStampLastInput2VerticalHi	= VIRTUALREG_START+76,
	kRegNumberVideoMappingRegisters		= VIRTUALREG_START+77,
	kRegNumberAudioMappingRegisters		= VIRTUALREG_START+78,
	kRegAudioSyncTolerance				= VIRTUALREG_START+79,
	kRegDmaSerialize					= VIRTUALREG_START+80,
	kRegSyncChannel						= VIRTUALREG_START+81,		// Mac name
	kRegSyncChannels					= VIRTUALREG_START+81,		// Windows and Linux name for the same thing
	kRegSoftwareUartFifo				= VIRTUALREG_START+82,
	kRegTimeCodeCh1Delay				= VIRTUALREG_START+83,
	kRegTimeCodeCh2Delay				= VIRTUALREG_START+84,
	kRegTimeCodeIn1Delay				= VIRTUALREG_START+85,
	kRegTimeCodeIn2Delay				= VIRTUALREG_START+86,
	kRegTimeCodeCh3Delay				= VIRTUALREG_START+87,
	kRegTimeCodeCh4Delay				= VIRTUALREG_START+88,
	kRegTimeCodeIn3Delay				= VIRTUALREG_START+89,
	kRegTimeCodeIn4Delay				= VIRTUALREG_START+90,
	kRegTimeCodeCh5Delay				= VIRTUALREG_START+91,
	kRegTimeCodeIn5Delay				= VIRTUALREG_START+92,
	kRegTimeCodeCh6Delay				= VIRTUALREG_START+93,
	kRegTimeCodeIn6Delay				= VIRTUALREG_START+94,
	kRegTimeCodeCh7Delay				= VIRTUALREG_START+95,
	kRegTimeCodeIn7Delay				= VIRTUALREG_START+96,
	kRegTimeCodeCh8Delay				= VIRTUALREG_START+97,
	kRegTimeCodeIn8Delay				= VIRTUALREG_START+98,

	kRegDebug1							= VIRTUALREG_START+100,		// general debug register

	// Control Panel virtual registers
	kDisplayReferenceSelect				= VIRTUALREG_START+120,
	kVANCMode							= VIRTUALREG_START+121,
	kRegDualStreamTransportType			= VIRTUALREG_START+122,
	kSDIOut1TransportType				= kRegDualStreamTransportType,
	kDSKMode							= VIRTUALREG_START+123,
	kIsoConvertEnable					= VIRTUALREG_START+124,
	kDSKAudioMode						= VIRTUALREG_START+125,
	kDSKForegroundMode					= VIRTUALREG_START+126,
	kDSKForegroundFade					= VIRTUALREG_START+127,
	kCaptureReferenceSelect				= VIRTUALREG_START+128,
#if !defined (NTV2_DEPRECATE)
	kPanMode							= VIRTUALREG_START+129,
#endif	//	!defined (NTV2_DEPRECATE)

	kReg2XTransferMode					= VIRTUALREG_START+130,
	kRegSDIOutput1RGBRange				= VIRTUALREG_START+131,
	kRegSDIInput1FormatSelect			= VIRTUALREG_START+132,
	kRegSDIInput2FormatSelect			= VIRTUALREG_START+133,
	kRegSDIInput1RGBRange				= VIRTUALREG_START+134,
	kRegSDIInput2RGBRange				= VIRTUALREG_START+135,
	kRegSDIInput1Stereo3DMode			= VIRTUALREG_START+136,
	kRegSDIInput2Stereo3DMode			= VIRTUALREG_START+137,
	kRegFrameBuffer1RGBRange			= VIRTUALREG_START+138,
	kRegFrameBuffer1Stereo3DMode		= VIRTUALREG_START+139,

#if !defined (NTV2_DEPRECATE)
	kPanModeOffsetH						= VIRTUALREG_START+140,
	kPanModeOffsetV						= VIRTUALREG_START+141,
#endif	//	!defined (NTV2_DEPRECATE)
	kK2RegAnalogInBlackLevel			= VIRTUALREG_START+142,
	kK2RegAnalogInputType				= VIRTUALREG_START+143,
	kRegHDMIOutColorSpaceModeCtrl		= VIRTUALREG_START+144,
	kHDMIOutProtocolMode				= VIRTUALREG_START+145,
	kRegHDMIOutStereoSelect				= VIRTUALREG_START+146,
	kRegHDMIOutStereoCodecSelect		= VIRTUALREG_START+147,
	kReversePulldownOffset				= VIRTUALREG_START+148,
	kRegSDIInput1ColorSpaceMode			= VIRTUALREG_START+149,

	kRegSDIInput2ColorSpaceMode			= VIRTUALREG_START+150,
	kRegSDIOutput2RGBRange				= VIRTUALREG_START+151,
	kRegSDIOutput1Stereo3DMode			= VIRTUALREG_START+152,
	kRegSDIOutput2Stereo3DMode			= VIRTUALREG_START+153,
	kRegFrameBuffer2RGBRange			= VIRTUALREG_START+154,
	kRegFrameBuffer2Stereo3DMode		= VIRTUALREG_START+155,
	kRegAudioGainDisable				= VIRTUALREG_START+156,
	kDBLAudioEnable						= VIRTUALREG_START+157,
	kActiveVideoOutFilter				= VIRTUALREG_START+158,
	kRegAudioInputMapSelect				= VIRTUALREG_START+159,

	kAudioInputDelay					= VIRTUALREG_START+160,
	kDSKGraphicFileIndex				= VIRTUALREG_START+161,
	kTimecodeBurnInMode					= VIRTUALREG_START+162,
	kUseQTTimecode						= VIRTUALREG_START+163,
	kRegAvailable164					= VIRTUALREG_START+164,
	kRP188SourceSelect					= VIRTUALREG_START+165,
	kQTCodecModeDebug					= VIRTUALREG_START+166,
	kRegHDMIOutColorSpaceModeStatus		= VIRTUALREG_START+167,
	kDeviceOnline						= VIRTUALREG_START+168,
	kIsDefaultDevice					= VIRTUALREG_START+169,

	kRegDesktopFrameBufferStatus		= VIRTUALREG_START+170,
	kRegSDIOutput1ColorSpaceMode		= VIRTUALREG_START+171,
	kRegSDIOutput2ColorSpaceMode		= VIRTUALREG_START+172,
	kAudioOutputDelay					= VIRTUALREG_START+173,
	kTimelapseEnable					= VIRTUALREG_START+174,
	kTimelapseCaptureValue				= VIRTUALREG_START+175,
	kTimelapseCaptureUnits				= VIRTUALREG_START+176,
	kTimelapseIntervalValue				= VIRTUALREG_START+177,
	kTimelapseIntervalUnits				= VIRTUALREG_START+178,
	kFrameBufferInstalled				= VIRTUALREG_START+179,

	kK2RegAnalogInStandard				= VIRTUALREG_START+180,
	kRegOutputTimecodeOffset			= VIRTUALREG_START+181,
	kRegOutputTimecodeType				= VIRTUALREG_START+182,
	kRegQuicktimeUsingBoard				= VIRTUALREG_START+183,		// Not used in Mac
	kRegApplicationPID					= VIRTUALREG_START+184,		// The rest of this section handled by IOCTL in Mac
	kRegApplicationCode					= VIRTUALREG_START+185,
	kRegReleaseApplication				= VIRTUALREG_START+186,
	kRegForceApplicationPID				= VIRTUALREG_START+187,
	kRegForceApplicationCode			= VIRTUALREG_START+188,

	// COMMON_VIRTUAL_REGS_PROCAMP_CONTROLS
	kRegProcAmpSDRegsInitialized		= VIRTUALREG_START+200,
	kRegProcAmpStandardDefBrightness	= VIRTUALREG_START+201,
	kRegProcAmpStandardDefContrast		= VIRTUALREG_START+202,
	kRegProcAmpStandardDefSaturation	= VIRTUALREG_START+203,
	kRegProcAmpStandardDefHue			= VIRTUALREG_START+204,
	kRegProcAmpStandardDefCbOffset		= VIRTUALREG_START+205,
	kRegProcAmpStandardDefCrOffset		= VIRTUALREG_START+206,
	kRegProcAmpEndStandardDefRange		= VIRTUALREG_START+207,

	kRegProcAmpHDRegsInitialized		= VIRTUALREG_START+220,
	kRegProcAmpHighDefBrightness		= VIRTUALREG_START+221,
	kRegProcAmpHighDefContrast			= VIRTUALREG_START+222,
	kRegProcAmpHighDefSaturationCb		= VIRTUALREG_START+223,
	kRegProcAmpHighDefSaturationCr		= VIRTUALREG_START+224,
	kRegProcAmpHighDefHue				= VIRTUALREG_START+225,
	kRegProcAmpHighDefCbOffset			= VIRTUALREG_START+226,
	kRegProcAmpHighDefCrOffset			= VIRTUALREG_START+227,
	kRegProcAmpEndHighDefRange			= VIRTUALREG_START+228,

	// COMMON_VIRTUAL_REGS_USERSPACE_BUFFLEVEL
	kRegChannel1UserBufferLevel			= VIRTUALREG_START+240,
	kRegChannel2UserBufferLevel			= VIRTUALREG_START+241,
	kRegInput1UserBufferLevel			= VIRTUALREG_START+242,
	kRegInput2UserBufferLevel			= VIRTUALREG_START+243,

	// COMMON_VIRTUAL_REGS_EX
	kRegProgressivePicture				= VIRTUALREG_START+260,
	kRegLUT2Type						= VIRTUALREG_START+261,
	kRegLUT3Type						= VIRTUALREG_START+262,
	kRegLUT4Type						= VIRTUALREG_START+263,
	kK2RegDigitalOutput3Select			= VIRTUALREG_START+264,
	kK2RegDigitalOutput4Select			= VIRTUALREG_START+265,
	kK2RegHDMIOutputSelect				= VIRTUALREG_START+266,
	kK2RegRGBRangeConverterLUTType		= VIRTUALREG_START+267,
	kRegTestPatternChoice				= VIRTUALREG_START+268,
	kRegTestPatternFormat				= VIRTUALREG_START+269,
	kRegEveryFrameTaskFilter			= VIRTUALREG_START+270,
	kRegDefaultInput					= VIRTUALREG_START+271,
	kRegDefaultVideoOutMode				= VIRTUALREG_START+272,
	kRegDefaultVideoFormat				= VIRTUALREG_START+273,
	kK2RegDigitalOutput5Select			= VIRTUALREG_START+274,
	kRegLUT5Type						= VIRTUALREG_START+275,

	// Macintosh platform custom section
	kRegMacUserModeDebugLevel			= VIRTUALREG_START+300,
	kRegMacKernelModeDebugLevel			= VIRTUALREG_START+301,
	kRegMacUserModePingLevel			= VIRTUALREG_START+302,
	kRegMacKernelModePingLevel			= VIRTUALREG_START+303,
	kRegLatencyTimerValue				= VIRTUALREG_START+304,
	kRegAudioAVSyncEnable				= VIRTUALREG_START+305,
	kRegAudioInputSelect				= VIRTUALREG_START+306,
	kSerialSuspended					= VIRTUALREG_START+307,
	kXilinxProgramming					= VIRTUALREG_START+308,
	kETTDiagLastSerialTimestamp			= VIRTUALREG_START+309,
	kETTDiagLastSerialTimecode			= VIRTUALREG_START+310,
	kStartupStatusFlags					= VIRTUALREG_START+311,
	kRegRGBRangeMode					= VIRTUALREG_START+312,
	kRegEnableQueuedDMAs				= VIRTUALREG_START+313,		//	If non-zero, enables queued DMAs on multi-engine devices (Mac only)

	// Linux platform custom section
	kRegBA0MemorySize					= VIRTUALREG_START+320,		// Memory-mapped register (BAR0) window size in bytes.
	kRegBA1MemorySize					= VIRTUALREG_START+321,		// Memory-mapped framebuffer window size in bytes.
	kRegBA4MemorySize					= VIRTUALREG_START+322,
	kRegNumDmaDriverBuffers				= VIRTUALREG_START+323,		// Number of bigphysarea frames available (Read only).
	kRegDMADriverBufferPhysicalAddress	= VIRTUALREG_START+324,		// Physical address of bigphysarea buffer
	kRegBA2MemorySize					= VIRTUALREG_START+325,
	kRegAcquireLinuxReferenceCount		= VIRTUALREG_START+326,		// Acquire the board with a reference count on acquire
	kRegReleaseLinuxReferenceCount		= VIRTUALREG_START+327,

	// IoHD virtual registers
	kRegAdvancedIndexing				= VIRTUALREG_START+340,		//	OBSOLETE after 12.4
	kRegTimeStampLastInput3VerticalLo	= VIRTUALREG_START+341,
	kRegTimeStampLastInput3VerticalHi	= VIRTUALREG_START+342,
	kRegTimeStampLastInput4VerticalLo	= VIRTUALREG_START+343,
	kRegTimeStampLastInput4VerticalHi	= VIRTUALREG_START+344,
	kRegTimeStampLastInput5VerticalLo	= VIRTUALREG_START+345,
	kRegTimeStampLastInput5VerticalHi	= VIRTUALREG_START+346,
	kRegTimeStampLastInput6VerticalLo	= VIRTUALREG_START+347,
	kRegTimeStampLastInput6VerticalHi	= VIRTUALREG_START+348,
	kRegTimeStampLastInput7VerticalLo	= VIRTUALREG_START+349,
	kRegTimeStampLastInput7VerticalHi	= VIRTUALREG_START+350,
	kRegTimeStampLastInput8VerticalLo	= VIRTUALREG_START+351,
	kRegTimeStampLastInput8VerticalHi	= VIRTUALREG_START+352,

	kRegTimeStampLastOutput2VerticalLo	= VIRTUALREG_START+353,
	kRegTimeStampLastOutput2VerticalHi	= VIRTUALREG_START+354,
	
	kRegTimeStampLastOutput3VerticalLo	= VIRTUALREG_START+355,
	kRegTimeStampLastOutput3VerticalHi	= VIRTUALREG_START+356,
	kRegTimeStampLastOutput4VerticalLo	= VIRTUALREG_START+357,
	kRegTimeStampLastOutput4VerticalHi	= VIRTUALREG_START+358,
	
	kRegTimeStampLastOutput5VerticalLo	= VIRTUALREG_START+359,
	
	kRegTimeStampLastOutput5VerticalHi	= VIRTUALREG_START+360,
	kRegTimeStampLastOutput6VerticalLo	= VIRTUALREG_START+361,
	kRegTimeStampLastOutput6VerticalHi	= VIRTUALREG_START+362,
	kRegTimeStampLastOutput7VerticalLo	= VIRTUALREG_START+363,
	kRegTimeStampLastOutput7VerticalHi	= VIRTUALREG_START+364,
	kRegTimeStampLastOutput8VerticalLo	= VIRTUALREG_START+365,
	
	kRegResetCycleCount					= VIRTUALREG_START+366,		// counts the number of device resets caused by plug-and-play or sleep, increments each time
	kRegUseProgressive					= VIRTUALREG_START+367,		// when an option (e.g. Avid MC) choose P over PSF formats
	
	kRegFlashSize						= VIRTUALREG_START+368,		// size of the flash partition for flash status
	kRegFlashStatus						= VIRTUALREG_START+369,		// progress of flash patition
	kRegFlashState						= VIRTUALREG_START+370,		// state status of flash

	kRegPCIDeviceID						= VIRTUALREG_START+371,		// set by driver (read only)

	kRegUartRxFifoSize					= VIRTUALREG_START+372,
	
	kRegEFTNeedsUpdating				= VIRTUALREG_START+373,		// set when any retail virtual register has been changed
    
    kRegSuspendSystemAudio              = VIRTUALREG_START+374,     // set when app wants to use AC audio and disable host audio (e.g., CoreAudio on MacOS)
    kRegAcquireReferenceCounter         = VIRTUALREG_START+375,

	kRegTimeStampLastOutput8VerticalHi	= VIRTUALREG_START+376,
	
	kRegFramesPerVertical				= VIRTUALREG_START+377,
	kRegServicesInitialized				= VIRTUALREG_START+378,		// set true when device is initialized by services

	kRegFrameBufferGangCount			= VIRTUALREG_START+379,

	kRegChannelCrosspointFirst			= VIRTUALREG_START+380,
	kRegChannelCrosspointLast			= VIRTUALREG_START+387,	

	kRegDriverVersionMajor				= VIRTUALREG_START+388,		// supported by all three platforms
	kRegDriverVersionMinor				= VIRTUALREG_START+389,		// used to check SDK version against the driver version
	kRegDriverVersionPoint				= VIRTUALREG_START+390,		// when calling ::Open
	kRegFollowInputFormat				= VIRTUALREG_START+391,

	kRegAncField1Offset					= VIRTUALREG_START+392,		///< @brief	How many bytes to subtract from the end of a frame buffer for field 1 ANC
	kRegAncField2Offset					= VIRTUALREG_START+393,		///< @brief	How many bytes to subtract from the end of a frame buffer for field 2 ANC
	kRegUnused_1						= VIRTUALREG_START+394,
	kRegUnused_2						= VIRTUALREG_START+395,
	
	kReg4kOutputTransportSelection		= VIRTUALREG_START+396,
	kRegCustomAncInputSelect			= VIRTUALREG_START+397,
	kRegUseThermostat					= VIRTUALREG_START+398,
	kRegThermalSamplingRate				= VIRTUALREG_START+399,
	kRegFanSpeed						= VIRTUALREG_START+400,

	kRegVideoFormatCh1					= VIRTUALREG_START+401,
	kRegVideoFormatCh2					= VIRTUALREG_START+402,
	kRegVideoFormatCh3					= VIRTUALREG_START+403,
	kRegVideoFormatCh4					= VIRTUALREG_START+404,
	kRegVideoFormatCh5					= VIRTUALREG_START+405,
	kRegVideoFormatCh6					= VIRTUALREG_START+406,
	kRegVideoFormatCh7					= VIRTUALREG_START+407,
	kRegVideoFormatCh8					= VIRTUALREG_START+408,

    // Sarek VOIP section
    kRegIPAddrEth0                      = VIRTUALREG_START+409,
    kRegSubnetEth0                      = VIRTUALREG_START+410,
    kRegGatewayEth0                     = VIRTUALREG_START+411,

    kRegIPAddrEth1                      = VIRTUALREG_START+412,
    kRegSubnetEth1                      = VIRTUALREG_START+413,
    kRegGatewayEth1                     = VIRTUALREG_START+414,
    
    kRegRxcEnable1                      = VIRTUALREG_START+415,
    kRegRxcPrimaryRxMatch1              = VIRTUALREG_START+416,
    kRegRxcPrimarySourceIp1             = VIRTUALREG_START+417,
    kRegRxcPrimaryDestIp1               = VIRTUALREG_START+418,
    kRegRxcPrimarySourcePort1           = VIRTUALREG_START+419,
    kRegRxcPrimaryDestPort1             = VIRTUALREG_START+420,
    kRegRxcPrimarySsrc1                 = VIRTUALREG_START+421,
    kRegRxcPrimaryVlan1                 = VIRTUALREG_START+422,
    kRegRxcSecondaryRxMatch1            = VIRTUALREG_START+423,
    kRegRxcSecondarySourceIp1           = VIRTUALREG_START+424,
    kRegRxcSecondaryDestIp1             = VIRTUALREG_START+425,
    kRegRxcSecondarySourcePort1         = VIRTUALREG_START+426,
    kRegRxcSecondaryDestPort1           = VIRTUALREG_START+427,
    kRegRxcSecondarySsrc1               = VIRTUALREG_START+428,
    kRegRxcSecondaryVlan1               = VIRTUALREG_START+429,
    kRegRxcNetworkPathDiff1             = VIRTUALREG_START+430,
    kRegRxcPlayoutDelay1                = VIRTUALREG_START+431,

    kRegRxcEnable2                      = VIRTUALREG_START+432,
    kRegRxcPrimaryRxMatch2              = VIRTUALREG_START+433,
    kRegRxcPrimarySourceIp2             = VIRTUALREG_START+434,
    kRegRxcPrimaryDestIp2               = VIRTUALREG_START+435,
    kRegRxcPrimarySourcePort2           = VIRTUALREG_START+436,
    kRegRxcPrimaryDestPort2             = VIRTUALREG_START+437,
    kRegRxcPrimarySsrc2                 = VIRTUALREG_START+438,
    kRegRxcPrimaryVlan2                 = VIRTUALREG_START+439,
    kRegRxcSecondaryRxMatch2            = VIRTUALREG_START+440,
    kRegRxcSecondarySourceIp2           = VIRTUALREG_START+441,
    kRegRxcSecondaryDestIp2             = VIRTUALREG_START+442,
    kRegRxcSecondarySourcePort2         = VIRTUALREG_START+443,
    kRegRxcSecondaryDestPort2           = VIRTUALREG_START+444,
    kRegRxcSecondarySsrc2               = VIRTUALREG_START+445,
    kRegRxcSecondaryVlan2               = VIRTUALREG_START+446,
    kRegRxcNetworkPathDiff2             = VIRTUALREG_START+447,
    kRegRxcPlayoutDelay2                = VIRTUALREG_START+448,

    kRegTxcEnable3                      = VIRTUALREG_START+449,
    kRegTxcPrimaryLocalPort3            = VIRTUALREG_START+450,
    kRegTxcPrimaryRemoteIp3             = VIRTUALREG_START+451,
    kRegTxcPrimaryRemotePort3           = VIRTUALREG_START+452,
    kRegTxcPrimaryRemoteMAC_lo3         = VIRTUALREG_START+453,
    kRegTxcPrimaryRemoteMAC_hi3         = VIRTUALREG_START+454,
    kRegTxcPrimaryAutoMac3              = VIRTUALREG_START+455,
    kRegTxcSecondaryLocalPort3          = VIRTUALREG_START+456,
    kRegTxcSecondaryRemoteIp3           = VIRTUALREG_START+457,
    kRegTxcSecondaryRemotePort3         = VIRTUALREG_START+458,
    kRegTxcSecondaryRemoteMAC_lo3       = VIRTUALREG_START+459,
    kRegTxcSecondaryRemoteMAC_hi3       = VIRTUALREG_START+460,
    kRegTxcSecondaryAutoMac3            = VIRTUALREG_START+461,

    kRegTxcEnable4                      = VIRTUALREG_START+462,
    kRegTxcPrimaryLocalPort4            = VIRTUALREG_START+463,
    kRegTxcPrimaryRemoteIp4             = VIRTUALREG_START+464,
    kRegTxcPrimaryRemotePort4           = VIRTUALREG_START+465,
    kRegTxcPrimaryRemoteMAC_lo4         = VIRTUALREG_START+466,
    kRegTxcPrimaryRemoteMAC_hi4         = VIRTUALREG_START+467,
    kRegTxcPrimaryAutoMac4              = VIRTUALREG_START+468,
    kRegTxcSecondaryLocalPort4          = VIRTUALREG_START+469,
    kRegTxcSecondaryRemoteIp4           = VIRTUALREG_START+470,
    kRegTxcSecondaryRemotePort4         = VIRTUALREG_START+471,
    kRegTxcSecondaryRemoteMAC_lo4       = VIRTUALREG_START+472,
    kRegTxcSecondaryRemoteMAC_hi4       = VIRTUALREG_START+473,
    kRegTxcSecondaryAutoMac4            = VIRTUALREG_START+474,

    kRegMailBoxAcquire                  = VIRTUALREG_START+475,
    kRegMailBoxRelease                  = VIRTUALREG_START+476,
    kRegMailBoxAbort                    = VIRTUALREG_START+477,
    kRegMailBoxTimeoutNS                = VIRTUALREG_START+478,		//	Units are 100 ns, not nanoseconds!
    
    kReg2022_2EncodeVideoFormatCh1      = VIRTUALREG_START+479,
    kReg2022_2EncodeUllModeCh1          = VIRTUALREG_START+480,
    kReg2022_2EncodeBitDepthCh1         = VIRTUALREG_START+481,
    kReg2022_2EncodeChromaSubSampCh1    = VIRTUALREG_START+482,
    kReg2022_2EncodeCodeBlockSizeCh1    = VIRTUALREG_START+483,
    kReg2022_2EncodeMbpsCh1             = VIRTUALREG_START+484,
    kReg2022_2EncodePidCh1              = VIRTUALREG_START+485,
    kReg2022_2EncodeStreamTypeCh1       = VIRTUALREG_START+486,

    kReg2022_2EncodeVideoFormatCh2      = VIRTUALREG_START+487,
    kReg2022_2EncodeUllModeCh2          = VIRTUALREG_START+488,
    kReg2022_2EncodeBitDepthCh2         = VIRTUALREG_START+489,
    kReg2022_2EncodeChromaSubSampCh2    = VIRTUALREG_START+490,
    kReg2022_2EncodeCodeBlockSizeCh2    = VIRTUALREG_START+491,
    kReg2022_2EncodeMbpsCh2             = VIRTUALREG_START+492,
    kReg2022_2EncodePidCh2              = VIRTUALREG_START+493,
    kReg2022_2EncodeStreamTypeCh2       = VIRTUALREG_START+494,

    kReg2022_7Enable                    = VIRTUALREG_START+495,

    kRegUserDefinedDBB                  = VIRTUALREG_START+496,

	kRegHDMIOutAudioChannels			= VIRTUALREG_START+497,
	kRegHDMIOutRGBRange					= VIRTUALREG_START+498,

	// NOTE: THIS VALUE HAS TO MATCH WHAT IS RETURNED BY NTV2BoardGetNumberRegisters
	kRegLast							= VIRTUALREG_START+499

} VirtualRegisterNum;

#endif  // NTV2VIRTUALREGISTERS_H

