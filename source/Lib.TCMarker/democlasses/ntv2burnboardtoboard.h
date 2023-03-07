/**
	@file	ntv2burnboardtoboard.h
	@brief	Header file for the NTV2BurnBoardToBoard demonstration class.
	@copyright	Copyright (C) 2012-2016 AJA Video Systems, Inc.  All rights reserved.
**/

#ifndef _NTV2BURNBOARDTOBOARD_H
#define _NTV2BURNBOARDTOBOARD_H

#include "ntv2card.h"
#include "ntv2utils.h"
#include "ntv2democommon.h"
#include "ajastuff/common/types.h"
#include "ajastuff/common/circularbuffer.h"
#include "ajastuff/system/thread.h"
#include "ajastuff/system/lock.h"
#include "ajastuff/common/timecodeburn.h"


/**
	@brief	Instances of me can capture frames from a video signal provided to an input of an AJA device,
			burn timecode into those frames, then deliver them to an output of the same AJA device, all in real time.
			I make use of the AJACircularBuffer, which simplifies implementing a producer/consumer model,
			in which a "consumer" thread delivers burned-in frames to the AJA device output, and a "producer"
			thread captures raw frames from the AJA device input.
			I also demonstrate how to detect if an SDI input has embedded timecode, and if so, how AutoCirculate
			makes it available. I also show how to embed timecode into an SDI output signal using AutoCirculate
			during playout.
**/

class NTV2BurnBoardToBoard
{
	//	Public Instance Methods
	public:
		/**
			@brief	Constructs me using the given configuration settings.
			@note	I'm not completely initialized and ready for use until after my Init method has been called.
			@param[in]	inDeviceSpecifier	Specifies the AJA device to use for Input. Defaults to "0", the first device found.
			@param[in]	outDeviceSpecifier	Specifies the AJA device to use for Output. Defaults to "1", the first device found.
			@param[in]	inWithAudio			If true (the default), include audio in the output signal;  otherwise, omit it.
			@param[in]	inPixelFormat		Specifies the pixel format to use for the device's frame buffers. Defaults to 8-bit YUV.
			@param[in]	inInputSource		Specifies which input to capture from. Defaults to SDI1.
			@param[in]	inMultiFormat		If true, enables multiformat/multichannel mode if the device supports it, and won't acquire
											or release the device. If false (the default), acquires/releases exclusive use of the device.
			@param[in]	inTCSource			Specifies the timecode source. Defaults to whatever is found embedded in the input video.
			@param[in]	inWithAnc			If true, capture & play ancillary data. Defaults to false.
		**/
							NTV2BurnBoardToBoard (const std::string &			inDeviceSpecifier	= "0",
										const std::string &			outDeviceSpecifier = "1",
										const bool					inWithAudio = true,
										const bool					disableBurn = true,
										const NTV2FrameBufferFormat	inPixelFormat = NTV2_FBF_8BIT_YCBCR,
										const NTV2InputSource		inInputSource		= NTV2_INPUTSOURCE_SDI1,
										const bool					inMultiFormat		= false,
										const NTV2TCIndex			inTCSource			= NTV2_TCINDEX_SDI1,
										const bool					inWithAnc			= false);
		virtual				~NTV2BurnBoardToBoard ();

		/**
			@brief	Initializes me and prepares me to Run.
		**/
		virtual AJAStatus		Init (void);

		/**
			@brief	Runs me.
			@note	Do not call this method without first calling my Init method.
		**/
		virtual AJAStatus		Run (void);

		/**
			@brief	Gracefully stops me from running.
		**/
		virtual void			Quit (void);

		/**
			@brief	Provides status information about my input (capture) and output (playout) processes.
			@param[out]	outFramesProcessed		Receives the number of frames successfully processed.
			@param[out]	outCaptureFramesDropped	Receives the number of dropped capture frames.
			@param[out]	outPlayoutFramesDropped	Receives the number of dropped playout frames.
			@param[out]	outCaptureBufferLevel	Receives the capture driver buffer level.
			@param[out]	outPlayoutBufferLevel	Receives the playout driver buffer level.
			@param[out]	inputVideoFormat	    Receives the inputVideoFormat .
**/
		virtual void			GetStatus (ULWord & outFramesProcessed, ULWord & outCaptureFramesDropped, ULWord & outPlayoutFramesDropped,
			ULWord & outCaptureBufferLevel, ULWord & outPlayoutBufferLevel,NTV2VideoFormat &inputVideoFormat);


	//	Protected Instance Methods
	protected:
		/**
			@brief	Sets up everything I need for capturing and playing video.
		**/
		virtual AJAStatus		SetupVideo (void);

		/**
			@brief	Sets up everything I need for capturing and playing audio.
		**/
		virtual AJAStatus		SetupAudio (void);

		/**
			@brief	Sets up board routing for capture.
		**/
		virtual void			RouteInputSignal (void);

		/**
			@brief	Sets up board routing for playout.
		**/
		virtual void			RouteOutputSignal (void);

		/**
			@brief	Sets up my circular buffers.
		**/
		virtual AJAStatus		SetupHostBuffers (void);

		/**
			@brief	Starts my playout thread.
		**/
		virtual void			StartPlayThread (void);

		/**
			@brief	Repeatedly plays out frames using AutoCirculate (until global quit flag set).
		**/
		virtual void			PlayFrames (void);

		/**
			@brief	Starts my capture thread.
		**/
		virtual void			StartCaptureThread (void);

		/**
			@brief	Repeatedly captures frames using AutoCirculate (until global quit flag set).
		**/
		virtual void			CaptureFrames (void);


		/**
			@brief	Returns true if the current input signal has timecode embedded in it; otherwise returns false.
		**/
		virtual bool			InputSignalHasTimecode (void);


		/**
			@brief	Returns true if there is a valid LTC signal on my device's primary analog LTC input port; otherwise returns false.
		**/
		virtual bool			AnalogLTCInputHasTimecode (void);


	//	Protected Class Methods
	protected:
		/**
			@brief	This is the playout thread's static callback function that gets called when the playout thread runs.
					This function gets "Attached" to the playout thread's AJAThread instance.
			@param[in]	pThread		A valid pointer to the playout thread's AJAThread instance.
			@param[in]	pContext	Context information to pass to the thread.
									(For this application, this will be set to point to the NTV2Burn instance.)
		**/
		static void				PlayThreadStatic (AJAThread * pThread, void * pContext);

		/**
			@brief	This is the capture thread's static callback function that gets called when the capture thread runs.
					This function gets "Attached" to the AJAThread instance.
			@param[in]	pThread		Points to the AJAThread instance.
			@param[in]	pContext	Context information to pass to the thread.
									(For this application, this will be set to point to the NTV2Burn instance.)
		**/
		static void				CaptureThreadStatic (AJAThread * pThread, void * pContext);


	//	Private Member Data
	private:
		AJAThread *					mPlayThread;		///< @brief	My playout thread object
		AJAThread *					mCaptureThread;		///< @brief	My capture thread object
		AJALock *					mLock;				///< @brief	My mutex object
		CNTV2Card					mInDevice;			///< @brief	My Input CNTV2Card instance
		NTV2DeviceID				mInDeviceID;			///< @brief	My Input device identifier
		const std::string			mInDeviceSpecifier;	///< @brief	Specifies which Input device I should use
		CNTV2Card					mOutDevice;			///< @brief	My Output CNTV2Card instance
		NTV2DeviceID				mOutDeviceID;			///< @brief	My Output device identifier
		const std::string			mOutDeviceSpecifier;	///< @brief	Specifies which Output device I should use
		NTV2Channel					mInputChannel;		///< @brief	The input channel I'm using
		NTV2Channel					mOutputChannel;		///< @brief	The output channel I'm using
		NTV2InputSource				mInputSource;		///< @brief	The input source I'm using
		NTV2OutputDestination		mOutputDestination;	///< @brief	The output I'm using
		NTV2VideoFormat				mVideoFormat;		///< @brief	My video format
		NTV2FrameBufferFormat		mPixelFormat;		///< @brief	My pixel format
		NTV2FormatDescriptor		mFormatDescriptor;	///< @brief	Description of the board's frame geometry
		NTV2EveryFrameTaskMode		mSavedTaskMode;		///< @brief	We will restore the previous state
		bool						mVancEnabled;		///< @brief	VANC enabled?
		bool						mWideVanc;			///< @brief	Wide VANC?
		NTV2AudioSystem				mAudioSystem;		///< @brief	The audio system I'm using
		bool						mDisableBurn;		///< @brief	Set "true" disable the actual time code burn....basically EtoE.
		bool						mGlobalQuit;		///< @brief	Set "true" to gracefully stop
		bool						mDoMultiChannel;	///< @brief	Set the board up for multi-format
		AJATimeCodeBurn				mTCBurner;			///< @brief	My timecode burner
		uint32_t					mVideoBufferSize;	///< @brief	My video buffer size, in bytes
		NTV2TCIndexes				mTCOutputs;			///< @brief	My output timecode destinations
		NTV2TCIndex					mTCSource;			///< @brief	The timecode source
		bool						mWithAnc;			///< @brief	Capture and Playout packetized ANC data
		AVDataBuffer						mAVHostBuffer [CIRCULAR_BUFFER_SIZE];	///< @brief	My host buffers
		AJACircularBuffer <AVDataBuffer *>	mAVCircularBuffer;						///< @brief	My ring buffer

};	//	NTV2Burn

#endif	//	_NTV2BURNBOARDTOBOARD_H
