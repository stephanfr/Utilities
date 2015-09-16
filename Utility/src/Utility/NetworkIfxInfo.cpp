/*
Copyright (c) 2013 Stephan Friedl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above copyright holders
shall not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>


#include "NetworkIfxInfo.h"



namespace SEFUtility
{


	NetworkIfxInfo::GetInfoResult	NetworkIfxInfo::GetIfxInfo( const std::string&		ifxName )
	{
		struct ifreq 	ifr;
		int 			socketFD;


		//	Start by opening a socket, we get the file descriptor in return

		socketFD=socket(PF_INET, SOCK_STREAM, 0);
		if( -1 == socketFD )
		{
			return( GetInfoResult::Failure( ErrorCodes::SOCKET_FAILED, std::string( "Call to socket() failed with error: " ) + strerror( errno )));
		}

		//	Put the interface name into the if request structure

		strncpy( ifr.ifr_name, ifxName.c_str(), sizeof(ifr.ifr_name) - 1 );
		ifr.ifr_name[sizeof(ifr.ifr_name)-1]='\0';

		//	Get the IP address

		if( ioctl( socketFD, SIOCGIFADDR, &ifr ) == -1)
		{
			close( socketFD );

			return( GetInfoResult::Failure( ErrorCodes::IOCTL_SIOCGIFADDR_FAILED, std::string( "Call to ioctl() with SIOCGIFADDR failed with error: " ) + strerror( errno )));
		}

		m_IPAddress = inet_ntoa( ((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr );

		//	Get the net mask

		if( ioctl( socketFD, SIOCGIFNETMASK, &ifr ) == -1 )
		{
			close( socketFD );

			return( GetInfoResult::Failure( ErrorCodes::IOCTL_SIOCGIFNETMASK_FAILED, std::string( "Call to ioctl() with SIOCGIFNETMASK failed with error: " ) + strerror( errno )));
		}

		m_netMask = inet_ntoa( ((struct sockaddr_in *)(&ifr.ifr_netmask))->sin_addr );

		//	Get the MAC address

		if( ioctl( socketFD, SIOCGIFHWADDR, &ifr ) == -1 )
		{
			close( socketFD );

			return( GetInfoResult::Failure( ErrorCodes::IOCTL_SIOCGIFHWADDR_FAILED, std::string( "Call to ioctl() with SIOCGIFHWADDR failed with error: " ) + strerror( errno )));
		}

		{
			char mac[32];

			for( int j = 0, k = 0; j < 6; j++ )
			{
				k += snprintf( mac+k, sizeof(mac)-k-1, j ? ":%02X" : "%02X", (int)(unsigned int)(unsigned char)ifr.ifr_hwaddr.sa_data[j] );
			}
			mac[sizeof(mac)-1]='\0';

			m_macAddress = mac;
		}

		//	Close the socket

		close( socketFD );

		//	Finished with success

		return( GetInfoResult::Success() );
}


} /* namespace SEFUtility */
