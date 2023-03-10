/**
	@file		ntv2driverinterface.h
	@brief		Declares the CNTV2DriverInterface base class.
	@copyright	(C) 2004-2016 AJA Video Systems, Inc.	Proprietary and confidential information.
**/

#ifndef NTV2DRIVERINTERFACE_H
#define NTV2DRIVERINTERFACE_H

#include "ajaexport.h"

#include "ajatypes.h"
#include "ntv2enums.h"
#include "videodefines.h"
#include "audiodefines.h"

#include "ntv2nubtypes.h"
#include "ntv2publicinterface.h"

#include <string>

#if defined(AJALinux ) || defined(AJAMac)
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

#ifdef MSWindows
	#include <WinSock.h>
	#include <assert.h>
#endif


#if !defined (AJA_VIRTUAL)
	#define	AJA_VIRTUAL		virtual
#endif

typedef struct {
    std::string     buildNumber;
    std::string     packageNumber;
    std::string     date;
    std::string     time;
} PACKAGE_INFO_STRUCT, *PPACKAGE_INFO_STRUCT;

/**
	@brief	I'm the base class that undergirds the platform-specific derived classes (from which CNTV2Card is ultimately derived).
**/
class AJAExport CNTV2DriverInterface
{
public:
	CNTV2DriverInterface ();
	virtual ~CNTV2DriverInterface ();

public:
	/**
		@brief		Opens an AJA device so that it can be monitored and/or controlled.
		@result		True if successful; otherwise false.
		@param[in]	inDeviceIndex		Optionally specifies a zero-based index number of the AJA device to open.
										Defaults to zero, which is the first AJA device found.
		@param[in]	displayError		Optionally specifies if an alert dialog should be displayed if a failure occurs
										when attempting to open the AJA device (on host platforms that support alert dialogs).
										Defaults to false.
		@param[in]	eDeviceType			Optionally specifies the type of AJA device to look for. This parameter is obsolete.
		@param[in]	hostName			Optionally specifies the name of a host machine on the local area network that has
										one or more AJA devices attached to it. Defaults to NULL, which attempts to open AJA
										devices on the local host. If not NULL, must be a valid pointer to a buffer containing
										a zero-terminated character string.
	**/
	virtual bool Open(UWord inDeviceIndex=0, bool displayError = false,
					  NTV2DeviceType eDeviceType = DEVICETYPE_UNKNOWN,
					  const char *hostName = 0) = 0;


	// call this before Open to set the shareable feature of the Card
	virtual bool SetShareMode (bool bShared) = 0;

	// call this before Open to set the overlapped feature of the Card
	virtual bool SetOverlappedMode (bool bOverlapped) = 0;

	/**
		@brief		Closes the AJA device, releasing host resources that may have been allocated in a previous Open call.
		@result		True if successful; otherwise false.
		@details	This function closes the CNTV2Card instance's connection to the AJA device.
					Once closed, the device can no longer be queried or controlled by the CNTV2Card instance.
					The CNTV2Card instance can be "reconnected" to another AJA device by calling its Open member function again.
	**/
	virtual bool Close() = 0;

	/**
		@brief		Updates or replaces all or part of the 32-bit contents of a specific register (real or virtual) on the AJA device.
					Using the optional mask and shift parameters, it's possible to set or clear any number of specific bits in a real
					register without altering any of the register's other bits.
		@result		True if successful; otherwise false.
		@param[in]	inRegisterNumber	Specifies the RegisterNum of the real register, or VirtualRegisterNum of the virtual register
										on the AJA device to be changed.
		@param[in]	inValue				Specifies the desired new register value. If the "inShift" parameter is non-zero, this value
										is shifted left by the designated number of bit positions before being masked and applied to
										the real register contents.
		@param[in]	inMask				Optionally specifies a bit mask to be applied to the new (shifted) value before updating the
										register. Defaults to 0xFFFFFFFF, which does not perform any masking.
										On Windows and MacOS, a zero mask is treated the same as 0xFFFFFFFF.
										On MacOS, "holes" in the mask (i.e., one or more runs of 0-bits lying between more-significant
										and less-significant 1-bits) were not handled correctly.
										This parameter is ignored when writing to a virtual register.
		@param[in]	inShift				Optionally specifies the number of bits to left-shift the specified value before applying
										it to the register. Defaults to zero, which does not perform any shifting.
										On MacOS, this parameter is ignored.
										On Windows, a shift value of 0xFFFFFFFF is treated the same as a zero shift value.
										This parameter is ignored when writing to a virtual register.
		@note		This function should be used only when there is no higher-level function available to accomplish the desired task.
		@note		The mask and shift parameters are ignored when setting a virtual register.
	**/
	virtual bool	WriteRegister (ULWord inRegisterNumber,  ULWord inValue,  ULWord inMask = 0xFFFFFFFF,  ULWord inShift = 0);

protected:
	virtual bool	ReadRegister (ULWord inRegisterNumber,  ULWord * pOutValue,  ULWord inMask = 0xFFFFFFFF,  ULWord inShift = 0);

public:
	/**
		@brief		Reads all or part of the 32-bit contents of a specific register (real or virtual) on the AJA device.
					Using the optional mask and shift parameters, it's possible to read any number of specific bits in a register
					while ignoring the register's other bits.
		@result		True if successful; otherwise false.
		@param[in]	inRegisterNumber	Specifies the RegisterNum of the real register, or VirtualRegisterNum of the virtual register
										on the AJA device to be read.
		@param[out]	outValue			Specifies a valid, non-NULL address of the ULWord that is to receive the register value obtained
										from the device.
		@param[in]	inMask				Optionally specifies a bit mask to be applied after reading the real device register.
										Defaults to 0xFFFFFFFF, which does not perform any masking.
										A zero mask (0x00000000) is also ignored.
										This parameter is ignored when reading a virtual register.
		@param[in]	inShift				Optionally specifies the number of bits to right-shift the value obtained
										from the device register after any mask has been applied.
										Defaults to zero, which performs no bit shift.
										This parameter is ignored when reading a virtual register.
		@note		This function should be used only when there is no higher-level function available to accomplish the desired task.
		@note		The mask and shift parameters are ignored when reading a virtual register.
	**/
	virtual inline bool	ReadRegister (const ULWord inRegisterNumber,  ULWord & outValue,  const ULWord inMask = 0xFFFFFFFF,  const ULWord inShift = 0)
	{
		return ReadRegister (inRegisterNumber, &outValue, inMask, inShift);
	}

	// Read multiple registers at once.
	virtual bool ReadRegisterMulti(	ULWord numRegs,
									ULWord *whichRegisterFailed,
							  		NTV2ReadWriteRegisterSingle aRegs[]);	///< @deprecated	Use CNTV2Card::ReadRegisters instead.

	virtual bool RestoreHardwareProcampRegisters() = 0;

	/**
		@brief		Transfers data between the AJA device and the host. This function will block and not return to the caller until
					the transfer has finished or failed.
		@param[in]	inDMAEngine		Specifies the device DMA engine to use. Use NTV2_DMA_FIRST_AVAILABLE for most applications.
									(Use the ::NTV2DeviceGetNumDMAEngines function to determine how many are available.)
		@param[in]	inIsRead		Specifies the transfer direction. Use 'true' for reading (device-to-host).
									Use 'false' for writing (host-to-device).
		@param[in]	inFrameNumber	Specifies the zero-based frame number of the starting frame to be transferred to/from the device.
		@param		pFrameBuffer	Specifies a valid, non-NULL address of the host buffer. If reading (device-to-host), this memory
									will be written into. If writing (host-to-device), this memory will be read from.
		@param[in]	inOffsetBytes	Specifies the byte offset into the device frame buffer where the data transfer will start.
		@param[in]	inByteCount		Specifies the total number of bytes to be transferred.
		@param[in]	inSynchronous	This parameter is obsolete, and ignored.
		@return		True if successful; otherwise false.
		@note		The host buffer must be at least inByteCount + inOffsetBytes in size; otherwise, host memory will be corrupted,
					or a bus error or other runtime exception may occur.
	**/
    virtual bool DmaTransfer (	const NTV2DMAEngine	inDMAEngine,
								const bool			inIsRead,
								const ULWord		inFrameNumber,
								ULWord *			pFrameBuffer,
								const ULWord		inOffsetBytes,
								const ULWord		inByteCount,
								const bool			inSynchronous = true);

	virtual bool DmaUnlock   (void)  = 0;

	virtual bool CompleteMemoryForDMA (ULWord * pFrameBuffer)  = 0;


	virtual bool PrepareMemoryForDMA (ULWord * pFrameBuffer, ULWord ulNumBytes)  = 0;


	virtual bool ConfigureInterrupt (bool bEnable,  INTERRUPT_ENUMS eInterruptType) = 0;

	virtual bool MapFrameBuffers (void)  = 0;

	virtual bool UnmapFrameBuffers (void)  = 0;

	virtual bool MapRegisters (void)  = 0;

	virtual bool UnmapRegisters (void)  = 0;

	virtual bool MapXena2Flash (void)  = 0;

	virtual bool UnmapXena2Flash (void)  = 0;

	virtual bool ConfigureSubscription (bool bSubscribe, INTERRUPT_ENUMS eInterruptType, PULWord & hSubcription);

	virtual bool GetInterruptCount (INTERRUPT_ENUMS eInterrupt,
								    ULWord *pCount)  = 0;

	virtual bool WaitForInterrupt (INTERRUPT_ENUMS eInterrupt, ULWord timeOutMs = 68);

	virtual bool	AutoCirculate (AUTOCIRCULATE_DATA &autoCircData);

	virtual inline bool		NTV2Message (NTV2_HEADER * pInMessage)					{ (void) pInMessage;  return false; }

	virtual bool ControlDriverDebugMessages(NTV2_DriverDebugMessageSet msgSet,
		  									bool enable ) = 0;

    virtual bool GetDriverVersion(ULWord* driverVersion) = 0;

	/**
		@return		True if the device is ready to be fully operable;  otherwise false.
		@note		Some devices have processors that require a lot of time (~30 to ~90 seconds) to start up after a PCIe bus reset,
					power-up or wake from sleep. Calls to Open, IsOpen, ReadRegister and WriteRegister will all succeed,
					but the device won't be capable of either ingesting or playing video or performing DMA operations.
	**/
	virtual bool		IsDeviceReady (void);
	virtual bool		IsMBSystemReady (void);
	virtual bool		IsKonaIPDevice (void);


    // Utility methods:
	#if !defined (NTV2_DEPRECATE)
		AJA_VIRTUAL NTV2_DEPRECATED NTV2BoardType	GetCompileFlag (void);
		virtual NTV2_DEPRECATED inline bool			BoardOpened (void) const		{ return IsOpen (); }
	#endif	//	!NTV2_DEPRECATE

	/**
		@brief	Returns true if I'm able to communicate with the device I represent.
		@return	True if I'm able to communicate with the device I represent;  otherwise false.
	**/
	virtual inline bool			IsOpen (void) const									{ return _boardOpened; }

	AJA_VIRTUAL void			InitMemberVariablesOnOpen (NTV2FrameGeometry frameGeometry, NTV2FrameBufferFormat frameFormat);

	virtual inline ULWord		GetPCISlotNumber (void) const						{ return _pciSlot; }

	virtual inline Word			SleepMs (LWord msec) const							{ (void) msec; return 0; }

	virtual inline ULWord		GetNumFrameBuffers (void) const						{ return _ulNumFrameBuffers; }
	virtual inline ULWord		GetAudioFrameBufferNumber (void) const				{ return (GetNumFrameBuffers () - 1); }
	virtual inline ULWord		GetFrameBufferSize (void) const						{ return _ulFrameBufferSize; }

	virtual NTV2_DEPRECATED bool DriverGetBitFileInformation (BITFILE_INFO_STRUCT & bitFileInfo,  NTV2BitFileType bitFileType = NTV2_VideoProcBitFile);	///< @deprecated	This function is obsolete.

	virtual bool DriverGetBuildInformation (BUILD_INFO_STRUCT & outBuildInfo);

    virtual bool GetPackageInformation(PACKAGE_INFO_STRUCT & packageInfo);

	// Functions for cards that support more than one bitfile

	virtual NTV2_DEPRECATED bool SwitchBitfile (NTV2DeviceID boardID, NTV2BitfileType bitfile)		{ (void) boardID; (void) bitfile; return false; }	///< @deprecated	This function is obsolete.

#if defined (NTV2_NUB_CLIENT_SUPPORT)
	virtual inline const char *	GetHostName (void) const							{ return _hostname.c_str (); }

	virtual inline NTV2NubProtocolVersion	GetNubProtocolVersion (void) const		{return _nubProtocolVersion;}

	virtual inline bool						IsRemote (void) const					{return _remoteHandle != INVALID_NUB_HANDLE;}
#else
	virtual inline bool						IsRemote (void) const					{return false;}
#endif

    virtual inline bool						HevcSendMessage (HevcMessageHeader * /*pMessage*/)		{ return false; }

protected:
	virtual NTV2_DEPRECATED bool			DisplayNTV2Error (const char * str)		{ (void) str; return  false;}	///< @deprecated	This function is obsolete.
#if defined (NTV2_NUB_CLIENT_SUPPORT)
	virtual bool				OpenRemote (UWord inDeviceIndex,
											bool displayErrorMessage,
											UWord ulBoardType,
											const char * hostname);
	virtual bool				CloseRemote (void);
#endif	//	defined (NTV2_NUB_CLIENT_SUPPORT)
	AJA_VIRTUAL bool			ParseFlashHeader (BITFILE_INFO_STRUCT &bitFileInfo);

	/**
		@brief	Private method that increments the event count tally for the given interrupt type.
		@param[in]	eInterruptType	Specifies the type of interrupt that occurred, which determines
									the counter to be incremented.
	**/
	virtual void	BumpEventCount (const INTERRUPT_ENUMS eInterruptType);

private:
	/**
		@brief	My assignment operator.
		@note	We have intentionally disabled this capability because it was never implemented
				and is currently broken. If/when it gets fixed, we'll make this a public method.
		@param[in]	inRHS	The rvalue to be assigned to the lvalue.
		@return	A non-constant reference to the lvalue.
	**/
	virtual CNTV2DriverInterface & operator = (const CNTV2DriverInterface & inRHS);

	/**
		@brief	My copy constructor.
		@note	We have intentionally disabled this because it was never implemented and is
				currently broken. If/when it gets fixed, we'll make this a public method.
		@param[in]	inObjToCopy		The object to be copied.
	**/
	CNTV2DriverInterface (const CNTV2DriverInterface & inObjToCopy);

protected:

    UWord					_boardNumber;			///< @brief	My device index number.
    bool					_boardOpened;			///< @brief	True if I'm open and connected to the device.
    NTV2DeviceType			_boardType;				///< @brief	This is obsolete.
	NTV2DeviceID			_boardID;				///< @brief	My cached device ID.
    bool					_displayErrorMessage;	///< @brief	This is obsolete.
	ULWord					_pciSlot;				//	FIXFIXFIX	Replace this with a std::string that identifies my location in the host device tree.
	ULWord					_programStatus;
#if defined (NTV2_NUB_CLIENT_SUPPORT)
	struct sockaddr_in		_sockAddr;				///< @brief	Socket address (if using nub to talk to remote host)
	AJASocket				_sockfd;				///< @brief	Socket descriptor (if using nub to talk to remote host)
	std::string				_hostname;				///< @brief	Remote host name (if using nub to talk to remote host)
	LWord					_remoteHandle;			///< @brief	Remote host handle (if using nub to talk to remote host)
	NTV2NubProtocolVersion	_nubProtocolVersion;	///< @brief	Protocol version (if using nub to talk to remote host)
#endif	//	defined (NTV2_NUB_CLIENT_SUPPORT)
	PULWord					mInterruptEventHandles	[eNumInterruptTypes];	///< @brief	For subscribing to each possible event, one for each interrupt type
	ULWord					mEventCounts			[eNumInterruptTypes];	///< @brief	My event tallies, one for each interrupt type. Note that these
																			///<		tallies are different from the interrupt tallies kept by the driver.
	ULWord *				_pFrameBaseAddress;

	// for old KSD and KHD boards
	ULWord *				_pCh1FrameBaseAddress;			//	DEPRECATE!
	ULWord *				_pCh2FrameBaseAddress;			//	DEPRECATE!

	ULWord *				_pRegisterBaseAddress;
	ULWord					_pRegisterBaseAddressLength;
	ULWord *				_pFS1FPGARegisterBaseAddress;	//	DEPRECATE!

	ULWord *				_pXena2FlashBaseAddress;

	ULWord					_ulNumFrameBuffers;
	ULWord					_ulFrameBufferSize;

};	//	CNTV2DriverInterface

#endif	//	NTV2DRIVERINTERFACE_H
