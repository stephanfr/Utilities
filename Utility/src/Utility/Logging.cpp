/*
 * Logging.cpp
 *
 *  Created on: Sep 17, 2013
 *      Author: steve
 */


#include "Logging.h"

#include <boost/log/core.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/ptr_container/ptr_list.hpp>

#include <boost/atomic.hpp>



namespace SEFUtility
{

	static boost::atomic<long>		g_LoggingFilterChangeCount( 0 );


	BOOST_LOG_ATTRIBUTE_KEYWORD( severity, "Severity", LoggingSeverityLevels )


	std::ostream& operator<< ( std::ostream& outputStream, LoggingSeverityLevels level )
	{
		static const char* strings[] =
		{
			"trace",
			"info",
			"normal",
			"warning",
			"error",
			"critical",
			"fatal"
		};

		if( static_cast<std::size_t>( level ) < sizeof( strings ) / sizeof( *strings ))
		{
			outputStream << strings[static_cast<int>( level )];
		}
		else
		{
			outputStream << static_cast<int>( level );
		}

		return( outputStream );
	}


	boost::log::formatting_ostream& operator<< ( boost::log::formatting_ostream& outputStream, boost::log::to_log_manip< LoggingSeverityLevels, tag::severity > const& manip )
	{
		static const char* strings[] =
		{
			"[TRACE]   ",
			"[INFO]    ",
			"[NORMAL]  ",
			"[WARNING] ",
			"[ERROR]   ",
			"[CRITICAL]",
			"[FATAL]   "
		};

		LoggingSeverityLevels level = manip.get();

		if( static_cast<std::size_t>( level ) < sizeof( strings ) / sizeof( *strings ))
		{
			outputStream << strings[static_cast<int>( level )];
		}
		else
		{
			outputStream << static_cast<int>( level );
		}

		return( outputStream );
	}


	class Logger;


	class LoggingStreamImpl : public ILoggingStream
	{
	public :

		LoggingStreamImpl( Logger&						logger,
						   LoggingSeverityLevels		severityLevel );

		virtual ~LoggingStreamImpl() {};


		bool	isLogging() const
		{
			if( m_lastLoggingFilterChangeCount.load() < g_LoggingFilterChangeCount.load() )
			{
				UpdateIsLogging();
			}

			return( m_isLogging.load() );
		}


	private :

		void	UpdateIsLogging() const;



		Logger&							m_logger;
		LoggingSeverityLevels			m_severityLevel;

		mutable boost::atomic<bool>		m_isLogging;
		mutable boost::atomic<long>		m_lastLoggingFilterChangeCount;



	    friend ILoggingStream& 		ILoggingStream::operator<<( StandardEndLine		manip );
	};



	class Logger : public ILogger, public boost::log::sources::severity_channel_logger_mt<LoggingSeverityLevels, std::string>
	{
	public :

		Logger( const char*		className )
			: boost::log::sources::severity_channel_logger_mt<LoggingSeverityLevels, std::string>( boost::log::keywords::channel = std::string( " [" ) + std::string( className ) + std::string( "] ")),
			  m_traceStream( *this, LoggingSeverityLevels::trace ),
			  m_infoStream( *this, LoggingSeverityLevels::info ),
			  m_normalStream( *this, LoggingSeverityLevels::normal ),
			  m_warningStream( *this, LoggingSeverityLevels::warning ),
			  m_errorStream( *this, LoggingSeverityLevels::error ),
			  m_criticalStream( *this, LoggingSeverityLevels::critical ),
			  m_fatalStream( *this, LoggingSeverityLevels::fatal )
		{
		}


		virtual ~Logger() {};

		ILoggingStream&		GetStream( LoggingSeverityLevels		logLevel )
		{
			switch( logLevel )
			{
				case LoggingSeverityLevels::trace :
					return( m_traceStream );

				case LoggingSeverityLevels::info :
					return( m_infoStream );

				case LoggingSeverityLevels::normal :
					return( m_normalStream );

				case LoggingSeverityLevels::warning :
					return( m_warningStream );

				case LoggingSeverityLevels::error :
					return( m_errorStream );

				case LoggingSeverityLevels::critical :
					return( m_criticalStream );

				case LoggingSeverityLevels::fatal :
					return( m_fatalStream );
			}

			//	We should never get here, so assert if we do.  If this is non-debug, then return the normal stream.

			assert( 0 );

			return( m_normalStream );
		}


		ILoggingStream&		TraceStream()
		{
			return( m_traceStream );
		}

		ILoggingStream&		InfoStream()
		{
			return( m_infoStream );
		}

		ILoggingStream&		NormalStream()
		{
			return( m_normalStream );
		}

		ILoggingStream&		WarningStream()
		{
			return( m_warningStream );
		}

		ILoggingStream&		ErrorStream()
		{
			return( m_errorStream );
		}

		ILoggingStream&		CriticalStream()
		{
			return( m_criticalStream );
		}

		ILoggingStream&		FatalStream()
		{
			return( m_fatalStream );
		}



	private :

		LoggingStreamImpl			m_traceStream;
		LoggingStreamImpl			m_infoStream;
		LoggingStreamImpl			m_normalStream;
		LoggingStreamImpl			m_warningStream;
		LoggingStreamImpl			m_errorStream;
		LoggingStreamImpl			m_criticalStream;
		LoggingStreamImpl			m_fatalStream;
	};



	LoggingStreamImpl::LoggingStreamImpl( Logger&					logger,
										  LoggingSeverityLevels		severityLevel )
		: m_logger( logger ),
		  m_severityLevel( severityLevel ),
		  m_isLogging( true ),
		  m_lastLoggingFilterChangeCount( 0 )
	{
		UpdateIsLogging();
	}


	void	LoggingStreamImpl::UpdateIsLogging() const
	{
		m_lastLoggingFilterChangeCount.store( g_LoggingFilterChangeCount.load() );

		boost::log::record		testRecord = m_logger.open_record( boost::log::keywords::severity = m_severityLevel );

		m_isLogging.store( (bool)testRecord );
	}




    ILoggingStream& 	ILoggingStream::operator<<( StandardEndLine		manip )
    {
		LoggingStreamImpl&	streamImpl = static_cast<LoggingStreamImpl&>( *this );

    	BOOST_LOG_SEV( streamImpl.m_logger, streamImpl.m_severityLevel ) << m_buffer.str();

    	m_buffer.str( "" );
    	m_buffer.clear();

        return( *this );
    }


	typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> 	LoggingSink;


	void		Logging::Init()
	{
		boost::shared_ptr<LoggingSink>	consoleSink = boost::log::add_console_log();

		boost::log::formatter formatter =
			boost::log::expressions::stream
				<< boost::log::expressions::attr< unsigned int >("LineID") << ": "
				<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S") << "  "
				<< boost::log::expressions::attr< std::string >( "Channel" )
				<< severity
				<< boost::log::expressions::message;

		consoleSink->set_formatter( formatter );
		consoleSink->locked_backend()->auto_flush(true);

//		boost::log::core::get()->set_filter( severity >= LoggingSeverityLevels::info );

		boost::log::add_common_attributes();
	}


	void		Logging::FiltersChanged()
	{
		g_LoggingFilterChangeCount.fetch_add( 1 );
	}



	std::unique_ptr<ILogger>		Logging::GetLogger( const char*		channelName )
	{
		return( std::unique_ptr<ILogger>( new Logger( channelName ) ));
	}

}





