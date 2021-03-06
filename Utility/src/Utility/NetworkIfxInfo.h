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

#ifndef NETWORKIFXINFO_H_
#define NETWORKIFXINFO_H_


#include "Result.h"



namespace SEFUtility
{

	class NetworkIfxInfo
	{
	public:

		NetworkIfxInfo() {};
		virtual ~NetworkIfxInfo() {};


		enum class ErrorCodes { SUCCESS = 0, SOCKET_FAILED, IOCTL_SIOCGIFADDR_FAILED, IOCTL_SIOCGIFNETMASK_FAILED, IOCTL_SIOCGIFHWADDR_FAILED };


		typedef Result<ErrorCodes> GetInfoResult;

		GetInfoResult	GetIfxInfo( const std::string&		ifxName );



		const std::string&		IPAddress() const
		{
			return( m_IPAddress );
		}

		const std::string&		MACAddress() const
		{
			return( m_macAddress );
		}

		const std::string&		NetMask() const
		{
			 return( m_netMask );
		}

	private :

		std::string			m_IPAddress;
		std::string			m_netMask;
		std::string			m_macAddress;

	};

} /* namespace SecurityEvents */
#endif /* NETWORKIFXINFO_H_ */
