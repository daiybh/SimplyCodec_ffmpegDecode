// DDRBaseControl.h
//
// Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
//
//

#pragma once

typedef enum {
 ACCOM_DDR,ABEKAS_DDR,SDL_DDR
} DDRType;

typedef enum {
 NO_CONNECTION,SERIAL_CONNECTION,ETHERNET_CONNECTION
} DDRConnectionType;

typedef enum {
 CONTROLTYPE_DDR,CONTROLTYPE_NTV
} ControlType;

/////////////////////////////////////////////////////////////////////////////
// CDDRBaseControl
//  This is the base class for Disk Recorder Control
//  Not meant to be used. 

class CDDRBaseControl
{

public: // Construct/Destruct
	CDDRBaseControl();
	~CDDRBaseControl();

public: // Methods
  virtual Boolean Open();
  virtual void    Close();
  virtual ULWord  GetCurrentFramefrom();
  virtual ULWord  GotoFrameByString(SByte *frameString);
  virtual ULWord  GotoFrameByInteger(ULWord frameNumber);
  virtual ULWord  RecordAtFrame(ULWord frameNumber);
  virtual ULWord  Play();
  virtual ULWord  ReversePlay();
  virtual ULWord  Stop();
  virtual ULWord  FastForward();
  virtual ULWord  Rewind();
  virtual ULWord  AdvanceFrame();
  virtual ULWord  BackFrame();
  virtual ULWord  Loop(ULWord startFrameNumber,ULWord endFrameNumber);

public: // Access Methods
  DDRConnectionType GetConnectionType();
  DDRType GetDDRType();
  
protected: // member variables
  DDRConnectionType m_connectionType;
  DDRType m_DDRType;	

private:

};

/////////////////////////////////////////////////////////////////////////////

