/*
  Module: CNTV2AudioLoopBack.h

  Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.

  For a description of how to use this class please refer to CNTV2AudioLoopBack.cpp
*/



#ifndef _NTV2AUDIOLOOPBACK_H
#define _NTV2AUDIOLOOPBACK_H

#include "AJATypes.h"
#include "NTV2Enums.h"
#include "audiodefines.h"
#include "ntv2active.h"
#ifndef NO_WGLIB
#include "..\..\WinGraphLib\win.h"        // (Wingraphlib thing) must be included before logwindow.h
                                          // We specify the path so we can't get the wrong 'win.h"!  (There is another one in the seldom-used Win32ClassLibrary!)
#include "logwindow.h"  // to hook into the Log Window!
#endif
#include "NTV2Card.h"


class CNTV2AudioLoopBack : public ActiveObject
{
public:
	CNTV2AudioLoopBack(UWord boardNumber,
                NTV2DMAEngine captureMethod = NTV2_DMA3,
                NTV2DMAEngine playbackMethod = NTV2_DMA2,
			  NTV2BoardType boardType= BOARDTYPE_UNKNOWN
				)
        : _boardNumber(boardNumber),
		  _boardType(boardType),
          _active(true) 

	{
		_thread.Resume ();
        _captureMethod = captureMethod;
        _playbackMethod = playbackMethod;
	}

    ~CNTV2AudioLoopBack() { Kill(); }
	void Quit();
	bool Active() { return _active; }


protected:
    virtual void InitThread ();
    virtual void Loop ();
    virtual void FlushThread () {_active=false;}

    ULWord ReadSamplesFromInput(ULWord newInputAddress);
    void   WriteSamplesToOutput(ULWord numSamples);

    ULWord DMASamplesFromInput(ULWord newInputAddress, NTV2DMAEngine dmaEngine);
    void   DMASamplesToOutput(ULWord numSamples, NTV2DMAEngine dmaEngine);

    void   InitializeAudioCapture();
    void   TerminateAudioCapture();
    void   DetectAudioFailure();

	UWord _boardNumber;
	NTV2BoardType _boardType;
    bool  _active;
	
    ULWord *_pCh2BaseAddress;
    ULWord _lastInputAddress;
    ULWord _lastOutputAddress;
    ULWord *_inputBuffer;		
	CNTV2Card _ntv2Card;
    NTV2DMAEngine _captureMethod;
    NTV2DMAEngine _playbackMethod;
	ULWord		  _audioFrameBuffer;
	ULWord		  _audioWrapAddress;
	ULWord		  _audioReadBufferOffset;
};

#endif
