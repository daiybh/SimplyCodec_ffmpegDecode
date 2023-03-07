// DDREthernetControl.cpp
// 
// Copyright (C) 2004 AJA Video Systems, Inc.  Proprietary and Confidential information.
//
//
#include "DDREthernetControl.h"

#define SERV_TCP_PORT           2929 /* Accom Dedicated unpriveleged RTD socket */
#define ACCOM_SERV_TCP_PORT     2929 /* Accom Dedicated unpriveleged RTD socket */
#define SDL_SERV_TCP_PORT       5678 /* SDL Dedicated unpriveleged RTD socket */
#define ABEKAS_SERV_TCP_PORT    2929 /* ABEKAS Dedicated unpriveleged RTD socket */

CDDREthernetControl::CDDREthernetControl()
{
  m_socketFD = 0;
}

CDDREthernetControl::~CDDREthernetControl()
{

}

bool CDDREthernetControl::Open()
{
    struct sockaddr_in	serverAddress;
    struct hostent	*hostEntry;
    u_short portNumber;
    char ipAddressString[40];
	
    if ( m_socketFD != 0 )
    {  // already open
       return true;  	
    }

    // FORCE SDL_DDR for Now!!!
    m_DDRType = SDL_DDR;
    switch ( m_DDRType )
    {
    	case ACCOM_DDR:
    		portNumber = ACCOM_SERV_TCP_PORT;
    	break;
    	case ABEKAS_DDR:
    		portNumber = ABEKAS_SERV_TCP_PORT;
    	break;
    	default:
    	case SDL_DDR:
    		portNumber = SDL_SERV_TCP_PORT;
    	break;
    }
	
    if ( (hostEntry = gethostbyname((char *)"209.77.77.92")) != NULL )
    {
    	sprintf(ipAddressString,"%s",
	    	inet_ntoa(*((struct in_addr *)(*hostEntry->h_addr_list))));
    }
    else
    {
    	// perhaps they entered the IP address
    	strcpy(ipAddressString,(char *)"209.77.77.92");
    }
 

    ZeroMemory((SByte *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ipAddressString);
    serverAddress.sin_port = htons(portNumber);
    
    if (( m_socketFD = socket(AF_INET,SOCK_STREAM, 0)) < 0 )
    {
		//printf("Cannot open stream socket\n");
		m_socketFD = 0;
		return false;
    }
   
    if ( connect(m_socketFD,
		 (struct sockaddr *)&serverAddress,
	         sizeof(serverAddress)) < 0 )
    {
		//printf("Cannot connect to DDR\n");
		m_socketFD = 0;
		return false;
    }

    return true;  
}

void    CDDREthernetControl::Close()
{
	if ( m_socketFD != 0)
	{
    	closesocket(m_socketFD);
		m_socketFD = 0;
    }
}

ULWord  CDDREthernetControl::GetCurrentFramefrom()
{ 
	long numBytesRead=0;
	ULWord currentFrame=0;
	SByte buffer[100];
	SByte currentByte;

	 strcpy(buffer,"");
	 Command("where\n",false);
	 readNBytesFromSocket(m_socketFD,&currentByte,1);
	 while ( currentByte != '\n' )
	 {	
		if (isdigit(currentByte) )
	 		buffer[numBytesRead++] = currentByte;
	 		
	  	readNBytesFromSocket(m_socketFD,&currentByte,1);
	  	if ( numBytesRead > (sizeof(buffer) -1) )
	  		break;
	 }
	 buffer[numBytesRead] = '\0';

	 currentFrame = atoi(buffer);
	 return currentFrame;	 

}

ULWord  CDDREthernetControl::GotoFrameByString(SByte *frameString)
{
	SByte commandString[40];
	strcpy(commandString,"goto ");
	strcat(commandString,frameString);
	strcat(commandString,"\n");
	Command(commandString, true);

	return true;

}

ULWord  CDDREthernetControl::GotoFrameByInteger(ULWord frameNumber)
{
	SByte commandString[40];
	
	sprintf(commandString,"goto %d\n",frameNumber);
	Command(commandString,true);
	
	return true;

}

ULWord  CDDREthernetControl::RecordAtFrame(ULWord frameNumber)
{
	SByte commandString[40];
	Sleep(100); 
	sprintf(commandString,"record input %d\n",frameNumber);
	Command(commandString,true);
	// Delay to make sure frame is captured.
	Sleep(200);	

	return true;
}

ULWord  CDDREthernetControl::Play()
{
	SByte commandString[40];
	
	sprintf(commandString,"play 1\n");
	Command(commandString,true);

	return true;

}

ULWord  CDDREthernetControl::ReversePlay()
{
	SByte commandString[40];
	
	sprintf(commandString,"play -1\n");
	Command(commandString,true);

	return true;

}

ULWord  CDDREthernetControl::Stop()
{
	SByte commandString[40];
	
	sprintf(commandString,"stop\n");
	Command(commandString,true);
	// Delay to make sure command has processed...of course no guarantee with
	// ethernet
	Sleep(1000);

	return GetCurrentFramefrom();
}

ULWord  CDDREthernetControl::FastForward()
{
	SByte commandString[40];
	
	sprintf(commandString,"play 30\n");
	Command(commandString,true);

	return true;

}

ULWord  CDDREthernetControl::Rewind()
{
	SByte commandString[40];
	
	sprintf(commandString,"play -30\n");
	Command(commandString,true);

	return true;

}

ULWord  CDDREthernetControl::AdvanceFrame()
{
	SByte commandString[40];
	
	sprintf(commandString,"jog 1\n");
	Command(commandString,true);
	Sleep(500);

	return GetCurrentFramefrom();

}       

ULWord  CDDREthernetControl::BackFrame()
{
	SByte commandString[40];
	
	sprintf(commandString,"jog -1\n");
	Command(commandString,true);
	Sleep(500);

	return GetCurrentFramefrom();

}

ULWord  CDDREthernetControl::Loop(ULWord startFrameNumber,ULWord endFrameNumber)
{
	SByte commandString[60];
	
	// Need to stop before you loop or disk recorder hangs.
	Stop();
	if ( startFrameNumber < endFrameNumber )
	{
		sprintf(commandString,"loop %d %d\n",startFrameNumber,endFrameNumber);
		Command(commandString,true);
		return true;
	}
	else
		return false;

}

LWord CDDREthernetControl::Command(SByte *cmdString, Boolean needAck)
{
    LWord length;
    
    length = strlen(cmdString);
    
    if ( length > 0 )
    {
	if ( writeNBytesToSocket(m_socketFD,cmdString,length) != length )
	{
	    //printf("Command %s pooped\n");
	    return -1;
	}

	if ( needAck ) 
	{
	    
	    if ( readNBytesFromSocket(m_socketFD, ack, 1 ) < 0 )
	    {
			//printf("No Ack %d\n",errno);
			return -1;
	    }
	}
	    
    }
    return 0;
    
}





LWord CDDREthernetControl::readNBytesFromSocket(LWord socketDescriptor, SByte *ptr, LWord numBytes )
{
    LWord numLeft, numRead;
    
    numLeft = numBytes;
    
    while(numLeft > 0 )
    {
	numRead = recv(socketDescriptor,ptr,numLeft,0);

	if ( numRead < 0 )
	    return (numRead);
	else if ( numRead == 0 )
	    break; /* EOF */

	numLeft -= numRead;
	ptr += numRead;
	
    }
    return (numBytes - numLeft);
}

LWord CDDREthernetControl::writeNBytesToSocket(LWord socketDescriptor, SByte *ptr, LWord numBytes)
{
    LWord numLeft,numWritten;

    numLeft = numBytes;
    
    while (numLeft > 0 ) 
    {
	numWritten = send(socketDescriptor, ptr,numLeft,0);
	if ( numWritten < 0 )
	{
	    return (numWritten);
	}
	numLeft -= numWritten;
	ptr += numWritten;
    }
    return (numBytes - numLeft);
    
}
