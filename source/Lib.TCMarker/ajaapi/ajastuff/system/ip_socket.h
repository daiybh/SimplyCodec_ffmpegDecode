/**
	@file		ip_socket.h
	@copyright	Copyright (C) 2011-2016 AJA Video Systems, Inc.  All rights reserved.
	@brief		Declares the AJAIPSocket class.
**/

#ifndef AJA_IP_SOCKET_H
#define AJA_IP_SOCKET_H

/////////////////////////////
// Includes
/////////////////////////////
#include "ajastuff/common/public.h"
#include "ajastuff/system/system.h"
#include "ajastuff/system/lock.h"
#include <string>
#include <map>

#if defined(AJA_LINUX) || defined(AJA_MAC)
#	include <arpa/inet.h>
#	include <ifaddrs.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	include <poll.h>
#	include <sys/socket.h>
#elif defined(AJA_WINDOWS)
	typedef int socklen_t;
#endif

/////////////////////////////
// Typedefs
/////////////////////////////
// The key is the IP address, while the second member is the interface name
typedef std::map<std::string, std::string> IPAddressContainerType;


/////////////////////////////
// Declarations
/////////////////////////////
/**
 *	Class which initializes any TCP/IP stack requirements for the target OS.
 */
class AJA_EXPORT AJAIPSocket
{
	public:
		AJAIPSocket(void);
		virtual ~AJAIPSocket(void);

		AJAStatus Initialize(void);
		AJAStatus Deinitialize(void);

		virtual AJAStatus Open(const std::string& ipAddress, uint16_t port);
		bool              IsOpen(void);

		AJAStatus Shutdown(int how);
		AJAStatus Close(void);
		AJAStatus SetSocketOption(int option, const void* pValue);

		bool GetHostIPAddresses(
				IPAddressContainerType& ipv4AddressContainer,
				IPAddressContainerType& ipv6AddressContainer);

		virtual uint32_t Poll(
							uint8_t*            pData,
							uint32_t            dataLength,
							struct sockaddr_in& client,
							int                 timeout);
		virtual uint32_t Read(
							uint8_t*            pData,
							uint32_t            dataLength,
							struct sockaddr_in& client);
		virtual uint32_t Write(
							const uint8_t*      pData,
							uint32_t            dataLength,
							struct sockaddr_in& targetAddress);


	protected:
		struct sockaddr_in mSocketAddress;
		socklen_t          mSocketLength;
		int                mSocket; 

		bool IsInstantiated(void);

	private:
		static AJALock     mMutex;
		static uint32_t    mInstantiationCount;
};


#endif // AJA_IP_SOCKET_H
