/**
	@file		ntv2encodehevcvifac.cpp
	@brief		Implementation of NTV2EncodeHEVCVifAc class.
	@copyright	Copyright (C) 2015-2016 AJA Video Systems, Inc.  All rights reserved.
**/

#include <stdio.h>

#include "ntv2encodehevcvifac.h"
#include "ntv2utils.h"
#include "ntv2devicefeatures.h"
#include "ajastuff/system/process.h"
#include "ajastuff/system/systemtime.h"

#define NTV2_AUDIOSIZE_MAX		(401 * 1024)

static const ULWord	kAppSignature	AJA_FOURCC ('D','E','M','O');


NTV2EncodeHEVCVifAc::NTV2EncodeHEVCVifAc (const string				  inDeviceSpecifier,
                                          const NTV2Channel			  inChannel,
                                          const M31VideoPreset        inPreset,
                                          const NTV2FrameBufferFormat inPixelFormat,
                                          const bool                  inQuadMode,
                                          const uint32_t              inAudioChannels,
                                          const bool                  inTimeCodeBurn,
                                          const bool                  inInfoData,
                                          const uint32_t              inMaxFrames)

:	mVideoInputThread		(NULL),
	mVideoProcessThread		(NULL),
	mCodecRawThread			(NULL),
	mCodecHevcThread		(NULL),
    mVideoFileThread 		(NULL),
    mAudioFileThread 		(NULL),
    mM31					(NULL),
    mHevcCommon             (NULL),
	mDeviceID				(DEVICE_ID_NOTFOUND),
	mDeviceSpecifier		(inDeviceSpecifier),
    mWithAudio				(inAudioChannels != 0),
    mInputChannel			(inChannel),
    mEncodeChannel          (M31_CH0),
    mPreset					(inPreset),
	mInputSource			(NTV2_INPUTSOURCE_SDI1),
    mVideoFormat			(NTV2_MAX_NUM_VIDEO_FORMATS),
    mPixelFormat			(inPixelFormat),
    mQuad					(inQuadMode),
	mInterlaced				(false),
    mMultiStream			(false),
    mWithInfo               (inInfoData),
    mWithAnc                (inTimeCodeBurn),
	mAudioSystem			(NTV2_AUDIOSYSTEM_1),
	mSavedTaskMode			(NTV2_STANDARD_TASKS),
    mNumAudioChannels       (0),
    mFileAudioChannels      (inAudioChannels),
    mMaxFrames              (inMaxFrames),
	mLastFrame				(false),
	mLastFrameInput			(false),
	mLastFrameRaw			(false),
	mLastFrameHevc			(false),
    mLastFrameVideo			(false),
    mLastFrameAudio			(false),
    mGlobalQuit				(false),
	mVideoInputFrameCount	(0),		
	mVideoProcessFrameCount	(0),		
	mCodecRawFrameCount		(0),
	mCodecHevcFrameCount	(0),
    mVideoFileFrameCount 	(0),
    mAudioFileFrameCount 	(0)
{
    ::memset (mVideoInputBuffer, 0x0, sizeof (mVideoInputBuffer));
    ::memset (mVideoRawBuffer, 0x0, sizeof (mVideoRawBuffer));
    ::memset (mVideoHevcBuffer, 0x0, sizeof (mVideoHevcBuffer));
    ::memset (mAudioInputBuffer, 0x0, sizeof (mAudioInputBuffer));
    ::memset (&mInputTransferStruct, 0, sizeof (mInputTransferStruct));
	::memset (&mInputTransferStatusStruct, 0, sizeof (mInputTransferStatusStruct));
    ::memset (&mOutputTransferStruct, 0, sizeof (mInputTransferStruct));
    ::memset (&mOutputTransferStatusStruct, 0, sizeof (mInputTransferStatusStruct));

}	//	constructor


NTV2EncodeHEVCVifAc::~NTV2EncodeHEVCVifAc ()
{
	//	Stop my capture and consumer threads, then destroy them...
	Quit ();

	if (mVideoInputThread != NULL)
	{
		delete mVideoInputThread;
		mVideoInputThread = NULL;
	}

	if (mVideoProcessThread != NULL)
	{
		delete mVideoProcessThread;
		mVideoProcessThread = NULL;
	}

	if (mCodecRawThread != NULL)
	{
		delete mCodecRawThread;
		mCodecRawThread = NULL;
	}

	if (mCodecHevcThread != NULL)
	{
		delete mCodecHevcThread;
		mCodecHevcThread = NULL;
	}

    if (mVideoFileThread != NULL)
	{
        delete mVideoFileThread;
        mVideoFileThread = NULL;
	}

    if (mAudioFileThread != NULL)
    {
        delete mAudioFileThread;
        mAudioFileThread = NULL;
    }

    if (mM31 != NULL)
	{
		delete mM31;
		mM31 = NULL;
	}
    
    if (mHevcCommon != NULL)
    {
        delete mHevcCommon;
        mHevcCommon = NULL;
    }
	
	// unsubscribe from input vertical event...
	mDevice.UnsubscribeInputVerticalEvent (mInputChannel);

	// free all my buffers...
	for (unsigned bufferNdx = 0; bufferNdx < VIDEO_RING_SIZE; bufferNdx++)
	{
        if (mVideoInputBuffer[bufferNdx].pVideoBuffer)
		{
            delete [] mVideoInputBuffer[bufferNdx].pVideoBuffer;
            mVideoInputBuffer[bufferNdx].pVideoBuffer = NULL;
		}
		if (mVideoInputBuffer[bufferNdx].pInfoBuffer)
		{
		 	delete [] mVideoInputBuffer[bufferNdx].pInfoBuffer;
			mVideoInputBuffer[bufferNdx].pInfoBuffer = NULL;
		}
        if (mVideoInputBuffer[bufferNdx].pAudioBuffer)
		{
            delete [] mVideoInputBuffer[bufferNdx].pAudioBuffer;
            mVideoInputBuffer[bufferNdx].pAudioBuffer = NULL;
		}

        if (mVideoRawBuffer[bufferNdx].pVideoBuffer)
		{
            delete [] mVideoRawBuffer[bufferNdx].pVideoBuffer;
            mVideoRawBuffer[bufferNdx].pVideoBuffer = NULL;
		}
		if (mVideoRawBuffer[bufferNdx].pInfoBuffer)
		{
		 	delete [] mVideoRawBuffer[bufferNdx].pInfoBuffer;
			mVideoRawBuffer[bufferNdx].pInfoBuffer = NULL;
		}
        if (mVideoRawBuffer[bufferNdx].pAudioBuffer)
		{
            delete [] mVideoRawBuffer[bufferNdx].pAudioBuffer;
            mVideoRawBuffer[bufferNdx].pAudioBuffer = NULL;
		}

        if (mVideoHevcBuffer[bufferNdx].pVideoBuffer)
		{
            delete [] mVideoHevcBuffer[bufferNdx].pVideoBuffer;
            mVideoHevcBuffer[bufferNdx].pVideoBuffer = NULL;
		}
		if (mVideoHevcBuffer[bufferNdx].pInfoBuffer)
		{
		 	delete [] mVideoHevcBuffer[bufferNdx].pInfoBuffer;
			mVideoHevcBuffer[bufferNdx].pInfoBuffer = NULL;
		}
        if (mVideoHevcBuffer[bufferNdx].pAudioBuffer)
		{
            delete [] mVideoHevcBuffer[bufferNdx].pAudioBuffer;
            mVideoHevcBuffer[bufferNdx].pAudioBuffer = NULL;
		}
    }

    if (mWithAudio)
	{
		for (unsigned bufferNdx = 0; bufferNdx < AUDIO_RING_SIZE; bufferNdx++)
		{
		    if (mAudioInputBuffer[bufferNdx].pVideoBuffer)
		    {
		        delete [] mAudioInputBuffer[bufferNdx].pVideoBuffer;
		        mAudioInputBuffer[bufferNdx].pVideoBuffer = NULL;
		    }
		    if (mAudioInputBuffer[bufferNdx].pInfoBuffer)
		    {
		        delete [] mAudioInputBuffer[bufferNdx].pInfoBuffer;
		        mAudioInputBuffer[bufferNdx].pInfoBuffer = NULL;
		    }
		    if (mAudioInputBuffer[bufferNdx].pAudioBuffer)
		    {
		        delete [] mAudioInputBuffer[bufferNdx].pAudioBuffer;
		        mAudioInputBuffer[bufferNdx].pAudioBuffer = NULL;
		    }
		}
	}

} // destructor


void NTV2EncodeHEVCVifAc::Quit (void)
{
    if (mM31 && !mLastFrame && !mGlobalQuit)
	{
		//	Set the last frame flag to start the quit process
		mLastFrame = true;

		//	Wait for the last frame to be written to disk
		int i;
		int timeout = 300;
		for (i = 0; i < timeout; i++)
		{
            if (mLastFrameVideo && (!mWithAudio || mLastFrameAudio)) break;
			AJATime::Sleep (10);
		}
		if (i == timeout)
			{ cerr << "## ERROR:  Wait for last frame timeout" << endl; }

		//	Stop the encoder stream
        if (!mM31->ChangeEHState(Hevc_EhState_ReadyToStop, mEncodeChannel))
			{ cerr << "## ERROR:  ChangeEHState ready to stop failed" << endl; }

        if (!mM31->ChangeEHState(Hevc_EhState_Stop, mEncodeChannel))
			{ cerr << "## ERROR:  ChangeEHState stop failed" << endl; }

		// stop the video input stream
        if (!mM31->ChangeVInState(Hevc_VinState_Stop, mEncodeChannel))
			{ cerr << "## ERROR:  ChangeVInState stop failed" << endl; }

        if(!mMultiStream)
        {
            //	Now go to the init state
            if (!mM31->ChangeMainState(Hevc_MainState_Init, Hevc_EncodeMode_Single))
                { cerr << "## ERROR:  ChangeMainState to init failed" << endl; }
        }
	}

	//	Stop the worker threads
	mGlobalQuit = true;

    if (mVideoInputThread)
        while (mVideoInputThread->Active ())
            AJATime::Sleep (10);

    if (mVideoProcessThread)
        while (mVideoProcessThread->Active ())
            AJATime::Sleep (10);

    if (mCodecRawThread)
        while (mCodecRawThread->Active ())
            AJATime::Sleep (10);

    if (mCodecHevcThread)
        while (mCodecHevcThread->Active ())
            AJATime::Sleep (10);

    if (mVideoFileThread)
        while (mVideoFileThread->Active ())
            AJATime::Sleep (10);

    if (mAudioFileThread)
        while (mAudioFileThread->Active ())
            AJATime::Sleep (10);

    //  Stop video capture
    mDevice.SetMode(mInputChannel, NTV2_MODE_DISPLAY, false);

    //  Release board
    if (!mMultiStream)
	{
		mDevice.ReleaseStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ()));
		mDevice.SetEveryFrameServices (mSavedTaskMode);		//	Restore prior task mode
	}

    //  Close output files
    mHevcCommon->CloseHevcFile ();

    if (mWithInfo)
    {
        mHevcCommon->CloseEncFile ();
    }

    if (mWithAudio)
    {
        mHevcCommon->CloseAiffFile ();
    }

}	//	Quit


AJAStatus NTV2EncodeHEVCVifAc::Init (void)
{
    char fileName[64];
    AJAStatus	status	(AJA_STATUS_SUCCESS);
    
    //	Open the device...
    if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, mDevice))
    { cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return AJA_STATUS_OPEN; }
    
    //  Grab board in a shared environment
    if (!mMultiStream)
    {
        if (!mDevice.AcquireStreamForApplication (kAppSignature, static_cast <uint32_t> (AJAProcess::GetPid ())))
            return AJA_STATUS_BUSY;							//	Another app is using the device
        mDevice.GetEveryFrameServices (&mSavedTaskMode);	//	Save the current state before we change it
    }
    mDevice.SetEveryFrameServices (NTV2_OEM_TASKS);			//	Since this is an OEM demo, use the OEM service level
    
    mDeviceID = mDevice.GetDeviceID ();						//	Keep the device ID handy, as it's used frequently
    
    // Make sure this device has an M31
    if (!NTV2DeviceHasHEVCM31 (mDeviceID))
	{
  	 	cerr << "## ERROR:  M31 not found" << endl;
        return AJA_STATUS_FAIL;
	}

    // Allocate our M31 helper class and our HEVC common class
    mM31 = new CNTV2m31 (&mDevice);
    mHevcCommon = new CNTV2DemoHevcCommon ();
    
    if ((mM31 == NULL) || (mHevcCommon == NULL))
    {
        return AJA_STATUS_FAIL;
    }
    
    //  Preset specification takes precedence
    if (mPreset < M31_NUMVIDEOPRESETS)
    {
        // This class only handles vif based presets so make sure they didn't pass in a file one
        if (!CNTV2m31::IsPresetVIF(mPreset))
            return AJA_STATUS_FAIL;

        //	Get NTV2 formats to match codec preset
		mVideoFormat = CNTV2m31::GetPresetVideoFormat(mPreset);
		mPixelFormat = CNTV2m31::GetPresetFrameBufferFormat(mPreset);
		mQuad = CNTV2m31::IsPresetUHD(mPreset);
		mInterlaced = CNTV2m31::IsPresetInterlaced(mPreset);
    }
    //  Otherwise use the pixel format and SDI input format
    else if (mPixelFormat >= NTV2_FBF_NUMFRAMEBUFFERFORMATS)
    {
         mPixelFormat = NTV2_FBF_8BIT_YCBCR;
    }

    //  Quad mode must be channel 1
    if (mQuad)
    {
        mInputChannel = NTV2_CHANNEL1;
        mOutputChannel = NTV2_CHANNEL5;
        mEncodeChannel = M31_CH0;
    }
    else
    {
        //  When input channel specified we are multistream
        switch (mInputChannel)
        {
        case NTV2_CHANNEL1: { mEncodeChannel = M31_CH0; mOutputChannel = NTV2_CHANNEL5; mMultiStream = true; break; }
        case NTV2_CHANNEL2: { mEncodeChannel = M31_CH1; mOutputChannel = NTV2_CHANNEL6; mMultiStream = true; break; }
        case NTV2_CHANNEL3: { mEncodeChannel = M31_CH2; mOutputChannel = NTV2_CHANNEL7; mMultiStream = true; break; }
        case NTV2_CHANNEL4: { mEncodeChannel = M31_CH3; mOutputChannel = NTV2_CHANNEL8; mMultiStream = true; break; }
        default: { mInputChannel = NTV2_CHANNEL1; mOutputChannel = NTV2_CHANNEL5; mEncodeChannel = M31_CH0; }
        }
    }

    //  When video format is unknown determine from SDI input
    if (mVideoFormat >= NTV2_MAX_NUM_VIDEO_FORMATS)
    {
        //  Get SDI input format
        status = mHevcCommon->DetermineInputFormat(mDevice.GetSDIInputVideoFormat(mInputChannel), mQuad, mVideoFormat);
        if (AJA_FAILURE(status))
            return status;

        //  Get codec preset for input format
        if(!CNTV2m31::ConvertVideoFormatToPreset(mVideoFormat, mPixelFormat, true, mPreset))
            return AJA_STATUS_FAIL;

		mQuad = CNTV2m31::IsPresetUHD(mPreset);
		mInterlaced = CNTV2m31::IsPresetInterlaced(mPreset);
    }

    //	Setup frame buffer
	status = SetupVideo ();
	if (AJA_FAILURE (status))
		return status;

	//	Route input signals to frame buffers
	RouteInputSignal ();

	//	Setup audio buffer
	status = SetupAudio ();
	if (AJA_FAILURE (status))
		return status;

	//	Setup to capture video/audio/anc input
    SetupAutoCirculate ();

	//	Setup codec
    status = mHevcCommon->SetupHEVC (mM31, mPreset, mEncodeChannel, mMultiStream, mWithInfo);
    if (AJA_FAILURE (status))
        return status;

	//	Setup the circular buffers
	SetupHostBuffers ();
  
	//	Create encoded video output file
    strcpy(fileName, "raw.hevc");
    if (mMultiStream)
    {
        sprintf(fileName, "raw_%d.hevc", (int)mInputChannel + 1);
    }

    status = mHevcCommon->CreateHevcFile (fileName, mMaxFrames);
    if (AJA_FAILURE (status))
        return status;

    if (mWithInfo)
    {
        //	Create encoded data output file
        strcpy(fileName, "raw.txt");
        if (mMultiStream)
        {
            sprintf(fileName, "raw_%d.txt", (int)mInputChannel + 1);
        }

        status = mHevcCommon->CreateEncFile (fileName, mMaxFrames);
        if (AJA_FAILURE (status))
            return status;
    }

    if (mWithAudio)
    {
        //	Create audio output file
        strcpy(fileName, "raw.aiff");
        if (mMultiStream)
        {
            sprintf(fileName, "raw_%d.aiff", (int)mInputChannel + 1);
        }

        status = mHevcCommon->CreateAiffFile (fileName, mFileAudioChannels, mMaxFrames, NTV2_AUDIOSIZE_MAX);
        if (AJA_FAILURE (status))
            return status;
    }

    return AJA_STATUS_SUCCESS;

}	//	Init


M31VideoPreset	NTV2EncodeHEVCVifAc::GetCodecPreset (void)
{
    return mPreset;
}

    
AJAStatus NTV2EncodeHEVCVifAc::SetupVideo (void)
{
	//	Setup frame buffer
	if (mQuad)
	{
		if (mInputChannel != NTV2_CHANNEL1)
			return AJA_STATUS_FAIL;

		//	Disable multiformat
		if (::NTV2DeviceCanDoMultiFormat (mDeviceID))
			mDevice.SetMultiFormatMode (false);

		//	Set the board video format
		mDevice.SetVideoFormat (mVideoFormat, false, false, NTV2_CHANNEL1);

		//	Set frame buffer format
		mDevice.SetFrameBufferFormat (NTV2_CHANNEL1, mPixelFormat);
		mDevice.SetFrameBufferFormat (NTV2_CHANNEL2, mPixelFormat);
		mDevice.SetFrameBufferFormat (NTV2_CHANNEL3, mPixelFormat);
		mDevice.SetFrameBufferFormat (NTV2_CHANNEL4, mPixelFormat);
        mDevice.SetFrameBufferFormat (NTV2_CHANNEL5, mPixelFormat);
        mDevice.SetFrameBufferFormat (NTV2_CHANNEL6, mPixelFormat);
        mDevice.SetFrameBufferFormat (NTV2_CHANNEL7, mPixelFormat);
        mDevice.SetFrameBufferFormat (NTV2_CHANNEL8, mPixelFormat);

		//	Set catpure mode
		mDevice.SetMode (NTV2_CHANNEL1, NTV2_MODE_CAPTURE, false);
		mDevice.SetMode (NTV2_CHANNEL2, NTV2_MODE_CAPTURE, false);
		mDevice.SetMode (NTV2_CHANNEL3, NTV2_MODE_CAPTURE, false);
		mDevice.SetMode (NTV2_CHANNEL4, NTV2_MODE_CAPTURE, false);
        mDevice.SetMode (NTV2_CHANNEL5, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL6, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL7, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL8, NTV2_MODE_DISPLAY, false);

		//	Enable frame buffers
		mDevice.EnableChannel (NTV2_CHANNEL1);
		mDevice.EnableChannel (NTV2_CHANNEL2);
		mDevice.EnableChannel (NTV2_CHANNEL3);
		mDevice.EnableChannel (NTV2_CHANNEL4);
        mDevice.EnableChannel (NTV2_CHANNEL5);
        mDevice.EnableChannel (NTV2_CHANNEL6);
        mDevice.EnableChannel (NTV2_CHANNEL7);
        mDevice.EnableChannel (NTV2_CHANNEL8);

		//	Save input source
		mInputSource = ::NTV2ChannelToInputSource (NTV2_CHANNEL1);
	}
    else if (mMultiStream)
	{
		//	Configure for multiformat
		if (::NTV2DeviceCanDoMultiFormat (mDeviceID))
			mDevice.SetMultiFormatMode (true);

        //	Set the channel video format for both input and output
        mDevice.SetVideoFormat (mVideoFormat, false, false, mInputChannel);
        mDevice.SetVideoFormat (mVideoFormat, false, false, mOutputChannel);

		//	Set frame buffer format
		mDevice.SetFrameBufferFormat (mInputChannel, mPixelFormat);
        mDevice.SetFrameBufferFormat (mOutputChannel, mPixelFormat);

		//	Set catpure mode
		mDevice.SetMode (mInputChannel, NTV2_MODE_CAPTURE, false);
        mDevice.SetMode (mOutputChannel, NTV2_MODE_DISPLAY, false);

		//	Enable frame buffer
		mDevice.EnableChannel (mInputChannel);
        mDevice.EnableChannel (mOutputChannel);

		//	Save input source
		mInputSource = ::NTV2ChannelToInputSource (mInputChannel);
	}
	else
	{
		//	Disable multiformat mode
		if (::NTV2DeviceCanDoMultiFormat (mDeviceID))
			mDevice.SetMultiFormatMode (false);

		//	Set the board format for both the input and output channel
		mDevice.SetVideoFormat (mVideoFormat, false, false, NTV2_CHANNEL1);
        mDevice.SetVideoFormat (mVideoFormat, false, false, NTV2_CHANNEL5);

		//	Set frame buffer format
		mDevice.SetFrameBufferFormat (mInputChannel, mPixelFormat);
        mDevice.SetFrameBufferFormat (mOutputChannel, mPixelFormat);

		//	Set display mode
		mDevice.SetMode (NTV2_CHANNEL1, NTV2_MODE_DISPLAY, false);
		mDevice.SetMode (NTV2_CHANNEL2, NTV2_MODE_DISPLAY, false);
		mDevice.SetMode (NTV2_CHANNEL3, NTV2_MODE_DISPLAY, false);
		mDevice.SetMode (NTV2_CHANNEL4, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL5, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL6, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL7, NTV2_MODE_DISPLAY, false);
        mDevice.SetMode (NTV2_CHANNEL8, NTV2_MODE_DISPLAY, false);

		//	Set catpure mode
		mDevice.SetMode (mInputChannel, NTV2_MODE_CAPTURE, false);

		//	Enable frame buffer
		mDevice.EnableChannel (mInputChannel);
        mDevice.EnableChannel (mOutputChannel);

		//	Save input source
		mInputSource = ::NTV2ChannelToInputSource (mInputChannel);
	}

	//	Set the device reference to the input...
    if (mMultiStream)
    {
        mDevice.SetReference (NTV2_REFERENCE_FREERUN);
    }
    else
    {
        mDevice.SetReference (::NTV2InputSourceToReferenceSource (mInputSource));
    }

	//	Enable and subscribe to the interrupts for the channel to be used...
	mDevice.EnableInputInterrupt (mInputChannel);
	mDevice.SubscribeInputVerticalEvent (mInputChannel);

    //  Setup for timecode burn
    mTimeBase.SetAJAFrameRate (mHevcCommon->GetAJAFrameRate(GetNTV2FrameRateFromVideoFormat (mVideoFormat)));
    mTimeCodeBurn.RenderTimeCodeFont (mHevcCommon->GetAJAPixelFormat (mPixelFormat),
                                      GetDisplayWidth (mVideoFormat),
                                      GetDisplayHeight (mVideoFormat));

	return AJA_STATUS_SUCCESS;

}	//	SetupVideo


AJAStatus NTV2EncodeHEVCVifAc::SetupAudio (void)
{
    //	In multiformat mode, base the audio system on the channel...
    if (mMultiStream && ::NTV2DeviceGetNumAudioStreams (mDeviceID) > 1 && UWord (mInputChannel) < ::NTV2DeviceGetNumAudioStreams (mDeviceID))
		mAudioSystem = ::NTV2ChannelToAudioSystem (mInputChannel);

	//	Have the audio system capture audio from the designated device input (i.e., ch1 uses SDIIn1, ch2 uses SDIIn2, etc.)...
	mDevice.SetAudioSystemInputSource (mAudioSystem, NTV2_AUDIO_EMBEDDED, ::NTV2ChannelToEmbeddedAudioInput (mInputChannel));

    mNumAudioChannels = ::NTV2DeviceGetMaxAudioChannels (mDeviceID);
    mDevice.SetNumberAudioChannels (mNumAudioChannels, mAudioSystem);
	mDevice.SetAudioRate (NTV2_AUDIO_48K, mAudioSystem);
    mDevice.SetEmbeddedAudioClock (NTV2_EMBEDDED_AUDIO_CLOCK_VIDEO_INPUT, mAudioSystem);

	//	The on-device audio buffer should be 4MB to work best across all devices & platforms...
	mDevice.SetAudioBufferSize (NTV2_AUDIO_BUFFER_BIG, mAudioSystem);

	return AJA_STATUS_SUCCESS;

}	//	SetupAudio


void NTV2EncodeHEVCVifAc::SetupHostBuffers (void)
{
	mVideoBufferSize = GetVideoActiveSize (mVideoFormat, mPixelFormat, false, false);
    mPicInfoBufferSize = sizeof(HevcPictureInfo)*2;
    mEncInfoBufferSize = sizeof(HevcEncodedInfo)*2;
    mAudioBufferSize = NTV2_AUDIOSIZE_MAX;
	
	// video input ring
    mVideoInputCircularBuffer.SetAbortFlag (&mGlobalQuit);
	for (unsigned bufferNdx = 0; bufferNdx < VIDEO_RING_SIZE; bufferNdx++ )
	{
        memset (&mVideoInputBuffer[bufferNdx], 0, sizeof(AVHevcDataBuffer));
        mVideoInputBuffer[bufferNdx].pVideoBuffer		= new uint32_t [mVideoBufferSize/4];
        mVideoInputBuffer[bufferNdx].videoBufferSize	= mVideoBufferSize;
        mVideoInputBuffer[bufferNdx].videoDataSize		= 0;
        mVideoInputBuffer[bufferNdx].videoDataSize2		= 0;
        mVideoInputBuffer[bufferNdx].pInfoBuffer		= new uint32_t [mPicInfoBufferSize/4];
        mVideoInputBuffer[bufferNdx].infoBufferSize     = mPicInfoBufferSize;
        mVideoInputBuffer[bufferNdx].infoDataSize		= 0;
        mVideoInputBuffer[bufferNdx].infoDataSize2		= 0;
        mVideoInputCircularBuffer.Add (& mVideoInputBuffer[bufferNdx]);
	}

    // video raw ring
    mVideoRawCircularBuffer.SetAbortFlag (&mGlobalQuit);
	for (unsigned bufferNdx = 0; bufferNdx < VIDEO_RING_SIZE; bufferNdx++ )
	{
        memset (&mVideoRawBuffer[bufferNdx], 0, sizeof(AVHevcDataBuffer));
        mVideoRawBuffer[bufferNdx].pVideoBuffer		= new uint32_t [mVideoBufferSize/4];
        mVideoRawBuffer[bufferNdx].videoBufferSize	= mVideoBufferSize;
        mVideoRawBuffer[bufferNdx].videoDataSize	= 0;
        mVideoRawBuffer[bufferNdx].videoDataSize2	= 0;
        mVideoRawBuffer[bufferNdx].pInfoBuffer		= new uint32_t [mPicInfoBufferSize/4];
        mVideoRawBuffer[bufferNdx].infoBufferSize   = mPicInfoBufferSize;
        mVideoRawBuffer[bufferNdx].infoDataSize		= 0;
        mVideoRawBuffer[bufferNdx].infoDataSize2	= 0;
        mVideoRawCircularBuffer.Add (& mVideoRawBuffer[bufferNdx]);
	}

    // video hevc ring
    mVideoHevcCircularBuffer.SetAbortFlag (&mGlobalQuit);
    for (unsigned bufferNdx = 0; bufferNdx < VIDEO_RING_SIZE; bufferNdx++ )
    {
        memset (&mVideoHevcBuffer[bufferNdx], 0, sizeof(AVHevcDataBuffer));
        mVideoHevcBuffer[bufferNdx].pVideoBuffer	= new uint32_t [mVideoBufferSize/4];
        mVideoHevcBuffer[bufferNdx].videoBufferSize	= mVideoBufferSize;
        mVideoHevcBuffer[bufferNdx].videoDataSize	= 0;
        mVideoHevcBuffer[bufferNdx].videoDataSize2	= 0;
        mVideoHevcBuffer[bufferNdx].pInfoBuffer		= new uint32_t [mEncInfoBufferSize/4];
        mVideoHevcBuffer[bufferNdx].infoBufferSize  = mEncInfoBufferSize;
        mVideoHevcBuffer[bufferNdx].infoDataSize	= 0;
        mVideoHevcBuffer[bufferNdx].infoDataSize2	= 0;
        mVideoHevcCircularBuffer.Add (& mVideoHevcBuffer[bufferNdx]);
    }

    if (mWithAudio)
    {
        // audio input ring
        mAudioInputCircularBuffer.SetAbortFlag (&mGlobalQuit);
        for (unsigned bufferNdx = 0; bufferNdx < AUDIO_RING_SIZE; bufferNdx++ )
        {
            memset (&mAudioInputBuffer[bufferNdx], 0, sizeof(AVHevcDataBuffer));
            mAudioInputBuffer[bufferNdx].pAudioBuffer		= new uint32_t [mAudioBufferSize/4];
            mAudioInputBuffer[bufferNdx].audioBufferSize	= mAudioBufferSize;
            mAudioInputBuffer[bufferNdx].audioDataSize		= 0;
            mAudioInputCircularBuffer.Add (& mAudioInputBuffer[bufferNdx]);
        }
    }

}	//	SetupHostBuffers


void NTV2EncodeHEVCVifAc::RouteInputSignal (void)
{
    // setup sdi io
	mDevice.SetSDITransmitEnable (NTV2_CHANNEL1, false);
	mDevice.SetSDITransmitEnable (NTV2_CHANNEL2, false);
	mDevice.SetSDITransmitEnable (NTV2_CHANNEL3, false);
	mDevice.SetSDITransmitEnable (NTV2_CHANNEL4, false);
    mDevice.SetSDITransmitEnable (NTV2_CHANNEL5, true);
    mDevice.SetSDITransmitEnable (NTV2_CHANNEL6, true);
    mDevice.SetSDITransmitEnable (NTV2_CHANNEL7, true);
    mDevice.SetSDITransmitEnable (NTV2_CHANNEL8, true);

	//	Give the device some time to lock to the input signal...
	mDevice.WaitForOutputVerticalInterrupt (mInputChannel, 8);

	//	When input is 3Gb convert to 3Ga for capture (no RGB support?)
	bool is3Gb = false;
	mDevice.GetSDIInput3GbPresent (is3Gb, mInputChannel);

	if (mQuad)
	{
		mDevice.SetSDIInLevelBtoLevelAConversion (NTV2_CHANNEL1, is3Gb);
		mDevice.SetSDIInLevelBtoLevelAConversion (NTV2_CHANNEL2, is3Gb);
		mDevice.SetSDIInLevelBtoLevelAConversion (NTV2_CHANNEL3, is3Gb);
		mDevice.SetSDIInLevelBtoLevelAConversion (NTV2_CHANNEL4, is3Gb);
        mDevice.SetSDIOutLevelAtoLevelBConversion (NTV2_CHANNEL5, false);
        mDevice.SetSDIOutLevelAtoLevelBConversion (NTV2_CHANNEL6, false);
        mDevice.SetSDIOutLevelAtoLevelBConversion (NTV2_CHANNEL7, false);
        mDevice.SetSDIOutLevelAtoLevelBConversion (NTV2_CHANNEL8, false);
    }
	else
	{
		mDevice.SetSDIInLevelBtoLevelAConversion (mInputChannel, is3Gb);
        mDevice.SetSDIOutLevelAtoLevelBConversion (mOutputChannel, false);
    }

	//	Signal routing...
	if (!mMultiStream)
		mDevice.ClearRouting ();
	mDevice.Connect (NTV2_XptFrameBuffer1Input, NTV2_XptSDIIn1);
	mDevice.Connect (NTV2_XptFrameBuffer2Input, NTV2_XptSDIIn2);
	mDevice.Connect (NTV2_XptFrameBuffer3Input, NTV2_XptSDIIn3);
	mDevice.Connect (NTV2_XptFrameBuffer4Input, NTV2_XptSDIIn4);
    mDevice.Connect (NTV2_XptSDIOut5Input, NTV2_XptFrameBuffer5YUV);
    mDevice.Connect (NTV2_XptSDIOut6Input, NTV2_XptFrameBuffer6YUV);
    mDevice.Connect (NTV2_XptSDIOut7Input, NTV2_XptFrameBuffer7YUV);
    mDevice.Connect (NTV2_XptSDIOut8Input, NTV2_XptFrameBuffer8YUV);

	//	Give the device some time to lock to the input signal...
	mDevice.WaitForOutputVerticalInterrupt (mInputChannel, 8);

}	//	RouteInputSignal


void NTV2EncodeHEVCVifAc::SetupAutoCirculate (void)
{
	//	Tell capture AutoCirculate to use 8 frame buffers on the device...
	mInputTransferStruct.channelSpec			= ::NTV2ChannelToInputChannelSpec (mInputChannel);
    mInputTransferStruct.videoBufferSize		= 0;
	mInputTransferStruct.videoDmaOffset			= 0;
    mInputTransferStruct.audioBufferSize		= 0;
	mInputTransferStruct.frameRepeatCount		= 1;
	mInputTransferStruct.desiredFrame			= -1;
	mInputTransferStruct.frameBufferFormat		= mPixelFormat;
	mInputTransferStruct.bDisableExtraAudioInfo	= true;

	mDevice.StopAutoCirculate (mInputTransferStruct.channelSpec);
	mDevice.AutoCirculateInitForInput (mInputChannel,	16,	//	Frames to circulate
										mWithAudio ? mAudioSystem : NTV2_AUDIOSYSTEM_INVALID,	//	Which audio system (if any)?
										AUTOCIRCULATE_WITH_RP188);								//	With RP188?

    // Driving output using AutoCirculate which is routed into the M31
    mOutputTransferStruct.channelSpec               = ::NTV2ChannelToOutputChannelSpec (mOutputChannel);
    mOutputTransferStruct.videoBufferSize           = 0;
    mOutputTransferStruct.videoDmaOffset            = 0;
    mOutputTransferStruct.audioBufferSize           = 0;
    mOutputTransferStruct.frameRepeatCount          = 1;
    mOutputTransferStruct.desiredFrame              = -1;
    mOutputTransferStruct.frameBufferFormat         = mPixelFormat;
    mOutputTransferStruct.bDisableExtraAudioInfo	= true;

    mDevice.StopAutoCirculate (mOutputTransferStruct.channelSpec);
    mDevice.AutoCirculateInitForOutput (mOutputChannel,	8);	//	Frames to circulate
}	//	SetupInputAutoCirculate


AJAStatus NTV2EncodeHEVCVifAc::Run ()
{
	if (mDevice.GetInputVideoFormat (mInputSource) == NTV2_FORMAT_UNKNOWN)
		cout << endl << "## WARNING:  No video signal present on the input connector" << endl;

	//	Start the playout and capture threads...
    StartVideoInputThread ();
    StartVideoProcessThread ();
    StartCodecRawThread ();
    StartCodecHevcThread ();
    StartVideoFileThread ();
    if (mWithAudio)
    {
        StartAudioFileThread ();
    }

	return AJA_STATUS_SUCCESS;

}	//	Run


// This is where we will start the video input thread
void NTV2EncodeHEVCVifAc::StartVideoInputThread (void)
{
    mVideoInputThread = new AJAThread ();
    mVideoInputThread->Attach (VideoInputThreadStatic, this);
    mVideoInputThread->SetPriority (AJA_ThreadPriority_High);
    mVideoInputThread->Start ();

}	// StartVideoInputThread


// The video input thread static callback
void NTV2EncodeHEVCVifAc::VideoInputThreadStatic (AJAThread * pThread, void * pContext)
{
	(void) pThread;

	NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->VideoInputWorker ();

}	// VideoInputThreadStatic


void NTV2EncodeHEVCVifAc::VideoInputWorker (void)
{
    CNTV2Card   ntv2Device;
    
    //	Open the device...
    if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, ntv2Device))
    { cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return; }
    
    ntv2Device.SubscribeInputVerticalEvent (mInputChannel);

	// start AutoCirculate running...
	ntv2Device.StartAutoCirculate (mInputTransferStruct.channelSpec);

	while (!mGlobalQuit)
	{
		AUTOCIRCULATE_STATUS_STRUCT	acStatus;
		ntv2Device.GetAutoCirculate (mInputTransferStruct.channelSpec, &acStatus);

        // wait for captured frame
		if (acStatus.state == NTV2_AUTOCIRCULATE_RUNNING && acStatus.bufferLevel > 1)
		{
			// At this point, there's at least one fully-formed frame available in the device's
			// frame buffer to transfer to the host. Reserve an AvaDataBuffer to "produce", and
			// use it in the next transfer from the device...
            AVHevcDataBuffer *	pVideoData	(mVideoInputCircularBuffer.StartProduceNextBuffer ());
            if (pVideoData)
            {
                // setup buffer pointers for transfer
                mInputTransferStruct.videoBuffer		= pVideoData->pVideoBuffer;
                mInputTransferStruct.videoBufferSize	= pVideoData->videoBufferSize;
                mInputTransferStruct.audioBuffer		= NULL;
                mInputTransferStruct.audioBufferSize	= 0;

                AVHevcDataBuffer *	pAudioData = 0;
                if (mWithAudio)
                {
                    pAudioData = mAudioInputCircularBuffer.StartProduceNextBuffer ();
                    if (pAudioData)
                    {
                        mInputTransferStruct.audioBuffer		= pAudioData->pAudioBuffer;
                        mInputTransferStruct.audioBufferSize	= pAudioData->audioBufferSize;
                    }
                }

                // do the transfer from the device into our host AvaDataBuffer...
                ntv2Device.TransferWithAutoCirculate (&mInputTransferStruct, &mInputTransferStatusStruct);

                // get the video data size
                pVideoData->videoDataSize = pVideoData->videoBufferSize;
                pVideoData->audioDataSize = 0;
                pVideoData->lastFrame = mLastFrame;

                if (mWithAudio && pAudioData)
                {
                    // get the audio data size
                    pAudioData->audioDataSize = mInputTransferStatusStruct.audioBufferSize;
                    pAudioData->lastFrame = mLastFrame;
                }

                if (mWithAnc)
                {
                    // get the sdi input anc data
                    pVideoData->timeCodeDBB = mInputTransferStatusStruct.frameStamp.currentRP188.DBB;
                    pVideoData->timeCodeLow = mInputTransferStatusStruct.frameStamp.currentRP188.Low;
                    pVideoData->timeCodeHigh = mInputTransferStatusStruct.frameStamp.currentRP188.High;
                }

                if (mWithInfo)
                {
                    // get picture and additional data pointers
                    HevcPictureInfo * pInfo = (HevcPictureInfo*)pVideoData->pInfoBuffer;
                    HevcPictureData * pPicData = &pInfo->pictureData;

                    // initialize info buffer to 0
                    memset(pInfo, 0, pVideoData->infoBufferSize);

                    // calculate pts based on 90 Khz clock tick
                    uint64_t pts = (uint64_t)mTimeBase.FramesToMicroseconds(mVideoInputFrameCount)*90000/1000000;

                    // set serial number, pts and picture number
                    pPicData->serialNumber = mVideoInputFrameCount;         		// can be anything
                    pPicData->ptsValueLow = (uint32_t)(pts & 0xffffffff);			//   (frame 5720000@60 test roll over)
                    pPicData->ptsValueHigh = (uint32_t)((pts >> 32) & 0x1);			// only use 1 bit
                    pPicData->pictureNumber = mVideoInputFrameCount + 1;    		// must count starting with 1

                    // set info data size
                    pVideoData->infoDataSize = sizeof(HevcPictureData);

					if(mInterlaced)
					{
						pPicData->serialNumber = mVideoInputFrameCount*2;
						pPicData->pictureNumber = mVideoInputFrameCount*2 + 1;

						// get picture and additional data pointers
						pInfo = (HevcPictureInfo*)(pVideoData->pInfoBuffer + sizeof(HevcPictureInfo)/4);
						pPicData = &pInfo->pictureData;

						// add half a frame time to pts
						pts = pts + (uint64_t)mTimeBase.FramesToMicroseconds(1)*90000/1000000/2;

						// set serial number, pts and picture number
						pPicData->serialNumber = mVideoInputFrameCount*2 + 1;
						pPicData->ptsValueLow = (uint32_t)(pts & 0xffffffff);
						pPicData->ptsValueHigh = (uint32_t)((pts >> 32) & 0x1);
						pPicData->pictureNumber = mVideoInputFrameCount*2 + 2;

						// set info data size
						pVideoData->infoDataSize2 = sizeof(HevcPictureData);
					}
                }

                if(pVideoData->lastFrame && !mLastFrameInput)
                {
                    printf ( "\nCapture last frame number %d\n", mVideoInputFrameCount );
                    mLastFrameInput = true;
                }

                mVideoInputFrameCount++;

                if (mWithAudio && pAudioData)
                {
                    mAudioInputCircularBuffer.EndProduceNextBuffer ();
                }

                // signal that we're done "producing" the frame, making it available for future "consumption"...
                mVideoInputCircularBuffer.EndProduceNextBuffer ();
            }	// if A/C running and frame(s) are available for transfer
        }
		else
		{
			// Either AutoCirculate is not running, or there were no frames available on the device to transfer.
			// Rather than waste CPU cycles spinning, waiting until a frame becomes available, it's far more
			// efficient to wait for the next input vertical interrupt event to get signaled...
            ntv2Device.WaitForInputVerticalInterrupt (mInputChannel);
		}
	}	// loop til quit signaled

	// Stop AutoCirculate...
	ntv2Device.StopAutoCirculate (mInputTransferStruct.channelSpec);

    ntv2Device.UnsubscribeInputVerticalEvent (mInputChannel);

}	// VideoInputWorker


// This is where we start the video process thread
void NTV2EncodeHEVCVifAc::StartVideoProcessThread (void)
{
    mVideoProcessThread = new AJAThread ();
    mVideoProcessThread->Attach (VideoProcessThreadStatic, this);
    mVideoProcessThread->SetPriority (AJA_ThreadPriority_High);
    mVideoProcessThread->Start ();

}	// StartVideoProcessThread


// The video process static callback
void NTV2EncodeHEVCVifAc::VideoProcessThreadStatic (AJAThread * pThread, void * pContext)
{
	(void) pThread;

	NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->VideoProcessWorker ();

}	// VideoProcessThreadStatic


void NTV2EncodeHEVCVifAc::VideoProcessWorker (void)
{
	while (!mGlobalQuit)
	{
		// wait for the next video input buffer
        AVHevcDataBuffer *	pSrcFrameData (mVideoInputCircularBuffer.StartConsumeNextBuffer ());
		if (pSrcFrameData)
		{
			// wait for the next video raw buffer
			AVHevcDataBuffer *	pDstFrameData (mVideoRawCircularBuffer.StartProduceNextBuffer ());
			if (pDstFrameData)
			{
				// do something useful with the frame data...
                ProcessVideoFrame(pSrcFrameData, pDstFrameData, mVideoProcessFrameCount);

                mVideoProcessFrameCount++;

				// release the video raw buffer
				mVideoRawCircularBuffer.EndProduceNextBuffer ();
            }

			// release the video input buffer
            mVideoInputCircularBuffer.EndConsumeNextBuffer ();

        }
	}	// loop til quit signaled

}	// VideoProcessWorker


// This is where we start the codec raw thread
void NTV2EncodeHEVCVifAc::StartCodecRawThread (void)
{
    mCodecRawThread = new AJAThread ();
    mCodecRawThread->Attach (CodecRawThreadStatic, this);
    mCodecRawThread->SetPriority (AJA_ThreadPriority_High);
    mCodecRawThread->Start ();

}	// StartCodecRawThread


// The codec raw static callback
void NTV2EncodeHEVCVifAc::CodecRawThreadStatic (AJAThread * pThread, void * pContext)
{
	(void) pThread;

	NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->CodecRawWorker ();

}	// CodecRawThreadStatic


void NTV2EncodeHEVCVifAc::CodecRawWorker (void)
{
    CNTV2Card       ntv2Device;
    
    //	Open the device...
    if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, ntv2Device))
    { cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return; }

	while (!mGlobalQuit)
	{
		// wait for the next raw video frame
        AVHevcDataBuffer *	pFrameData (mVideoRawCircularBuffer.StartConsumeNextBuffer ());
		if (pFrameData)
		{
			if (!mLastFrameRaw)
			{
                // In vif mode we are just driving the AutoCirculate output which is routed into the M31, no need
                // to DMA frames using RawTransfer
                mOutputTransferStruct.videoBuffer		= pFrameData->pVideoBuffer;
                mOutputTransferStruct.videoBufferSize	= pFrameData->videoBufferSize;
                mOutputTransferStruct.audioBuffer		= NULL;
                mOutputTransferStruct.audioBufferSize	= 0;

                ntv2Device.TransferWithAutoCirculate (&mOutputTransferStruct, &mOutputTransferStatusStruct);

                if (mCodecRawFrameCount == 3)
                {
                    // start AutoCirculate running...
                    mDevice.StartAutoCirculate (mOutputTransferStruct.channelSpec);
                }
            
                if (pFrameData->lastFrame)
				{
					mLastFrameRaw = true;
				}

                mCodecRawFrameCount++;
            }

			// release the raw video frame
            mVideoRawCircularBuffer.EndConsumeNextBuffer ();
		}
	}  // loop til quit signaled

    // Stop AutoCirculate...
    ntv2Device.StopAutoCirculate (mOutputTransferStruct.channelSpec);

} // CodecRawWorker


// This is where we will start the codec hevc thread
void NTV2EncodeHEVCVifAc::StartCodecHevcThread (void)
{
    mCodecHevcThread = new AJAThread ();
    mCodecHevcThread->Attach (CodecHevcThreadStatic, this);
    mCodecHevcThread->SetPriority (AJA_ThreadPriority_High);
    mCodecHevcThread->Start ();

} // StartCodecHevcThread


// The codec hevc static callback
void NTV2EncodeHEVCVifAc::CodecHevcThreadStatic (AJAThread * pThread, void * pContext)
{
    (void) pThread;

    NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->CodecHevcWorker ();

}	//	CodecHevcThreadStatic


void NTV2EncodeHEVCVifAc::CodecHevcWorker (void)
{
    CNTV2Card       ntv2Device;
    CNTV2m31 *      m31;
    
    //	Open the device...
    if (!CNTV2DeviceScanner::GetFirstDeviceFromArgument (mDeviceSpecifier, ntv2Device))
    { cerr << "## ERROR:  Device '" << mDeviceSpecifier << "' not found" << endl;  return; }
    
    // Allocate our M31 helper class and our HEVC common class
    m31 = new CNTV2m31 (&ntv2Device);
    
    while (!mGlobalQuit)
    {
        // wait for the next hevc frame 
        AVHevcDataBuffer *	pFrameData (mVideoHevcCircularBuffer.StartProduceNextBuffer ());
        if (pFrameData)
        {
			if (!mLastFrameHevc)
			{
				if (mInterlaced)
				{
					// get field 1 video and info buffer and size
					uint8_t* pVideoBuffer = (uint8_t*)pFrameData->pVideoBuffer;
					uint8_t* pInfoBuffer = (uint8_t*)pFrameData->pInfoBuffer;
					uint32_t videoBufferSize = pFrameData->videoBufferSize;
					uint32_t infoBufferSize = sizeof(HevcEncodedInfo);

					// transfer an hevc field 1 from the codec including encoded information
					m31->EncTransfer(mEncodeChannel,
										pVideoBuffer,
										videoBufferSize,
										pInfoBuffer,
										infoBufferSize,
										pFrameData->videoDataSize,
										pFrameData->infoDataSize,
										pFrameData->lastFrame);

					// round the video size up
					pFrameData->videoDataSize = mHevcCommon->AlignDataBuffer(pVideoBuffer,
																videoBufferSize,
																pFrameData->videoDataSize,
																8, 0xff);
					// round the info size up
					pFrameData->infoDataSize = mHevcCommon->AlignDataBuffer(pInfoBuffer,
																infoBufferSize,
																pFrameData->infoDataSize,
																8, 0);

					// get field 2 video and info buffer and size
					pVideoBuffer = ((uint8_t*)pFrameData->pVideoBuffer) + pFrameData->videoDataSize;
					pInfoBuffer = ((uint8_t*)pFrameData->pInfoBuffer) + sizeof(HevcEncodedInfo);
					videoBufferSize = pFrameData->videoBufferSize - pFrameData->videoDataSize;
					infoBufferSize = sizeof(HevcEncodedInfo);

					// transfer an hevc field 2 from the codec including encoded information
					m31->EncTransfer(mEncodeChannel,
										pVideoBuffer,
										videoBufferSize,
										pInfoBuffer,
										infoBufferSize,
										pFrameData->videoDataSize2,
										pFrameData->infoDataSize2,
										pFrameData->lastFrame);

					// round the video size up
					pFrameData->videoDataSize2 = mHevcCommon->AlignDataBuffer(pVideoBuffer,
																videoBufferSize,
																pFrameData->videoDataSize2,
																8, 0xff);
					// round the info size up
					pFrameData->infoDataSize2 = mHevcCommon->AlignDataBuffer(pInfoBuffer,
																infoBufferSize,
																pFrameData->infoDataSize2,
																8, 0);
				}
				else
				{
					// transfer an hevc frame from the codec including encoded information
					m31->EncTransfer(mEncodeChannel,
										(uint8_t*)pFrameData->pVideoBuffer,
										pFrameData->videoBufferSize,
										(uint8_t*)pFrameData->pInfoBuffer,
										pFrameData->infoBufferSize,
										pFrameData->videoDataSize,
										pFrameData->infoDataSize,
										pFrameData->lastFrame);

					// round the video size up
					pFrameData->videoDataSize = mHevcCommon->AlignDataBuffer(pFrameData->pVideoBuffer,
																pFrameData->videoBufferSize,
																pFrameData->videoDataSize,
																8, 0xff);
					// round the info size up
					pFrameData->infoDataSize = mHevcCommon->AlignDataBuffer(pFrameData->pInfoBuffer,
																pFrameData->infoBufferSize,
																pFrameData->infoDataSize,
																8, 0);
				}

                if (pFrameData->lastFrame)
				{
					mLastFrameHevc = true;
                }

                mCodecHevcFrameCount++;
            }

            // release and recycle the buffer...
            mVideoHevcCircularBuffer.EndProduceNextBuffer ();
        }
    }	//	loop til quit signaled
    
    delete m31;
}	//	EncTransferFrames


// This is where we start the video file writer thread
void NTV2EncodeHEVCVifAc::StartVideoFileThread (void)
{
    mVideoFileThread = new AJAThread ();
    mVideoFileThread->Attach (VideoFileThreadStatic, this);
    mVideoFileThread->SetPriority (AJA_ThreadPriority_High);
    mVideoFileThread->Start ();

} // StartVideoFileThread


// The file writer static callback
void NTV2EncodeHEVCVifAc::VideoFileThreadStatic (AJAThread * pThread, void * pContext)
{
    (void) pThread;

    NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->VideoFileWorker ();

} // VideoFileStatic


void NTV2EncodeHEVCVifAc::VideoFileWorker (void)
{
    while (!mGlobalQuit)
    {
        // wait for the next codec hevc frame
        AVHevcDataBuffer *	pFrameData (mVideoHevcCircularBuffer.StartConsumeNextBuffer ());
        if (pFrameData)
        {
            if (!mLastFrameVideo)
			{
				// write the frame / fields hevc to the output file
                mHevcCommon->WriteHevcData(pFrameData->pVideoBuffer, pFrameData->videoDataSize + pFrameData->videoDataSize2);

                if (mWithInfo)
                {
                    // write the frame / field 1 encoded data to the output file
                    mHevcCommon->WriteEncData(pFrameData->pInfoBuffer, pFrameData->infoDataSize);
                    // write the field 2 encoded data to the output file
                    mHevcCommon->WriteEncData(pFrameData->pInfoBuffer + sizeof(HevcEncodedInfo)/4, pFrameData->infoDataSize2);
                }

                if (pFrameData->lastFrame)
				{
                    printf ( "Video file last frame number %d\n", mVideoFileFrameCount );
                    mLastFrameVideo = true;
				}

                mVideoFileFrameCount++;
            }

            // release the hevc buffer
            mVideoHevcCircularBuffer.EndConsumeNextBuffer ();
        }
    } // loop til quit signaled

} // VideoFileWorker


// This is where we start the audio file writer thread
void NTV2EncodeHEVCVifAc::StartAudioFileThread (void)
{
    mAudioFileThread = new AJAThread ();
    mAudioFileThread->Attach (AudioFileThreadStatic, this);
    mAudioFileThread->SetPriority (AJA_ThreadPriority_High);
    mAudioFileThread->Start ();

} // StartAudioFileThread


// The file writer static callback
void NTV2EncodeHEVCVifAc::AudioFileThreadStatic (AJAThread * pThread, void * pContext)
{
    (void) pThread;

    NTV2EncodeHEVCVifAc *	pApp (reinterpret_cast <NTV2EncodeHEVCVifAc *> (pContext));
    pApp->AudioFileWorker ();

} // AudioFileStatic


void NTV2EncodeHEVCVifAc::AudioFileWorker (void)
{
    while (!mGlobalQuit)
    {
        // wait for the next codec hevc frame
        AVHevcDataBuffer *	pFrameData (mAudioInputCircularBuffer.StartConsumeNextBuffer ());
        if (pFrameData)
        {
            if (!mLastFrameAudio)
            {
                // write the audio samples to the output file
                mHevcCommon->WriteAiffData(pFrameData->pAudioBuffer, mNumAudioChannels, pFrameData->audioDataSize/mNumAudioChannels/4);

                if (pFrameData->lastFrame)
                {
                    printf ( "Audio file last frame number %d\n", mAudioFileFrameCount );
                    mLastFrameAudio = true;
                }
            }

            mAudioFileFrameCount++;

            // release the hevc buffer
            mAudioInputCircularBuffer.EndConsumeNextBuffer ();
        }
    } // loop til quit signaled

} // AudioFileWorker


//////////////////////////////////////////////


void NTV2EncodeHEVCVifAc::GetStatus (AVHevcStatus * outInputStatus)
{
    AUTOCIRCULATE_STATUS_STRUCT	inputACStatus;
    
    mDevice.GetAutoCirculate (mInputTransferStruct.channelSpec, &inputACStatus);
    outInputStatus->framesProcessed = inputACStatus.framesProcessed;
    outInputStatus->framesDropped = inputACStatus.framesDropped;
    outInputStatus->bufferLevel = inputACStatus.bufferLevel;
    
}	//	GetStatus


AJAStatus NTV2EncodeHEVCVifAc::ProcessVideoFrame (AVHevcDataBuffer * pSrcFrame, AVHevcDataBuffer * pDstFrame, uint32_t frameNumber)
{

	//	Override this function to use the frame data in the way your application requires
	memcpy(pDstFrame->pVideoBuffer, pSrcFrame->pVideoBuffer, pSrcFrame->videoDataSize);
    pDstFrame->videoDataSize = pSrcFrame->videoDataSize;
    pDstFrame->timeCodeDBB = pSrcFrame->timeCodeDBB;
    pDstFrame->timeCodeLow = pSrcFrame->timeCodeLow;
    pDstFrame->timeCodeHigh = pSrcFrame->timeCodeHigh;
    pDstFrame->lastFrame = pSrcFrame->lastFrame;
    if (mWithInfo)
    {
        memcpy(pDstFrame->pInfoBuffer, pSrcFrame->pInfoBuffer, pSrcFrame->infoDataSize + pSrcFrame->infoDataSize2);
        pDstFrame->infoDataSize = pSrcFrame->infoDataSize;
        pDstFrame->infoDataSize2 = pSrcFrame->infoDataSize2;
    }

    if (mWithAnc)
    {
        char timeString[24];
        mTimeCode.Set(frameNumber);
        mTimeCode.SetStdTimecodeForHfr(false);
        mTimeCode.QueryString(timeString, mTimeBase, false);
        mTimeCodeBurn.BurnTimeCode((char*)pDstFrame->pVideoBuffer, timeString, 10);
        mTimeCode.SetRP188(pDstFrame->timeCodeDBB, pDstFrame->timeCodeLow, pDstFrame->timeCodeHigh, mTimeBase);
        mTimeCode.QueryString(timeString, mTimeBase, false);
        mTimeCodeBurn.BurnTimeCode((char*)pDstFrame->pVideoBuffer, timeString, 20);
    }

    return AJA_STATUS_SUCCESS;

}	//	ProcessFrame
