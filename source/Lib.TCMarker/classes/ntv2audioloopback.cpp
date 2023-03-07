/*

  Module : CNTV2AudioLoopBack.cpp

  Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
                                                                                  
  This software is provided by AJA Video, Inc. "AS IS" with no express or implied warranties.

  Demo class that reads audio from the input buffer and places it in
  the output buffer for NTV2(KHD,KSD).

  We use the 'Audio' (50 Hz) Interrupt simply as a way to measure time
  while we're waiting for the buffer to fill up with more data.

  Please refer to the NTV2Demo project in the NTV2SDK for an example of how this class is
  used.


  To use this class the user must first instantiate an object of this type as follows:

	CNTV2AudioLoopBack _ntv2AudioLoopBack  ;
	_ntv2AudioLoopBack = new CNTV2AudioLoopBack(
												0,						// Board number
												AudioDMACaptureMode,	// DMAEngine
												AudioDMAPlaybackMode,	// DMAEngine
												);

  Once the above class is instantiated the thread that it creates begins to run immediately.  
  There is really nothing for the client to do after this other than destroy this object when 
  the user closes the application.  An example of closing this thread is as follows:


	if ( _ntv2AudioLoopBack != NULL )
	{
        _ntv2AudioLoopBack->Quit();
		delete _ntv2AudioLoopBack;
		Sleep(500);
		_ntv2AudioLoopBack = NULL;
	}

  Note that it is important to call CNTV2AudioLoopBack::Quit() before deleting the object.

*/


#include "ntv2audioloopback.h"
#include "ntv2Card.h"
#include "ntv2devicefeatures.h"
#include "fixed.h"
#include "performanceCounter.h"


void CNTV2AudioLoopBack::InitThread()
{
    _lastInputAddress = 0;
    _lastOutputAddress = 0;
    _pCh2BaseAddress = NULL;
    _inputBuffer = NULL;
}

 
void CNTV2AudioLoopBack::Loop()
{
	_ntv2Card.Open(_boardNumber,false,_boardType);
	
	if ( _ntv2Card.BoardOpened() )
	{
		_audioFrameBuffer = _ntv2Card.GetAudioFrameBufferNumber();  
		_ntv2Card.GetAudioWrapAddress(&_audioWrapAddress);
		_ntv2Card.GetAudioReadOffset(&_audioReadBufferOffset);

	    _ntv2Card.EnableAudioInterrupt();  
        // note that we don't turn it off when we exit... (it's harmless, and it might have been enabled when we entered anyway.!)

	    _ntv2Card.SubscribeAudioInterruptEvent();  

        // Use Channel 2 Access so Channel 1 can be used for video
        ULWord oldPCIAccessFrame;
	    _ntv2Card.GetPCIAccessFrame(NTV2_CHANNEL2,&oldPCIAccessFrame);
	    _ntv2Card.SetPCIAccessFrame(NTV2_CHANNEL2,_audioFrameBuffer);
	    _ntv2Card.GetBaseAddress(NTV2_CHANNEL2,&_pCh2BaseAddress);

        // Start Capturing and wait for the Audio Input buffer to fill up a little.
        InitializeAudioCapture();

        ULWord audioInputAddress;
        // audioInputAddress is the last byte of the last 128-byte audio sample written by the hardware.
        _ntv2Card.ReadAudioLastIn(&audioInputAddress);

        if (audioInputAddress == _lastInputAddress)
        {
            // no audio has been captured... perhaps this board does not support audio.
            // In this case, audioInputAddress is not valid...  so we should not continue on using it!
            // (ReadAudioLastIn returns the address of the last byte written by hardware...
            //  which means that the next address to write to should be the last address + 1...
            //  except in the initial condition, when the register reads 0, and the next address to be written is also 0...
            //  [this is a hardware fluke], so we must not "+1" if we're still in the initial condition!)
            MessageBox(0 ,"Board may not support audio.","Audio Capture Failed",MB_OK);
        }
        else
        {
            ULWord numSamples =0;
            _inputBuffer = new ULWord[0x100000];
            //_inputBuffer = new ULWord[(audioInputAddress+1)/sizeof(ULWord)];
            switch (_captureMethod)
            {
            case NTV2_PIO:
                numSamples = ReadSamplesFromInput(audioInputAddress+1);
                break;
            case NTV2_DMA1:
            default:
                NTV2DMAEngine dmaEngine = _captureMethod;
                numSamples = DMASamplesFromInput(audioInputAddress+1, dmaEngine);
                break;
            }

            bool done = false;
            while ( !done )
            {
                _ntv2Card.WaitForAudioInterrupt();
                if (_playbackMethod == NTV2_PIO)
                {
                    WriteSamplesToOutput(numSamples);
                }
                else
                {
                    DMASamplesToOutput(numSamples, _playbackMethod);
                }

                if ( _isDying )
                {
                    done = true;
                }
                else
                {
                    //DetectAudioFailure();  No longer necessary

                    _ntv2Card.ReadAudioLastIn(&audioInputAddress);
                    if (_captureMethod == NTV2_PIO)
                    {
                        numSamples = ReadSamplesFromInput(audioInputAddress+1);
                    }
                    else
                    {
                        numSamples = DMASamplesFromInput(audioInputAddress+1, _captureMethod);
                    }
                }

                // We have a duty to check for isDying about every Vertical Interval...
                // otherwise, the thread may be killed via a time-out, and then we have no thread!
                if ( _isDying )
                {
                    done = true;
                }
            }
        }

        TerminateAudioCapture();

	    _ntv2Card.SetPCIAccessFrame(NTV2_CHANNEL2,oldPCIAccessFrame);   
        
        delete [] _inputBuffer;
        _inputBuffer = NULL;

	}
	_active = false;
}

// ReadSamplesFromInput 
// Reads samples in from _lastInputAddress to newInputAddress
ULWord CNTV2AudioLoopBack::ReadSamplesFromInput(ULWord newInputAddress)
{
    assert(_pCh2BaseAddress);
    ULWord numSamplesRead = 0;
   
    if ( newInputAddress == _audioWrapAddress) 
        newInputAddress = 0;

    ULWord* hardwareAddress = _pCh2BaseAddress + ((_lastInputAddress+_audioReadBufferOffset)/4);
    while ( _lastInputAddress != newInputAddress )
    {
        _inputBuffer[numSamplesRead] = *hardwareAddress++;
        _lastInputAddress += 4;
        if ( hardwareAddress == ( _pCh2BaseAddress + ((_audioWrapAddress + _audioReadBufferOffset)/4)) )
        {
            // reset to start of buffer
            hardwareAddress =  _pCh2BaseAddress + ((_audioReadBufferOffset)/4);
            _lastInputAddress = 0;
        }
        numSamplesRead++;
    }

    return numSamplesRead;
}

// DMASamplesFromInput 
// Reads samples in from lastInputAddress to newInputAddress
ULWord CNTV2AudioLoopBack::DMASamplesFromInput(ULWord newInputAddress, NTV2DMAEngine dmaEngine)
{
    ULWord bytesRead = 0;

    ULWord frame = _audioFrameBuffer;

    if ( newInputAddress > _lastInputAddress )
    {
        ULONG dmaBytes = newInputAddress - _lastInputAddress;
        _ntv2Card.DmaRead(dmaEngine, frame, _inputBuffer,
                          _audioReadBufferOffset + _lastInputAddress, dmaBytes);
        bytesRead = dmaBytes;

        _lastInputAddress += bytesRead;
    }
    else if ( newInputAddress < _lastInputAddress )
    {
        // a Wrap will be required to reach the target address.

        ULONG dmaBytes = _audioWrapAddress - _lastInputAddress;
        if ( _lastInputAddress != _audioWrapAddress)
        {
            _ntv2Card.DmaRead(dmaEngine, frame, _inputBuffer,
                              _audioReadBufferOffset + _lastInputAddress, dmaBytes);
            bytesRead += dmaBytes;
        }

        dmaBytes = newInputAddress;
        if (dmaBytes != 0)
        {
            _ntv2Card.DmaRead(dmaEngine, frame, _inputBuffer + (bytesRead/4),
                              _audioReadBufferOffset, dmaBytes);
            bytesRead += dmaBytes;

            _lastInputAddress = dmaBytes;
        }
        else
        {
            _lastInputAddress = 0;
        }
    }

    ULWord numSamplesRead = bytesRead/4;
    return numSamplesRead;
}

void CNTV2AudioLoopBack::WriteSamplesToOutput(ULWord numSamples)
{
    assert(_pCh2BaseAddress);

    ULWord* hardwareAddress = _pCh2BaseAddress + (_lastOutputAddress/4);

    for ( ULWord sampleCount = 0; sampleCount < numSamples; sampleCount++ )
    {
        *hardwareAddress++ =  _inputBuffer[sampleCount];
        _lastOutputAddress += 4;
        if ( hardwareAddress == ( _pCh2BaseAddress + (_audioWrapAddress /4)) )
        {
            // reset to start of buffer
            _lastOutputAddress = 0;
            hardwareAddress = _pCh2BaseAddress + (_lastOutputAddress/4);

        }
    }
}

///////////////////////////////////////////////////////////////
// get data from _inputBuffer and write it to the card.
///////////////////////////////////////////////////////////////
void CNTV2AudioLoopBack::DMASamplesToOutput(ULWord numSamples, NTV2DMAEngine dmaEngine)
{
    ULWord bytesWritten = 0;
    ULWord frame = _audioFrameBuffer;

    if ( (_lastOutputAddress+(numSamples*4)) < _audioWrapAddress )
    {
        ULONG dmaBytes = numSamples*4;
        _ntv2Card.DmaWrite(dmaEngine, frame, _inputBuffer,
                          _lastOutputAddress, dmaBytes);
        bytesWritten = dmaBytes;

        _lastOutputAddress += bytesWritten;
    }
    else if ( (_lastOutputAddress+(numSamples*4)) >= _audioWrapAddress )
    {
        // a Wrap will be required to reach the target address.

        ULONG dmaBytes = _audioWrapAddress - _lastOutputAddress;
        if ( dmaBytes != 0)
        {
            _ntv2Card.DmaWrite(dmaEngine, frame, _inputBuffer,
                              _lastOutputAddress, dmaBytes);
            bytesWritten += dmaBytes;
        }

        dmaBytes = (numSamples*4) - bytesWritten;
        if (dmaBytes != 0)
        {
            _ntv2Card.DmaWrite(dmaEngine, frame, _inputBuffer + (bytesWritten/4),
                              0, dmaBytes);
            bytesWritten += dmaBytes;

            _lastOutputAddress = dmaBytes;
        }
        else
        {
            _lastOutputAddress = 0;
        }
    }

}

///////////////////////////////////////////////////////////////
// InitializeAudioCapture
// Reset Audio machine, turn Capture on, and wait for some data.
///////////////////////////////////////////////////////////////
void CNTV2AudioLoopBack::InitializeAudioCapture()
{
    ULWord audioControl;
	_ntv2Card.ReadAudioControl(&audioControl);
	audioControl |= 0x300;		// Set "reset Audio Input" and "reset Audio Output" bits
  	audioControl &= ~0x1;		// Turn Capture Off
	_ntv2Card.WriteAudioControl(audioControl);

	audioControl &= ~0x300;		// Clear "reset Audio Input" and "reset Audio Output" bits
	audioControl |= 0x1;		// Set Capture bit
    
    _ntv2Card.WaitForAudioInterrupt();           
    _ntv2Card.WriteAudioControl(audioControl);   // Start capturing

    int delay = 5;  // number of interrupts to wait for input data to fill the buffer...
                   // big number will cause the audio output to 'lag behind' real-time...
                   // small number will cause output to 'run out of data' 
                   // when the CPU becomes Busy (because of some other program).
                   // (WHICH PUTS THE AUDIO H/W INTO A BAD STATE!!).
                   // If this program is run as a HIGH PRIORITY task,
                   // then a small number can be used without worry of 'running out of data'.
    for (int i = 0; i<delay; i++)
    {
        //_ntv2Card.WaitForVerticalInterrupt();           // Wait for interrupt
        _ntv2Card.WaitForAudioInterrupt();           // Wait for 50 Hz interrupt

        // 
        if ( _isDying )
        {
	        _active = false;
            return;
        }
    }
}

///////////////////////////////////////////////////////////////
// TerminateAudioCapture
///////////////////////////////////////////////////////////////
void CNTV2AudioLoopBack::TerminateAudioCapture()
{
    ULWord audioControl;

    _ntv2Card.ReadAudioControl(&audioControl);
	audioControl |= 0x300;		// Set "reset Audio Input" and "reset Audio Output" bits
  	audioControl &= ~0x1;		// Turn Capture Off
	_ntv2Card.WriteAudioControl(audioControl);
}

///////////////////////////////////////////////////////////////
// DetectAudioFailure  (deprecated)
// Formerly - checked the status bits, and reset if there was a failure.
///////////////////////////////////////////////////////////////
void CNTV2AudioLoopBack::DetectAudioFailure()
{
    // The former 'Status' bits (bits 11 and 12) are no longer in use,
    // and have been re-allocated as "Audio Output Pause" and "Audio Output Last Address Mode".
    // So, we really don't want to reset based on these bits!
/*
    ULWord audioControl;

	_ntv2Card.ReadAudioControl(&audioControl);
    if ( (audioControl & 0x1800) == 0x1800)
    {
        // Audio Status bits = 3 indicates 'FIFO Went Empty'.
        // The audio machine ran out of data in the output buffer.
        // We didn't keep it full enough!  Bail out for emergency landing.
        InitializeAudioCapture();
		if( NULL !=_pLogWindow )
			_pLogWindow->Log("Audio Output buffer ran dry!\n");
    }
*/
}

void CNTV2AudioLoopBack::Quit()
{
	_isDying++;
	while ( _active )
		Sleep(100);
}
