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


#include "Logging.h"

#include <boost/log/core.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/from_settings.hpp>

#include <boost/ptr_container/ptr_list.hpp>

#include <boost/property_tree/ptree.hpp>

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




	class BoostLogManager
	{
	public :

		typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> 	LoggingSink;
		typedef	boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>		FileSink;


		BoostLogManager()
		{
			//	Create the console and file sinks.  The file sink is just a text file named 'startup' with a count suffix.

			boost::shared_ptr<LoggingSink>	consoleSink = boost::log::add_console_log();
			boost::shared_ptr<FileSink>		fileSink = boost::log::add_file_log("startup_%N.log");

			//	Set the formatter for the console and text file to the default and set autoflush as well.

			boost::log::formatter defaultFormatter =
				boost::log::expressions::stream
					<< boost::log::expressions::attr< unsigned int >("LineID") << ": "
					<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S") << "  "
					<< boost::log::expressions::attr< std::string >( "Channel" )
					<< severity
					<< boost::log::expressions::message;

			consoleSink->set_formatter( defaultFormatter );
			consoleSink->locked_backend()->auto_flush( true );

			fileSink->set_formatter( defaultFormatter );
			fileSink->locked_backend()->auto_flush( true );

			//	Add the common attributes

			boost::log::add_common_attributes();

			//	Write the first record to the log.

			Logger( "LoggingInitialization" ).NormalStream() << "Logging Initiated" << std::endl;
		}



		void		DefaultLoggingConfig( const char*			processName )
		{
			//	Start by stripping out all the existing sinks.  There should only be the startup console and file anyway.

			boost::log::core::get()->remove_all_sinks();

			//	Create the console sinks

			boost::shared_ptr<LoggingSink>	consoleSink = boost::log::add_console_log();

			//	Create the fileSink.  Use the process name with a counting suffix and a simple roll every 1MB rule.

			boost::shared_ptr<FileSink>	fileSink( new FileSink( boost::log::keywords::file_name = std::string( processName ) + "_%N.log",
																boost::log::keywords::rotation_size = 1048576 ));

			//	Create the default formatter and assign it to both sinks

			boost::log::formatter formatter =
				boost::log::expressions::stream
					<< boost::log::expressions::attr< unsigned int >("LineID") << ": "
					<< boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S") << "  "
					<< boost::log::expressions::attr< std::string >( "Channel" )
					<< severity
					<< boost::log::expressions::message;

			consoleSink->set_formatter( formatter );
			consoleSink->locked_backend()->auto_flush( true );

			fileSink->set_formatter( formatter );
			fileSink->locked_backend()->auto_flush( true );

			//	Default to only warnings and above.

			boost::log::core::get()->set_filter( severity >= LoggingSeverityLevels::warning );

			//	Add the common attributes

			boost::log::add_common_attributes();

			//	Output a warning message that the logging was not configured

			Logger( "DefaultLoggingConfig" ).WarningStream() << "No logging configuration specified.  Default logging config used." << std::endl;
		}

	};


	static BoostLogManager		g_boostLogManager;


	void		SetIfInitialized( boost::log::settings&						settings,
								  const std::string&						label,
								  boost::optional<const std::string&>		value )
	{
		if( value.is_initialized() )
		{
			settings[label] = value.get();
		}
	}


	Logging::InitResult				Logging::Init( const char*								processName,
							   	   	   	   	   	   const boost::property_tree::ptree&		loggingSection )
	{
		//	Logging configuration settings block

		boost::log::settings 		configSettings;

		configSettings["Core"]["DisableLogging"] = false;

		configSettings["Sinks.Console"]["Destination"] = "Console";
		configSettings["Sinks.Console"]["AutoFlush"] = true;

		//	Get a collection of all the logging files to open

		{
			auto	filesSection = loggingSection.get_child_optional( "files" );

			if( !filesSection.is_initialized() )
			{
				Logger( "LoggingConfigInit" ).ErrorStream() << "Missing 'files' section in logging configuration.  Using Default Config." << std::endl;

				g_boostLogManager.DefaultLoggingConfig( processName );

				return( InitResult::Failure( ErrorCodes::MISSING_FILES_SECTION, "Missing 'files' section in logging configuration.  Using Default Config." ) );
			}

			auto fileRange = filesSection->equal_range( "file" );

			for( auto itrFileSpec = fileRange.first; itrFileSpec != fileRange.second; ++itrFileSpec )
			{
				LogFileSpec		fileSpec;

				auto		parseResult = fileSpec.Parse( itrFileSpec->second );

				if( parseResult.Failed() )
				{
					Logger( "LoggingConfigInit" ).ErrorStream() << "Missing 'files' section in logging configuration.  Using Default Config." << std::endl;

					g_boostLogManager.DefaultLoggingConfig( processName );

					return( InitResult::Failure( ErrorCodes::BAD_FILE_SPEC, "Bad File Specification.  Using Default Config." ) );
				}

				std::string		prefix = std::string( "Sinks." ) + fileSpec.sink_name();

				configSettings[prefix + ".Destination"] = "TextFile";
				configSettings[prefix + ".FileName"] = fileSpec.file_name();
				configSettings[prefix + ".AutoFlush"] = fileSpec.auto_flush();

				SetIfInitialized( configSettings, prefix + std::string( ".Target" ), fileSpec.fieldValue( "target" ) );
				SetIfInitialized( configSettings, prefix + std::string( ".Filter" ), fileSpec.fieldValue( "filter" ) );
				SetIfInitialized( configSettings, prefix + std::string( ".Format" ), fileSpec.fieldValue( "format" ) );
			}
		}

		//	Start by stripping out all the existing sinks.  There should only be the startup console and file anyway.

		boost::log::core::get()->remove_all_sinks();

		//	Initialize from the settings we just pulled from the config

		boost::log::init_from_settings( configSettings );

		Logger( "LoggingConfigInit" ).NormalStream() << "Logging initialized from configuration file." << std::endl;

		//	Finished with Success

		return( InitResult::Success() );
	}



	void							Logging::DefaultConfiguration( const char*		processName )
	{
		g_boostLogManager.DefaultLoggingConfig( processName );
	}



	void							Logging::FiltersChanged()
	{
		g_LoggingFilterChangeCount.fetch_add( 1 );
	}



	std::unique_ptr<ILogger>		Logging::GetLogger( const char*		channelName )
	{
		return( std::unique_ptr<ILogger>( new Logger( channelName ) ));
	}

}





