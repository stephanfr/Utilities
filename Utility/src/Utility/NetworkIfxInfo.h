/*
 * NetworkIfxInfo.h
 *
 *  Created on: Oct 7, 2013
 *      Author: steve
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
