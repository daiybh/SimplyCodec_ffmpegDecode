// DDREthernetControl.h
//
// Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
//
//

#pragma once

#include "BaseMachineControl.h"

/////////////////////////////////////////////////////////////////////////////
// CDDREthernetControl
typedef enum {
 ACCOM_DDR,ABEKAS_DDR,SDL_DDR
} DDRType;

typedef enum {
 NO_CONNECTION,SERIAL_CONNECTION,ETHERNET_CONNECTION
} DDRConnectionType;

class CDDREthernetControl : public CBaseMachineControl
{

public: // Construct/Destruct
	CDDREthernetControl();
	~CDDREthernetControl();

public: // Methods
  virtual bool Open();
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

  LWord Command(SByte *cmdString, Boolean needAck);

protected: // Methods
  LWord writeNBytesToSocket(LWord socketDescriptor, SByte *ptr, LWord numBytes);
  LWord readNBytesFromSocket(LWord socketDescriptor, SByte *ptr, LWord numBytes );

protected: // member variables
  int m_socketFD;
  SByte   ack[10];

public: // Access Methods
  DDRConnectionType GetConnectionType();
  DDRType GetDDRType();
  
protected: // member variables
  DDRConnectionType m_connectionType;
  DDRType m_DDRType;	


};

/////////////////////////////////////////////////////////////////////////////

