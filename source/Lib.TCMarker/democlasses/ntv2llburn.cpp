/**
	@file		ntv2llburn.cpp
	@brief		Implementation of NTV2LLBurn demonstration class.
	@copyright	Copyright (C) 2012-2016 AJA Video Systems, Inc.  All rights reserved.
**/

#include "ntv2llburn.h"
#include "ajastuff/common/types.h"
#include "ajastuff/system/memory.h"
#include <iostream>


#define NTV2_AUDIOSIZE_MAX	(401 * 1024)

const uint32_t	kAppSignature	(AJA_FOURCC ('L','l','b','u'));


NTV2LLBurn::NTV2LLBurn (const string &				inDeviceSpecifier,
						const bool					inWithAudio,
						const NTV2FrameBufferFormat	inPixelFormat,
						const NTV2InputSource		inInputSource,
						const NTV2TCIndex			inTCIndex,
						const bool					inDoMultiChannel)

	:	mRunThread				(NULL),
		mDeviceID				(DEVICE_ID_NOTFOUND),
		mDeviceSpecifier		(inDeviceSpecifier),
		mWithAudio				(inWithAudio),
		mInputChannel			(NTV2_CHANNEL1),
		mOutputChannel			(NTV2_CHANNEL3),
		mInputSource			(inInputSource),
		mTimecodeIndex			(inTCIndex),
		mOutputDestination		(NTV2_OUTPUTDESTINATION_SDI3),
		mVideoFormat			(NTV2_FORMAT_UNKNOWN),
		mPixelFormat			(inPixelFormat),
		mSavedTaskMode			(NTV2_DISABLE_TASKS),
		mVancEnabled			(false),
		mWideVanc				(false),
		mAudioSystem			(NTV2_AUDIOSYSTEM_1),
		mDoMultiChannel			(inDoMultiChannel),
		mGlobalQuit				(false),
		mVideoBufferSize		(0),
		mAudioBufferSize		(0),
		mpHostVideoBuffer		(NULL),
		mpHostAudioBuffer		(NULL),
		mFramesProcessed		(0),
		mFramesDropped			(0)
{
}	//	constructor


NTV2LLBurn::~NTV2LLBurn ()
{
	//	Stop my capture and playout threads, then destroy them...
	Quit ();

	delete mRunThread;
	mRunThread = NULL;

	//	Unsubscribe from input vertical event...
	mDevice.UnsubscribeInputVerticalEvent (mInputChannel);

	//	Free all my host buffers...
	if (mpHostVideoBuffer)
	{
		AJAMemory::FreeAligned (mpHostVideoBuffer);
		mpHostVideoBuffer = NULL;
	}

	if (mpHostAudioBuffer)
	{
		AJAMemory::FreeAligned (mpHostAudioBuffer);
		mpHostAudioBuffer = NULL;
	}

	if (!mDoMultiChannel)
	{
		mDevice.SetEveryFrameServices (mSavedTaskMode);										//	Restore prior service level
		mDevice.ReleaseStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ()));	//	Release the device
	}

	//	Don't leave the audio system active after we exit
	mDevice.SetAudioInputReset	(mAudioSystem, true);
	mDevice.SetAudioOutputReset	(mAudioSystem, true);

}	//	destructor


void NTV2LLBurn::Quit (void)
{
	//	Set the global 'quit' flag, and wait for the thread to go inactive...
	mGlobalQuit = true;

	if (mRunThread)
		while (mRunThread->Active ())
			AJATime::Sleep (10);

}	//	Quit


AJAStatus NTV2LLBurn::Init (void)
{
	AJAStatus	status	(AJA_STATUS_SUCCESS);

	//	Open the device...
	if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, mDevice))
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return AJA_STATUS_OPEN;}

	if (!mDevice.IsDeviceReady ())
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not ready" << endl;  return AJA_STATUS_INITIALIZE;}

	ULWord	appSignature	(0);
	int32_t	appPID			(0);
	mDevice.GetEveryFrameServices (mSavedTaskMode);				//	Save the current device state
	mDevice.GetStreamingApplication (&appSignature, &appPID);	//	Who currently "owns" the device?
	if (!mDoMultiChannel)
	{
		if (!mDevice.AcquireStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ())))
		{
			cerr << "## ERROR:  Unable to acquire device because another app (pid " << appPID << ") owns it" << endl;
			return AJA_STATUS_BUSY;		//	Some other app is using the device
		}
		mDevice.SetEveryFrameServices (NTV2_OEM_TASKS);			//	Set the OEM service level
		mDevice.ClearRouting ();								//	Clear the current device routing (since I "own" the device)
	}
	else
		mDevice.SetEveryFrameServices (NTV2_OEM_TASKS);			//	Force OEM tasks

	mDeviceID = mDevice.GetDeviceID ();							//	Keep the device ID handy since it will be used frequently

	//	Configure the SDI relays if present
	if (::NTV2DeviceHasSDIRelays (mDeviceID))
	{
		//	Note that if the board's jumpers are not set in the position
		//	to enable the watchdog timer, these calls will have no effect.
		mDevice.SetSDIWatchdogEnable12 (true);
		mDevice.SetSDIWatchdogEnable34 (true);

		//	Set timeout delay to 2 seconds expressed in multiples of 8 ns
		//	and take the relays out of bypass
		mDevice.SetSDIWatchdogTimeout (2 * 125000000);
		mDevice.KickSDIWatchdog ();

		//	Give the mechanical relays some time to switch
		AJATime::Sleep (500);
	}

	//	Set up the video and audio...
	status = SetupVideo ();
	if (AJA_FAILURE (status))
		return status;

	status = SetupAudio ();
	if (AJA_FAILURE (status))
		return status;

	//	Set up the circular buffers...
	status = SetupHostBuffers ();
	if (AJA_FAILURE (status))
		return status;

	//	Set up the device signal routing, and both playout and capture AutoCirculate...
	RouteInputSignal ();
	RouteOutputSignal ();

	if (NTV2_IS_ANALOG_TIMECODE_INDEX(mTimecodeIndex))
		mDevice.SetLTCInputEnable(true);	//	Enable analog LTC input (some LTC inputs are shared with reference input)


	//	This is for the timecode that we will burn onto the image...
	NTV2FormatDescriptor fd = GetFormatDescriptor (GetNTV2StandardFromVideoFormat (mVideoFormat),
													mPixelFormat,
													mVancEnabled,
													Is2KFormat (mVideoFormat),
													mWideVanc);

	//	Lastly, prepare my AJATimeCodeBurn instance...
	mTCBurner.RenderTimeCodeFont (CNTV2DemoCommon::GetAJAPixelFormat (mPixelFormat), fd.numPixels, fd.numLines);

	return AJA_STATUS_SUCCESS;

}	//	Init


AJAStatus NTV2LLBurn::SetupVideo (void)
{
	const uint16_t	numFrameStores	(::NTV2DeviceGetNumFrameStores (mDeviceID));

	//	Set the video format to match the incoming video format.
	//	Does the device support the desired input source?
	if (!::NTV2DeviceCanDoInputSource (mDeviceID, mInputSource))
	{
		cerr << "## ERROR:  This device cannot receive input from the specified source" << endl;
		return AJA_STATUS_BAD_PARAM;	//	Nope
	}

	mInputChannel = ::NTV2InputSourceToChannel (mInputSource);

	switch (mInputSource)
	{
		case NTV2_INPUTSOURCE_ANALOG:	mInputChannel = NTV2_CHANNEL1;  mOutputChannel = NTV2_CHANNEL3;								break;
		case NTV2_INPUTSOURCE_HDMI:		mInputChannel = NTV2_CHANNEL1;  mOutputChannel = NTV2_CHANNEL3;								break;
		case NTV2_INPUTSOURCE_SDI1:		mOutputChannel = numFrameStores == 2 || numFrameStores > 4 ? NTV2_CHANNEL2 : NTV2_CHANNEL3;	break;
		case NTV2_INPUTSOURCE_SDI2:		mOutputChannel = numFrameStores > 4 ? NTV2_CHANNEL3 : NTV2_CHANNEL4;						break;
		case NTV2_INPUTSOURCE_SDI3:		mOutputChannel = NTV2_CHANNEL4;																break;
		case NTV2_INPUTSOURCE_SDI4:		mOutputChannel = numFrameStores > 4 ? NTV2_CHANNEL5 : NTV2_CHANNEL3;						break;
		case NTV2_INPUTSOURCE_SDI5: 	mOutputChannel = NTV2_CHANNEL6;																break;
		case NTV2_INPUTSOURCE_SDI6:		mOutputChannel = NTV2_CHANNEL7;																break;
		case NTV2_INPUTSOURCE_SDI7:		mOutputChannel = NTV2_CHANNEL8;																break;
		case NTV2_INPUTSOURCE_SDI8:		mOutputChannel = NTV2_CHANNEL7;																break;
		default:	cerr << "## ERROR:  Bad input source" << endl;  return AJA_STATUS_BAD_PARAM;
	}

	bool	isTransmit	(false);
	if (::NTV2DeviceHasBiDirectionalSDI (mDevice.GetDeviceID ())			//	If device has bidirectional SDI connectors (Io4K, Corvid24, etc.)...
		&& NTV2_INPUT_SOURCE_IS_SDI (mInputSource)							//	...and desired input source is SDI...
			&& mDevice.GetSDITransmitEnable (mInputChannel, &isTransmit)	//	...and GetSDITransmitEnable succeeds...
				&& isTransmit)												//	...and input is set to "transmit"...
	{
		mDevice.SetSDITransmitEnable (mInputChannel, false);				//	...then disable transmit mode...
		AJATime::Sleep (500);												//	...and give the device a dozen frames or so to lock to the input signal
	}	//	if input SDI connector needs to switch from transmit mode

	//	Determine the input video signal format, and set the device's reference source to that input...
	mVideoFormat = mDevice.GetInputVideoFormat (mInputSource);
	if (mVideoFormat == NTV2_FORMAT_UNKNOWN)
	{
		cerr << "## ERROR:  The specified input has no signal, or the device cannot handle the signal format" << endl;
		return AJA_STATUS_NOINPUT;	//	Sorry, can't handle this format
	}

	mDevice.SetReference (::NTV2InputSourceToReferenceSource (mInputSource));
	mAudioSystem		= ::NTV2InputSourceToAudioSystem (mInputSource);
	mOutputDestination	= ::NTV2ChannelToOutputDestination (mOutputChannel);

	if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID)					//	If device has bidirectional SDI connectors...
		&& NTV2_OUTPUT_DEST_IS_SDI (mOutputDestination))			//	...and output destination is SDI...
			mDevice.SetSDITransmitEnable (mOutputChannel, true);	//	...then enable transmit mode

	mDevice.EnableChannel (mInputChannel);		//	Enable the input frame buffer
	mDevice.EnableChannel (mOutputChannel);		//	Enable the output frame buffer

	if(::NTV2DeviceCanDoMultiFormat (mDeviceID) && mDoMultiChannel)
		mDevice.SetMultiFormatMode (true);
	else if(::NTV2DeviceCanDoMultiFormat (mDeviceID))
		mDevice.SetMultiFormatMode (false);

	//	Set the input channel format to the detected input format...
	mDevice.SetVideoFormat (mVideoFormat, false, false, ::NTV2DeviceCanDoMultiFormat (mDeviceID) ? mInputChannel : NTV2_CHANNEL1);
	if (::NTV2DeviceCanDoMultiFormat (mDeviceID))									//	If device supports multiple formats per-channel...
		mDevice.SetVideoFormat (mVideoFormat, false, false, mOutputChannel);		//	...then also set the output channel format to the detected input format

	//	Set the frame buffer pixel format for both channels on the device, assuming it
	//	supports that pixel format . . . otherwise default to 8-bit YCbCr...
	if (!::NTV2DeviceCanDoFrameBufferFormat (mDeviceID, mPixelFormat))
		mPixelFormat = NTV2_FBF_8BIT_YCBCR;

	//	Set the pixel format for both device frame buffers...
	mDevice.SetFrameBufferFormat (mInputChannel, mPixelFormat);
	mDevice.SetFrameBufferFormat (mOutputChannel, mPixelFormat);

	//	Enable and subscribe to the interrupts for the channel to be used...
	mDevice.EnableInputInterrupt (mInputChannel);
	mDevice.SubscribeInputVerticalEvent (mInputChannel);

	//	Enable and subscribe to the output interrupts (though it's enabled by default)...
	mDevice.EnableOutputInterrupt (mInputChannel);
	mDevice.SubscribeOutputVerticalEvent (mInputChannel);

	//
	//	Normally, timecode embedded in the output signal comes from whatever is written
	//	into the RP188 registers (30/31 for SDI out 1, 65/66 for SDIout2, etc.).
	//	Under this scheme, in this demo, the timecode embedded in the output will be up
	//	to 3 frames ahead of the timecode that's actually burned into the frame. Modern
	//	AJA devices can bypass the RP188 registers and simply copy whatever timecode appears
	//	in any input source (called the "bypass source"). To ensure that the playout timecode
	//	will actually be seen in the output signal, "bypass mode" must be disabled...
	//
	bool	isBypassEnabled	(false);
	mDevice.IsRP188BypassEnabled (mInputChannel, isBypassEnabled);
	if (isBypassEnabled)
		mDevice.DisableRP188Bypass (mInputChannel);

	//	Be sure the RP188 source for the input channel is being grabbed from the right input...
	mDevice.SetRP188Source (mInputChannel, mInputSource);

	//	Enable timecode output
	mDevice.SetRP188Mode (mOutputChannel, NTV2_RP188_OUTPUT);

	if (!NTV2_IS_SD_VIDEO_FORMAT (mVideoFormat))
	{
		//	Enable VANC for non-SD formats, to pass thru captions, etc.
		mDevice.SetEnableVANCData (false);
		if (::Is8BitFrameBufferFormat (mPixelFormat))
		{
			//	8-bit FBFs require VANC bit shift...
			mDevice.SetVANCShiftMode (mInputChannel, NTV2_VANCDATA_8BITSHIFT_ENABLE);
			mDevice.SetVANCShiftMode (mOutputChannel, NTV2_VANCDATA_8BITSHIFT_ENABLE);
		}
	}	//	if not SD video
	mDevice.GetEnableVANCData (&mVancEnabled, &mWideVanc);

	//	Tell the hardware which buffers to use until the main worker thread runs
	mDevice.SetInputFrame	(mInputChannel,  0);
	mDevice.SetOutputFrame	(mOutputChannel, 2);

	//	Set the Frame Store modes
	mDevice.SetMode	(mInputChannel,  NTV2_MODE_CAPTURE);
	mDevice.SetMode (mOutputChannel, NTV2_MODE_DISPLAY);

	return AJA_STATUS_SUCCESS;

}	//	SetupVideo


AJAStatus NTV2LLBurn::SetupAudio (void)
{
	//	Have the audio subsystem capture audio from the designated input source...
	mDevice.SetAudioSystemInputSource (mAudioSystem, ::NTV2InputSourceToAudioSource (mInputSource), ::NTV2InputSourceToEmbeddedAudioInput (mInputSource));

	//	It's best to use all available audio channels...
	mDevice.SetNumberAudioChannels (::NTV2DeviceGetMaxAudioChannels (mDeviceID), mAudioSystem);

	//	Assume 48kHz PCM...
	mDevice.SetAudioRate (NTV2_AUDIO_48K, mAudioSystem);

	//	4MB device audio buffers work best...
	mDevice.SetAudioBufferSize (NTV2_AUDIO_BUFFER_BIG, mAudioSystem);

	//	Set up the output audio embedders...
	if (::NTV2DeviceGetNumAudioSystems (mDeviceID) > 1)
		mDevice.SetSDIOutputAudioSystem (mOutputChannel, mAudioSystem);

	//
	//	Loopback mode plays whatever audio appears in the input signal when it's
	//	connected directly to an output (i.e., "end-to-end" mode). If loopback is
	//	left enabled, the video will lag the audio as video frames get briefly delayed
	//	in our ring buffer. Audio, therefore, needs to come out of the (buffered) frame
	//	data being played, so loopback must be turned off...
	//
	mDevice.SetAudioLoopBack (NTV2_AUDIO_LOOPBACK_OFF, mAudioSystem);

	//	Reset both the input and output sides of the audio system so that the buffer
	//	pointers are reset to zero and inhibited from advancing.
	mDevice.SetAudioOutputReset	(mAudioSystem, true);
	mDevice.SetAudioInputReset	(mAudioSystem, true);

	//	Ensure that the audio system will capture samples when the reset is removed
	mDevice.SetAudioCaptureEnable (mAudioSystem, true);

	return AJA_STATUS_SUCCESS;

}	//	SetupAudio


AJAStatus NTV2LLBurn::SetupHostBuffers (void)
{
	mVideoBufferSize = GetVideoWriteSize (mVideoFormat, mPixelFormat, mVancEnabled, mWideVanc);
	mAudioBufferSize = NTV2_AUDIOSIZE_MAX;

	//	Allocate and add each in-host buffer to my member variables.
	//	Note that DMA performance can be accelerated slightly by using page-aligned video buffers...
	mpHostVideoBuffer = reinterpret_cast <uint32_t *> (AJAMemory::AllocateAligned (mVideoBufferSize, AJA_PAGE_SIZE));
	mpHostAudioBuffer = reinterpret_cast <uint32_t *> (AJAMemory::AllocateAligned (mAudioBufferSize, AJA_PAGE_SIZE));

	if (!mpHostVideoBuffer || !mpHostAudioBuffer)
	{
		cerr << "## ERROR:  Unable to allocate host video and/or audio buffer " << endl;
		return AJA_STATUS_MEMORY;
	}

	return AJA_STATUS_SUCCESS;

}	//	SetupHostBuffers


void NTV2LLBurn::RouteInputSignal (void)
{
	const NTV2OutputCrosspointID	inputOutputXpt	(::GetInputSourceOutputXpt (mInputSource));
	const NTV2InputCrosspointID		fbInputXpt		(::GetFrameBufferInputXptFromChannel (mInputChannel));

	if (::IsRGBFormat (mPixelFormat))
	{
		//	If the frame buffer is configured for RGB pixel format, incoming YUV must be converted.
		//	This routes the video signal from the input through a color space converter before
		//	connecting to the RGB frame buffer...
		const NTV2InputCrosspointID		cscVideoInputXpt	(::GetCSCInputXptFromChannel (mInputChannel));
		const NTV2OutputCrosspointID	cscOutputXpt		(::GetCSCOutputXptFromChannel (mInputChannel, false, true));	//	false=video, true=RGB

		mDevice.Connect (cscVideoInputXpt, inputOutputXpt);	//	Connect the CSC's video input to the input spigot's output
		mDevice.Connect (fbInputXpt, cscOutputXpt);			//	Connect the frame store's input to the CSC's output
	}
	else
		mDevice.Connect (fbInputXpt, inputOutputXpt);		//	Route the YCbCr signal directly from the input to the frame buffer's input

}	//	RouteInputSignal


void NTV2LLBurn::RouteOutputSignal (void)
{
	const NTV2InputCrosspointID		outputInputXpt	(::GetOutputDestInputXpt (mOutputDestination));
	const NTV2OutputCrosspointID	fbOutputXpt		(::GetFrameBufferOutputXptFromChannel (mOutputChannel, ::IsRGBFormat (mPixelFormat)));
	NTV2OutputCrosspointID			outputXpt		(fbOutputXpt);

	if (::IsRGBFormat (mPixelFormat))
	{
		const NTV2OutputCrosspointID	cscVidOutputXpt	(::GetCSCOutputXptFromChannel (mOutputChannel, false, true));
		const NTV2InputCrosspointID		cscVidInputXpt	(::GetCSCInputXptFromChannel (mOutputChannel));

		mDevice.Connect (cscVidInputXpt, fbOutputXpt);		//	Connect the CSC's video input to the frame store's output
		mDevice.Connect (outputInputXpt, cscVidOutputXpt);	//	Connect the SDI output's input to the CSC's video output
		outputXpt = cscVidOutputXpt;
	}
	else
		mDevice.Connect (outputInputXpt, outputXpt);

	if (!mDoMultiChannel)
	{
		//	Route all SDI outputs to the outputXpt...
		const NTV2Channel	startNum		(NTV2_CHANNEL1);
		const NTV2Channel	endNum			(NTV2Channel (::NTV2DeviceGetNumVideoChannels (mDeviceID)));
		NTV2WidgetID		outputWidgetID	(NTV2_WIDGET_INVALID);

		for (NTV2Channel chan (startNum);  chan < endNum;  chan = NTV2Channel (chan + 1))
		{
			mDevice.SetRP188Source (chan, 0);	//	Set all SDI spigots to capture embedded LTC (VITC could be an option)

			if (chan == mInputChannel || chan == mOutputChannel)
				continue;	//	Skip the input & output channel, already routed
			if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID))
				mDevice.SetSDITransmitEnable (chan, true);
			if (CNTV2SignalRouter::GetWidgetForInput (::GetSDIOutputInputXpt (chan, ::NTV2DeviceCanDoDualLink (mDeviceID)), outputWidgetID))
				if (::NTV2DeviceCanDoWidget (mDeviceID, outputWidgetID))
				{
					mDevice.Connect (::GetSDIOutputInputXpt (chan), outputXpt);
				}
		}	//	for each output spigot

		//	If HDMI and/or analog video outputs are available, route them, too...
		if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtHDMIOut1))
			mDevice.Connect (NTV2_XptHDMIOutInput, outputXpt);			//	Route the output signal to the HDMI output
		if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtHDMIOut1v2))
			mDevice.Connect (NTV2_XptHDMIOutQ1Input, outputXpt);		//	Route the output signal to the HDMI output
		if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtAnalogOut1))
			mDevice.Connect (NTV2_XptAnalogOutInput, outputXpt);		//	Route the output signal to the Analog output
		if (::NTV2DeviceCanDoWidget (mDeviceID, NTV2_WgtSDIMonOut1))
			mDevice.Connect (::GetSDIOutputInputXpt (NTV2_CHANNEL5), outputXpt);	//	Route the output signal to the SDI monitor output
	}

}	//	RouteOutputSignal


AJAStatus NTV2LLBurn::Run ()
{
	//	Start the main worker thread...
	StartRunThread ();

	return AJA_STATUS_SUCCESS;

}	//	Run



//////////////////////////////////////////////

//	This is where we will start the worker thread
void NTV2LLBurn::StartRunThread (void)
{
	//	Create and start the worker thread...
	mRunThread = new AJAThread ();
	mRunThread->Attach (RunThreadStatic, this);
	mRunThread->SetPriority (AJA_ThreadPriority_High);
	mRunThread->Start ();

}	//	StartRunThread


//	The worker thread function
void NTV2LLBurn::RunThreadStatic (AJAThread * pThread, void * pContext)		//	static
{
	(void) pThread;
	//	Grab the NTV2LLBurn instance pointer from the pContext parameter,
	//	then call its ProcessFrames method...
	NTV2LLBurn *	pApp	(reinterpret_cast <NTV2LLBurn *> (pContext));
	pApp->ProcessFrames ();

}	//	RunThreadStatic


void NTV2LLBurn::ProcessFrames (void)
{
	uint32_t	currentInFrame			= 0;	//	Will ping-pong between 0 and 1
	uint32_t	currentOutFrame			= 2;	//	Will ping-pong between 2 and 3
	uint32_t	currentAudioInAddress	= 0;
	uint32_t	audioReadOffset			= 0;
	uint32_t	audioInWrapAddress		= 0;
	uint32_t	audioOutWrapAddress		= 0;
	uint32_t	audioBytesCaptured		= 0;
	bool		audioIsReset			= true;
	string		timeCodeString;

	mFramesProcessed = mFramesDropped = 0;	//	Start with a fresh frame count

	mDevice.GetAudioReadOffset	(audioReadOffset,		mAudioSystem);
	mDevice.GetAudioWrapAddress	(audioOutWrapAddress,	mAudioSystem);

	//	Wait to make sure the next two SDK calls will be made during the same frame...
	mDevice.WaitForInputFieldID (NTV2_FIELD0, mInputChannel);

	//	Before the main loop starts, ping-pong the buffers so the hardware will use
	//	different buffers than the ones it was using while idling...
	currentInFrame	^= 1;
	currentOutFrame	^= 1;
	mDevice.SetInputFrame	(mInputChannel,  currentInFrame);
	mDevice.SetOutputFrame	(mOutputChannel, currentOutFrame);

	//	Wait until the hardware starts filling the new buffers, and then start audio
	//	capture as soon as possible to match the video...
	mDevice.WaitForInputFieldID (NTV2_FIELD0, mInputChannel);
	mDevice.SetAudioInputReset	(mAudioSystem, false);

	mAudioInLastAddress		= audioReadOffset;
	audioInWrapAddress		= audioOutWrapAddress + audioReadOffset;
	mAudioOutLastAddress	= 0;

	currentInFrame	^= 1;
	currentOutFrame	^= 1;
	mDevice.SetInputFrame	(mInputChannel,  currentInFrame);
	mDevice.SetOutputFrame	(mOutputChannel, currentOutFrame);

	while (!mGlobalQuit)
	{
		//	Wait until the input has completed capturing a frame...
		mDevice.WaitForInputFieldID (NTV2_FIELD0, mInputChannel);

		if (mWithAudio)
		{
			//	Read the audio position registers as close to the interrupt as possible...
			mDevice.ReadAudioLastIn (&currentAudioInAddress, mInputChannel);
			currentAudioInAddress &= ~0x3;	//	Force DWORD alignment
			currentAudioInAddress += audioReadOffset;

			if (audioIsReset && mAudioOutLastAddress)
			{
				//	Now that the audio system has some samples to play, playback can be taken out of reset...
				mDevice.SetAudioOutputReset (mAudioSystem, false);
				audioIsReset = false;
			}

			if (currentAudioInAddress < mAudioInLastAddress)
			{
				//	Audio address has wrapped around the end of the buffer.
				//	Do the calculations and transfer from the last address to the end of the buffer...
				audioBytesCaptured 	= audioInWrapAddress - mAudioInLastAddress;

				mDevice.DMAReadAudio (mAudioSystem, mpHostAudioBuffer, mAudioInLastAddress, audioBytesCaptured);

				//	Transfer the new samples from the start of the buffer to the current address...
				mDevice.DMAReadAudio (mAudioSystem, &mpHostAudioBuffer [audioBytesCaptured / sizeof (uint32_t)],
										audioReadOffset, currentAudioInAddress - audioReadOffset);

				audioBytesCaptured += currentAudioInAddress - audioReadOffset;
			}
			else
			{
				audioBytesCaptured = currentAudioInAddress - mAudioInLastAddress;

				//	No wrap, so just perform a linear DMA from the buffer...
				mDevice.DMAReadAudio (mAudioSystem, mpHostAudioBuffer, mAudioInLastAddress, audioBytesCaptured);
			}

			mAudioInLastAddress = currentAudioInAddress;
		}	//	if mWithAudio

		//	Flip sense of the buffers again to refer to the buffers that the hardware isn't using (i.e. the off-screen buffers)...
		currentInFrame	^= 1;
		currentOutFrame	^= 1;

		//	DMA the new frame to system memory...
		mDevice.DMAReadFrame (currentInFrame, mpHostVideoBuffer, mVideoBufferSize);

		//	Determine which timecode value should be burned in to the video frame
		RP188_STRUCT	timecodeValue;
		if (mTimecodeIndex != NTV2_TCINDEX_LTC1 && mTimecodeIndex != NTV2_TCINDEX_LTC2 && InputSignalHasTimecode ())
		{
			//	Use the embedded input time code...
			mDevice.GetRP188Data (mInputChannel, 0, timecodeValue);
			CRP188	inputRP188Info	(timecodeValue);
			inputRP188Info.GetRP188Str (timeCodeString);
		}
		else if ((mTimecodeIndex == NTV2_TCINDEX_LTC1 || mTimecodeIndex == NTV2_TCINDEX_LTC2) && AnalogLTCInputHasTimecode ())
		{
			//	Use the analog input time code...
			mDevice.ReadAnalogLTCInput (mTimecodeIndex== NTV2_TCINDEX_LTC1 ? 0 : 1, timecodeValue);
			CRP188	analogRP188Info	(timecodeValue);
			analogRP188Info.GetRP188Str (timeCodeString);
		}
		else
		{
			//	Invent a timecode (based on the number of frames procesed)...
			const	NTV2FrameRate	ntv2FrameRate	(GetNTV2FrameRateFromVideoFormat (mVideoFormat));
			const	TimecodeFormat	tcFormat		(CNTV2DemoCommon::NTV2FrameRate2TimecodeFormat(ntv2FrameRate));
			const	CRP188			frameRP188Info	(mFramesProcessed, tcFormat);

			frameRP188Info.GetRP188Reg (timecodeValue);
			frameRP188Info.GetRP188Str (timeCodeString);
		}

		//	"Burn" the timecode into the host buffer while we have full access to it...
		mTCBurner.BurnTimeCode (reinterpret_cast <char *> (mpHostVideoBuffer), timeCodeString.c_str (), 80);

		//	Send the updated frame back to the board for display...
		mDevice.DMAWriteFrame (currentOutFrame, mpHostVideoBuffer, mVideoBufferSize);

		//	Write the output timecode...
		mDevice.SetRP188Data (mOutputChannel, currentOutFrame, timecodeValue);

		if (mWithAudio)
		{
			//	Calculate where the next audio samples should go in the buffer, taking wraparound into account...
			if ((mAudioOutLastAddress + audioBytesCaptured) > audioOutWrapAddress)
			{
				//	The audio will wrap. Transfer enough bytes to fill the buffer to the end...
				mDevice.DMAWriteAudio (mAudioSystem, mpHostAudioBuffer, mAudioOutLastAddress, audioOutWrapAddress - mAudioOutLastAddress);

				//	Now transfer the remaining bytes to the front of the buffer...
				mDevice.DMAWriteAudio (mAudioSystem, &mpHostAudioBuffer [(audioOutWrapAddress - mAudioOutLastAddress) / sizeof (uint32_t)],
									   0, audioBytesCaptured - (audioOutWrapAddress - mAudioOutLastAddress));

				mAudioOutLastAddress = audioBytesCaptured - (audioOutWrapAddress - mAudioOutLastAddress);
			}
			else
			{
				//	No wrap, so just do a linear DMA from the buffer...
				mDevice.DMAWriteAudio (mAudioSystem, mpHostAudioBuffer, mAudioOutLastAddress, audioBytesCaptured);

				mAudioOutLastAddress += audioBytesCaptured;
			}
		}	//	if mWithAudio

		//	Check for dropped frames by ensuring the hardware has not started to process
		//	the buffers that were just filled....
		uint32_t readBackIn;
		uint32_t readBackOut;
		mDevice.GetInputFrame	(mInputChannel,		readBackIn);
		mDevice.GetOutputFrame	(mOutputChannel,	readBackOut);

		if ((readBackIn == currentInFrame) || (readBackOut == currentOutFrame))
		{
			cerr	<< "## WARNING:  Drop detected:  current in " << currentInFrame << ", readback in " << readBackIn
					<< ", current out " << currentOutFrame << ", readback out " << readBackOut << endl;
			mFramesDropped++;
		}
		else
			mFramesProcessed++;

		//	Tell the hardware which buffers to start using at the beginning of the next frame...
		mDevice.SetInputFrame	(mInputChannel,  currentInFrame);
		mDevice.SetOutputFrame	(mOutputChannel, currentOutFrame);

	}	//	loop til quit signaled

}	//	ProcessFrames


//////////////////////////////////////////////


void NTV2LLBurn::GetStatus (ULWord & outFramesProcessed, ULWord & outFramesDropped)
{
	outFramesProcessed = mFramesProcessed;
	outFramesDropped = mFramesDropped;

}	//	GetACStatus


static ULWord GetRP188RegisterForInput (const NTV2InputSource inInputSource)
{
	switch (inInputSource)
	{
		case NTV2_INPUTSOURCE_SDI1:		return kRegRP188InOut1DBB;	break;	//	reg 29
		case NTV2_INPUTSOURCE_SDI2:		return kRegRP188InOut2DBB;	break;	//	reg 64
		case NTV2_INPUTSOURCE_SDI3:		return kRegRP188InOut3DBB;	break;	//	reg 268
		case NTV2_INPUTSOURCE_SDI4:		return kRegRP188InOut4DBB;	break;	//	reg 273
		case NTV2_INPUTSOURCE_SDI5:		return kRegRP188InOut5DBB;	break;	//	reg 342
		case NTV2_INPUTSOURCE_SDI6:		return kRegRP188InOut6DBB;	break;	//	reg 418
		case NTV2_INPUTSOURCE_SDI7:		return kRegRP188InOut7DBB;	break;	//	reg 427
		case NTV2_INPUTSOURCE_SDI8:		return kRegRP188InOut8DBB;	break;	//	reg 436
		default:						return 0;					break;
	}	//	switch on input source

}	//	GetRP188RegisterForInput


bool NTV2LLBurn::InputSignalHasTimecode (void)
{
	bool			result		(false);
	const ULWord	regNum		(GetRP188RegisterForInput (mInputSource));
	ULWord			regValue	(0);

	//
	//	Bit 16 of the RP188 DBB register will be set if there is timecode embedded in the input signal...
	//
	if (regNum && mDevice.ReadRegister (regNum, &regValue) && regValue & BIT(16))
		result = true;

	return result;

}	//	InputSignalHasTimecode


bool NTV2LLBurn::AnalogLTCInputHasTimecode (void)
{
	ULWord	regMask		(kRegMaskLTC1InPresent);
	ULWord	regValue	(0);
	switch (mTimecodeIndex)
	{
		case NTV2_TCINDEX_LTC1:										break;
		case NTV2_TCINDEX_LTC2:	regMask = kRegMaskLTC2InPresent;	break;
		default:				return false;						break;
	}
	mDevice.ReadRegister (kRegLTCStatusControl, &regValue, regMask);
	return regValue ? true : false;

}	//	AnalogLTCInputHasTimecode
