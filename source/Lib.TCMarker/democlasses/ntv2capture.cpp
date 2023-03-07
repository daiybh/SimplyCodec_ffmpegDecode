/**
	@file		ntv2capture.cpp
	@brief		Implementation of NTV2Capture class.
	@copyright	Copyright (C) 2012-2016 AJA Video Systems, Inc.  All rights reserved.
**/

#include "ntv2capture.h"
#include "ntv2utils.h"
#include "ntv2devicefeatures.h"
#include "ajastuff/system/process.h"
#include "ajastuff/system/systemtime.h"

#include <iterator>     // std::front_inserter

#define NTV2_AUDIOSIZE_MAX	(401 * 1024)
#define NTV2_ANCSIZE_MAX	(0x2000)


static const ULWord	kAppSignature	AJA_FOURCC ('D','E','M','O');


NTV2Capture::NTV2Capture (const string					inDeviceSpecifier,
						  const bool					withAudio,
						  const NTV2Channel				channel,
						  const NTV2FrameBufferFormat	pixelFormat,
						  const bool					inLevelConversion,
						  const bool					inDoMultiFormat,
						  const bool					inWithAnc)

	:	mConsumerThread		(NULL),
		mProducerThread		(NULL),
		mLock				(new AJALock (CNTV2DemoCommon::GetGlobalMutexName ())),
		mDeviceID			(DEVICE_ID_NOTFOUND),
		mDeviceSpecifier	(inDeviceSpecifier),
		mWithAudio			(withAudio),
		mInputChannel		(channel),
		mInputSource		(::NTV2ChannelToInputSource (mInputChannel)),
		mVideoFormat		(NTV2_FORMAT_UNKNOWN),
		mPixelFormat		(pixelFormat),
		mSavedTaskMode		(NTV2_DISABLE_TASKS),
		mVancEnabled		(false),
		mWideVanc			(false),
		mAudioSystem		(NTV2_AUDIOSYSTEM_1),
		mDoLevelConversion	(inLevelConversion),
		mDoMultiFormat		(inDoMultiFormat),
		mGlobalQuit			(false),
		mWithAnc			(inWithAnc),
		mVideoBufferSize	(0),
		mAudioBufferSize	(0)
{
	::memset (mAVHostBuffer, 0x0, sizeof (mAVHostBuffer));
	mPixelFormat = NTV2_FBF_10BIT_YCBCR;
	mVideoFormat = NTV2_FORMAT_1080i_5000;

	mVideoBufferSize = ::GetVideoWriteSize(mVideoFormat, mPixelFormat, mVancEnabled, mWideVanc);
	printf("NTV2_FORMAT_1080i_5000-->%d\n", mVideoBufferSize);
	mVideoFormat = NTV2_FORMAT_720p_5994;
	mVideoBufferSize = ::GetVideoWriteSize(mVideoFormat, mPixelFormat, mVancEnabled, mWideVanc);

	printf("NTV2_FORMAT_720p_5994-->%d\n", mVideoBufferSize);
}	//	constructor


NTV2Capture::~NTV2Capture ()
{
	//	Stop my capture and consumer threads, then destroy them...
	Quit ();

	delete mConsumerThread;
	mConsumerThread = NULL;

	delete mProducerThread;
	mProducerThread = NULL;

	//	Unsubscribe from input vertical event...
	mDevice.UnsubscribeInputVerticalEvent (mInputChannel);

	//	Free all my buffers...
	for (unsigned bufferNdx = 0; bufferNdx < CIRCULAR_BUFFER_SIZE; bufferNdx++)
	{
		if (mAVHostBuffer[bufferNdx].fVideoBuffer)
		{
			delete mAVHostBuffer[bufferNdx].fVideoBuffer;
			mAVHostBuffer[bufferNdx].fVideoBuffer = NULL;
		}
		if (mAVHostBuffer[bufferNdx].fAudioBuffer)
		{
			delete mAVHostBuffer[bufferNdx].fAudioBuffer;
			mAVHostBuffer[bufferNdx].fAudioBuffer = NULL;
		}
		if (mAVHostBuffer[bufferNdx].fAncBuffer)
		{
			delete mAVHostBuffer[bufferNdx].fAncBuffer;
			mAVHostBuffer[bufferNdx].fAncBuffer = NULL;
		}
	}	//	for each buffer in the ring

}	//	destructor


void NTV2Capture::Quit (void)
{
	//	Set the global 'quit' flag, and wait for the threads to go inactive...
	mGlobalQuit = true;

	if (mConsumerThread)
		while (mConsumerThread->Active ())
			AJATime::Sleep (10);

	if (mProducerThread)
		while (mProducerThread->Active ())
			AJATime::Sleep (10);

	if (!mDoMultiFormat)
	{
		mDevice.ReleaseStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ()));
		mDevice.SetEveryFrameServices (mSavedTaskMode);		//	Restore prior task mode
	}

}	//	Quit


AJAStatus NTV2Capture::Init (void)
{
	AJAStatus	status	(AJA_STATUS_SUCCESS);

	//	Open the device...
	if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, mDevice))
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return AJA_STATUS_OPEN;}

	if (!mDevice.IsDeviceReady ())
		{cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not ready" << endl;  return AJA_STATUS_INITIALIZE;}

	if (!mDoMultiFormat)
	{
		if (!mDevice.AcquireStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ())))
			return AJA_STATUS_BUSY;							//	Another app is using the device
		mDevice.GetEveryFrameServices (&mSavedTaskMode);	//	Save the current state before we change it
	}
	mDevice.SetEveryFrameServices (NTV2_OEM_TASKS);			//	Since this is an OEM demo, use the OEM service level

	mDeviceID = mDevice.GetDeviceID ();						//	Keep the device ID handy, as it's used frequently

	//	Sometimes other applications disable some or all of the frame buffers, so turn on ours now...
	mDevice.EnableChannel (mInputChannel);
	if (::NTV2DeviceCanDoMultiFormat (mDeviceID))
		mDevice.SetMultiFormatMode (mDoMultiFormat);

	//	Set up the video and audio...
	status = SetupVideo ();
	if (AJA_FAILURE (status))
		return status;

	status = SetupAudio ();
	if (AJA_FAILURE (status))
		return status;

	//	Set up the circular buffers, the device signal routing, and both playout and capture AutoCirculate...
	SetupHostBuffers ();
	RouteInputSignal ();
	SetupInputAutoCirculate ();

	return AJA_STATUS_SUCCESS;

}	//	Init


AJAStatus NTV2Capture::SetupVideo (void)
{
	//	Enable and subscribe to the interrupts for the channel to be used...
	mDevice.EnableInputInterrupt(mInputChannel);
	mDevice.SubscribeInputVerticalEvent(mInputChannel);
	mDevice.SubscribeOutputVerticalEvent(NTV2_CHANNEL1);

	//	Disable SDI output from the SDI input being used,
	//	but only if the device supports bi-directional SDI,
	//	and only if the input being used is an SDI input...
	if (::NTV2DeviceHasBiDirectionalSDI(mDeviceID))
		mDevice.SetSDITransmitEnable(mInputChannel, false);

	//	Wait for four verticals to let the receiver lock...
	mDevice.WaitForOutputVerticalInterrupt (NTV2_CHANNEL1, 4);

	//	Set the video format to match the incomming video format.
	//	Does the device support the desired input source?

	//	If the device supports bi-directional SDI and the
	//	requested input is SDI, ensure the SDI direction
	//	is configured for input...
	if (::NTV2DeviceHasBiDirectionalSDI (mDeviceID) && NTV2_INPUT_SOURCE_IS_SDI (mInputSource))
	{
		mDevice.SetSDITransmitEnable (mInputChannel, false);

		//	Give the input circuit some time (~10 frames) to lock onto the input signal...
		mDevice.WaitForInputVerticalInterrupt (mInputChannel, 10);
	}

	//	Determine the input video signal format...
	mVideoFormat = mDevice.GetInputVideoFormat (mInputSource);
	if (mVideoFormat == NTV2_FORMAT_UNKNOWN)
	{
		cerr << "## ERROR:  No input signal or unknown format" << endl;
		return AJA_STATUS_NOINPUT;	//	Sorry, can't handle this format
	}

	//	Set the device video format to whatever we detected at the input...
	mDevice.SetReference (::NTV2InputSourceToReferenceSource (mInputSource));
	mDevice.SetVideoFormat (mVideoFormat, false, false, mInputChannel);

	//	Set the frame buffer pixel format for all the channels on the device
	//	(assuming it supports that pixel format -- otherwise default to 8-bit YCbCr)...
	if (!::NTV2DeviceCanDoFrameBufferFormat (mDeviceID, mPixelFormat))
		mPixelFormat = NTV2_FBF_8BIT_YCBCR;

	mDevice.SetFrameBufferFormat (mInputChannel, mPixelFormat);

	return AJA_STATUS_SUCCESS;

}	//	SetupVideo


AJAStatus NTV2Capture::SetupAudio (void)
{
	//	In multiformat mode, base the audio system on the channel...
	if (mDoMultiFormat && ::NTV2DeviceGetNumAudioSystems (mDeviceID) > 1 && UWord (mInputChannel) < ::NTV2DeviceGetNumAudioSystems (mDeviceID))
		mAudioSystem = ::NTV2ChannelToAudioSystem (mInputChannel);

	//	Have the audio system capture audio from the designated device input (i.e., ch1 uses SDIIn1, ch2 uses SDIIn2, etc.)...
	mDevice.SetAudioSystemInputSource (mAudioSystem, NTV2_AUDIO_EMBEDDED, ::NTV2ChannelToEmbeddedAudioInput (mInputChannel));

	mDevice.SetNumberAudioChannels (::NTV2DeviceGetMaxAudioChannels (mDeviceID), mAudioSystem);
	mDevice.SetAudioRate (NTV2_AUDIO_48K, mAudioSystem);

	//	The on-device audio buffer should be 4MB to work best across all devices & platforms...
	mDevice.SetAudioBufferSize (NTV2_AUDIO_BUFFER_BIG, mAudioSystem);

	return AJA_STATUS_SUCCESS;

}	//	SetupAudio


void NTV2Capture::SetupHostBuffers (void)
{
	//	Let my circular buffer know when it's time to quit...
	mAVCircularBuffer.SetAbortFlag (&mGlobalQuit);

	mVancEnabled = false;
	mWideVanc = false;
	mDevice.GetEnableVANCData (&mVancEnabled, &mWideVanc);
	mVideoBufferSize = ::GetVideoWriteSize (mVideoFormat, mPixelFormat, mVancEnabled, mWideVanc);
	mAudioBufferSize = NTV2_AUDIOSIZE_MAX;
	mAncBufferSize = NTV2_ANCSIZE_MAX;
	m_DataSize = mVideoBufferSize;
	//	Allocate and add each in-host AVDataBuffer to my circular buffer member variable...
	for (unsigned bufferNdx = 0; bufferNdx < CIRCULAR_BUFFER_SIZE; bufferNdx++ )
	{
		mAVHostBuffer [bufferNdx].fVideoBuffer		= reinterpret_cast <uint32_t *> (new uint8_t [mVideoBufferSize]);
		mAVHostBuffer [bufferNdx].fVideoBufferSize	= mVideoBufferSize;
		mAVHostBuffer [bufferNdx].fAudioBuffer		= mWithAudio ? reinterpret_cast <uint32_t *> (new uint8_t [mAudioBufferSize]) : 0;
		mAVHostBuffer [bufferNdx].fAudioBufferSize	= mWithAudio ? mAudioBufferSize : 0;
		mAVHostBuffer [bufferNdx].fAncBuffer		= mWithAnc ? reinterpret_cast <uint32_t *> (new uint8_t [mAncBufferSize]) : 0;
		mAVHostBuffer [bufferNdx].fAncBufferSize	= mAncBufferSize;
		mAVCircularBuffer.Add (& mAVHostBuffer [bufferNdx]);
	}	//	for each AVDataBuffer

}	//	SetupHostBuffers


void NTV2Capture::RouteInputSignal (void)
{
	//	For this simple example, tie the user-selected input to frame buffer 1.
	//	Is this user-selected input supported on the device?
	if (!::NTV2DeviceCanDoInputSource (mDeviceID, mInputSource))
		mInputSource = NTV2_INPUTSOURCE_SDI1;

	const bool						isRGB					(::IsRGBFormat (mPixelFormat));
	const NTV2OutputCrosspointID	sdiInputWidgetOutputXpt	(::GetSDIInputOutputXptFromChannel (mInputChannel));
	const NTV2InputCrosspointID		frameBufferInputXpt		(::GetFrameBufferInputXptFromChannel (mInputChannel));
	const NTV2InputCrosspointID		cscWidgetVideoInputXpt	(::GetCSCInputXptFromChannel (mInputChannel));
	const NTV2OutputCrosspointID	cscWidgetRGBOutputXpt	(::GetCSCOutputXptFromChannel (mInputChannel, /*inIsKey*/ false, /*inIsRGB*/ true));

	if (!mDoMultiFormat)
		mDevice.ClearRouting ();

	if (isRGB)
	{
		mDevice.Connect (frameBufferInputXpt,		cscWidgetRGBOutputXpt);		//	Frame store input to CSC widget's RGB output
		mDevice.Connect (cscWidgetVideoInputXpt,	sdiInputWidgetOutputXpt);	//	CSC widget's YUV input to SDI-In widget's output
	}
	else
		mDevice.Connect (frameBufferInputXpt,		sdiInputWidgetOutputXpt);	//	Frame store input to SDI-In widget's output

}	//	RouteInputSignal


void NTV2Capture::SetupInputAutoCirculate (void)
{
	//	Tell capture AutoCirculate to use 7 frame buffers on the device...
	mDevice.AutoCirculateStop (mInputChannel);
	{
		AJAAutoLock	autoLock (mLock);	//	Avoid AutoCirculate buffer collisions
		mDevice.AutoCirculateInitForInput (mInputChannel,	7,	//	Number of frames to circulate
											mWithAudio ? mAudioSystem : NTV2_AUDIOSYSTEM_INVALID,	//	Which audio system (if any)?
											AUTOCIRCULATE_WITH_RP188);								//	Include timecode
	}
	mInputTransfer.acFrameBufferFormat = mPixelFormat;		//	Specify the FBF

}	//	SetupInputAutoCirculate


AJAStatus NTV2Capture::Run ()
{
	//	Start the playout and capture threads...
	StartConsumerThread ();
	StartProducerThread ();
	return AJA_STATUS_SUCCESS;

}	//	Run


AJAStatus NTV2Capture::start()
{
	mProducerThread->Start();
	return AJA_STATUS_SUCCESS;
}

//////////////////////////////////////////////

//	This is where we will start the consumer thread
void NTV2Capture::StartConsumerThread (void)
{
	//	Create and start the consumer thread...
	mConsumerThread = new AJAThread ();
	mConsumerThread->Attach (ConsumerThreadStatic, this);
	mConsumerThread->SetPriority (AJA_ThreadPriority_High);
	mConsumerThread->Start ();

}	//	StartConsumerThread


//	The consumer thread function
void NTV2Capture::ConsumerThreadStatic (AJAThread * pThread, void * pContext)		//	static
{
	(void) pThread;

	//	Grab the NTV2Capture instance pointer from the pContext parameter,
	//	then call its ConsumeFrames method...
	NTV2Capture *	pApp	(reinterpret_cast <NTV2Capture *> (pContext));
	pApp->ConsumeFrames ();

}	//	ConsumerThreadStatic
char*GetPixelForamtString(NTV2FrameBufferFormat mPixelFormat)
{
	switch (mPixelFormat)
	{
	case NTV2_FBF_10BIT_YCBCR:
		return "NTV2_FBF_10BIT_YCBCR";
	case NTV2_FBF_8BIT_YCBCR:
		return "NTV2_FBF_8BIT_YCBCR";
	default:
		return "NTV2FrameBufferFormat";
		break;
	}
}
char* GetNTV2VideoFormatString(NTV2VideoFormat videoFormat)
{
	return "NTV2VideoFormat";
}
void NTV2Capture::ConsumeFrames (void)
{	
	FILE *fpV = nullptr,*fpA=nullptr;
	char filePathV[512], filePathA[512];
	sprintf_s(filePathV, "%s\\capture_device(%s)_input(%d)_format(%d_%dx%d)_pixel(%s).yuv",m_pSavepath, mDeviceSpecifier.c_str(),mInputChannel,
		mVideoFormat,
		GetDisplayWidth(mVideoFormat),
		GetDisplayHeight(mVideoFormat),
		GetPixelForamtString(mPixelFormat));

	sprintf_s(filePathA, "%s\\capture_device(%s)_input(%d)_format(%d_%dx%d)_pixel(%s).pcm", m_pSavepath, mDeviceSpecifier.c_str(), mInputChannel,
		mVideoFormat,
		GetDisplayWidth(mVideoFormat),
		GetDisplayHeight(mVideoFormat),
		GetPixelForamtString(mPixelFormat));

	if (fpV == nullptr)
		fopen_s(&fpV, filePathV, "wb");

	if (fpA == nullptr)
		fopen_s(&fpA, filePathA, "wb");

	ULWord ulSize = GetVideoActiveSize(mVideoFormat, mPixelFormat, mVancEnabled, mWideVanc);
	//m_DataSize = ulSize;
	while (!mGlobalQuit)
	{
		//	Wait for the next frame to become ready to "consume"...
		AVDataBuffer *	pFrameData	(mAVCircularBuffer.StartConsumeNextBuffer ());
		if (pFrameData)
		{
			//	Do something useful with the frame data...
			//	. . .		. . .		. . .		. . .
			//		. . .		. . .		. . .		. . .
			//			. . .		. . .		. . .		. . .
			if (fpV == nullptr)			
				fopen_s(&fpV, filePathV, "wb");
			if (fpA == nullptr)
				fopen_s(&fpA, filePathA, "wb");

			if (fpV)
			{
				fwrite(pFrameData->fVideoBuffer, ulSize, 1, fpV);
				if(fpA)
					fwrite(pFrameData->fAudioBuffer, pFrameData->fAudioBufferSize, 1, fpA);
			}
			//	Now release and recycle the buffer...
			mAVCircularBuffer.EndConsumeNextBuffer ();
		}
	}	//	loop til quit signaled
	if (fpV)fclose(fpV);
	if (fpA)fclose(fpA);
}	//	ConsumeFrames


//////////////////////////////////////////////



//////////////////////////////////////////////

//	This is where we start the capture thread
void NTV2Capture::StartProducerThread (void)
{
	//	Create and start the capture thread...
	mProducerThread = new AJAThread ();
	mProducerThread->Attach (ProducerThreadStatic, this);
	mProducerThread->SetPriority (AJA_ThreadPriority_High);
	

}	//	StartProducerThread


//	The capture thread function
void NTV2Capture::ProducerThreadStatic (AJAThread * pThread, void * pContext)		//	static
{
	(void) pThread;

	//	Grab the NTV2Capture instance pointer from the pContext parameter,
	//	then call its CaptureFrames method...
	NTV2Capture *	pApp	(reinterpret_cast <NTV2Capture *> (pContext));
	pApp->CaptureFrames ();

}	//	ProducerThreadStatic


void NTV2Capture::CaptureFrames (void)
{
	NTV2AudioChannelPairs	nonPcmPairs, oldNonPcmPairs;

	//	Start AutoCirculate running...
	mDevice.AutoCirculateStart (mInputChannel);

	while (!mGlobalQuit)
	{
		AUTOCIRCULATE_STATUS	acStatus;
		mDevice.AutoCirculateGetStatus (mInputChannel, acStatus);

		if (acStatus.IsRunning () && acStatus.HasAvailableInputFrame ())
		{
			//	At this point, there's at least one fully-formed frame available in the device's
			//	frame buffer to transfer to the host. Reserve an AVDataBuffer to "produce", and
			//	use it in the next transfer from the device...
			AVDataBuffer *	captureData	(mAVCircularBuffer.StartProduceNextBuffer ());

			mInputTransfer.SetBuffers (captureData->fVideoBuffer, captureData->fVideoBufferSize,
										captureData->fAudioBuffer, captureData->fAudioBufferSize,
										captureData->fAncBuffer, captureData->fAncBufferSize);

			//	Do the transfer from the device into our host AVDataBuffer...
			mDevice.AutoCirculateTransfer (mInputChannel, mInputTransfer);
			captureData->fAudioBufferSize = mInputTransfer.acTransferStatus.acAudioTransferSize;

			NTV2SDIInStatistics	sdiStats;
			mDevice.ReadSDIStatistics (sdiStats);

			//	"Capture" timecode into the host AVDataBuffer while we have full access to it...
			NTV2_RP188	timecode;
			mInputTransfer.GetInputTimeCode (timecode);
			captureData->fRP188Data = timecode;

			//	Look for PCM/NonPCM changes in the audio stream...
			if (mDevice.GetInputAudioChannelPairsWithoutPCM (mInputChannel, nonPcmPairs))
			{
				NTV2AudioChannelPairs	becomingNonPCM, becomingPCM;
				set_difference (oldNonPcmPairs.begin(), oldNonPcmPairs.end(), nonPcmPairs.begin(), nonPcmPairs.end(),  inserter (becomingPCM, becomingPCM.begin()));
				set_difference (nonPcmPairs.begin(), nonPcmPairs.end(),  oldNonPcmPairs.begin(), oldNonPcmPairs.end(),  inserter (becomingNonPCM, becomingNonPCM.begin()));
				if (!becomingNonPCM.empty () || !becomingPCM.empty ())
					cerr << "## NOTE:  Audio channel pair(s) '" << becomingNonPCM << "' now non-PCM, '" << becomingPCM << "' now PCM" << endl;
				oldNonPcmPairs = nonPcmPairs;
			}

			//	Signal that we're done "producing" the frame, making it available for future "consumption"...
			mAVCircularBuffer.EndProduceNextBuffer ();
		}	//	if A/C running and frame(s) are available for transfer
		else
		{
			//	Either AutoCirculate is not running, or there were no frames available on the device to transfer.
			//	Rather than waste CPU cycles spinning, waiting until a frame becomes available, it's far more
			//	efficient to wait for the next input vertical interrupt event to get signaled...
			mDevice.WaitForInputVerticalInterrupt (mInputChannel);
		}
	}	//	loop til quit signaled

	//	Stop AutoCirculate...
	mDevice.AutoCirculateStop (mInputChannel);

}	//	CaptureFrames


//////////////////////////////////////////////


void NTV2Capture::GetACStatus (ULWord & outGoodFrames, ULWord & outDroppedFrames, ULWord & outBufferLevel)
{
	AUTOCIRCULATE_STATUS	status;
	mDevice.AutoCirculateGetStatus (mInputChannel, status);
	outGoodFrames = status.acFramesProcessed;
	outDroppedFrames = status.acFramesDropped;
	outBufferLevel = status.acBufferLevel;
}
