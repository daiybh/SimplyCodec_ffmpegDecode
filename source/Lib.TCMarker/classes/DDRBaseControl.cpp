// DDRControl.cpp
// 
// Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
//
//
#include "StdAfx.h"
#include "DDRBaseControl.h"

CDDRBaseControl::CDDRBaseControl()
{

}

CDDRBaseControl::~CDDRBaseControl()
{

}

Boolean CDDRBaseControl::Open()
{
  return false;
}

void    CDDRBaseControl::Close()
{

}

ULWord  CDDRBaseControl::GetCurrentFramefrom()
{
  return -1;
}

ULWord  CDDRBaseControl::GotoFrameByString(SByte *frameString)
{
  return -1;
}

ULWord  CDDRBaseControl::GotoFrameByInteger(ULWord frameNumber)
{
  return -1;
}

ULWord  CDDRBaseControl::RecordAtFrame(ULWord frameNumber)
{
  return -1;
}

ULWord  CDDRBaseControl::Play()
{
  return -1;
}

ULWord  CDDRBaseControl::ReversePlay()
{
  return -1;
}

ULWord  CDDRBaseControl::Stop()
{
  return -1;
}

ULWord  CDDRBaseControl::FastForward()
{
  return -1;
}

ULWord  CDDRBaseControl::Rewind()
{
  return -1;
}

ULWord  CDDRBaseControl::AdvanceFrame()
{
  return -1;
}

ULWord  CDDRBaseControl::BackFrame()
{
  return -1;
}

ULWord  CDDRBaseControl::Loop(ULWord startFrameNumber,ULWord endFrameNumber)
{
  return -1;
}

DDRConnectionType CDDRBaseControl::GetConnectionType()
{
  return m_connectionType;
}

DDRType CDDRBaseControl::GetDDRType()
{
  return m_DDRType;
}

