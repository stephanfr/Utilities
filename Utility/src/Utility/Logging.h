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



#ifndef LOGGING_H_
#define LOGGING_H_


#include <memory>
#include <iostream>
#include <sstream>
#include <ostream>

#include "ConfigManager.h"

#include <boost/assign/list_of.hpp>

#include <boost/algorithm/string/predicate.hpp>




//
//	The idea here is to ratchet down the complexity of the Boost::Log library to something
//		more digestible for run of the mill projects.  These classes provide a very light-weight
//		facade in front Boost::Log that permit use of the logging library without having to include
//		any of the Boost::Log header files in the project source code - or link with the Boost::Log
//		libraries across the board.
//
//	Needless to say, the gratuitous flexibility and power of boost logging libraries but if you need
//		something more than this you can always dip into Boost::Log directly for those features
//		you want to enable.
//


namespace SEFUtility
{

	enum class LoggingSeverityLevels
	{
		trace = 0,
		info,
		normal,
		warning,
		error,
		critical,
		fatal
	};


	//
	//	ILoggingStream is a base class that is used in place of the Boost::Log streams in user code.
	//		This class uses a string stream to roll up the 'message' payload for the logging record
	//		and then pushes out that buffer when std::endl is inserted into the stream
	//
	//	The support for std::endl was courtesy of GManNickG's (http://stackoverflow.com/users/87234/gmannickg)
	//		post on StackOverflow: http://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator

	class ILoggingStream
	{
	public :

		virtual ~ILoggingStream() {};


		virtual bool	isLogging() const = 0;

		//	The insertion operators (with the excpetion of std::endl below) are all declared and
		//		implemented in the header, along with the stringstream buffer to help insure that
		//		all of these operators are inlined in the end user's source code and not
		//		accessed through calls into the shared object.

		//	The template fucntion below doesn't seem too happy with substituting 'const char*' for 'T',
		//		so I simply declared an insertion operator for const char*.

		ILoggingStream&		operator<<( const char*		payload )
		{
			m_buffer << payload;

			return( *this );
		};

		template<typename T>
		ILoggingStream&		operator<<( const T&		payload )
		{
			m_buffer << payload;

			return( *this );
		};


		//
		//	The code below extends support for std::endl to this custom stream.
		//
		//		It isn't real transparent, but std::endl is actually a function of std::cout.
		//		To support std::endl for this custom stream, we have to define a function
		//		pointer type that matches the definition of std::endl for std::cout and
		//		the create an insertion operator for our stream that takes that function pointer.
		//

	    //	First define the type for std::cout

	    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

	    //	Next define the type for std::endl using the type for std::cout above

	    typedef CoutType& (*StandardEndLine)(CoutType&);

	    //	Finally, declare the insertion operator for the std::endl function pointer.

	    ILoggingStream& operator<<(StandardEndLine manip);


	protected :

	    ILoggingStream()
			: m_buffer()
		{}


	private :

		std::stringstream				m_buffer;
	};


	//
	//	ILogger is an abstract class used as a placeholder in the end user code in place of a Boost::Log
	//		logger.  The primary purpose of this class is to serve up different types of ILoggingStream
	//		streams for use in the end user code.
	//

	class ILogger
	{
	public :

		virtual ~ILogger() {};

		virtual ILoggingStream&		GetStream( LoggingSeverityLevels		logLevel ) = 0;

		virtual ILoggingStream&		TraceStream() = 0;
		virtual ILoggingStream&		InfoStream() = 0;
		virtual ILoggingStream&		NormalStream() = 0;
		virtual ILoggingStream&		WarningStream() = 0;
		virtual ILoggingStream&		ErrorStream() = 0;
		virtual ILoggingStream&		CriticalStream() = 0;
		virtual ILoggingStream&		FatalStream() = 0;
	};


	//
	//	Logging is used to hold two static functions, the logging initialization function and
	//		the ILogger factory.
	//

	class Logging
	{
	public :

		enum class ErrorCodes { SUCCESS = 0,
								MISSING_FILES_SECTION,
								BAD_FILE_SPEC,
								BAD_CORE_SETTINGS };

		typedef Result<ErrorCodes>		InitResult;


		static InitResult					Init( const char*								processName,
				   	   	   	  	  	  	  	  	  const boost::property_tree::ptree&		loggingSection );

		static void							DefaultConfiguration( const char*				processName );

		static void							FiltersChanged();

		static std::unique_ptr<ILogger>		GetLogger( const char*		channelName );
	};



	//
	//	Initialization specification classes
	//

	class CoreSettings : public PTreeConfigSettings
	{
	public :

		CoreSettings()
			: PTreeConfigSettings( std::list<std::string>(),
								   boost::assign::list_of( "disable_logging" )( "filter" ) )
		{}
	};



	class ConsoleSpec : public PTreeConfigSettings
	{
	public :

		ConsoleSpec()
			: PTreeConfigSettings( boost::assign::list_of( "sink_name" )( "auto_flush" ),
								   boost::assign::list_of( "filter" )( "format" )( "asynchronous" ) )
		{}

		const std::string&		sink_name() const
		{
			return( requiredFieldValue( "sink_name" ) );
		}

		bool					auto_flush() const
		{
			return( boost::iequals( requiredFieldValue( "auto_flush" ), "true" ) );
		}
	};


	class LogFileSpec : public PTreeConfigSettings
	{
	public :


		LogFileSpec()
			: PTreeConfigSettings( boost::assign::list_of( "sink_name" )( "file_name" )( "auto_flush" ),
								   boost::assign::list_of( "target" )( "rotation_size" )( "rotation_interval" )( "rotation_time_point" )( "filter" )( "asynchronous" )( "max_size" )( "min_free_space" )( "scan_for_files" )( "asynchronous" ) )
		{}


		const std::string&		sink_name() const
		{
			return( requiredFieldValue( "sink_name" ) );
		}

		const std::string&		file_name() const
		{
			return( requiredFieldValue( "file_name" ) );
		}

		bool					auto_flush() const
		{
			return( boost::iequals( requiredFieldValue( "auto_flush" ), "true" ) );
		}

	};

}




//
//	The following #defines are to be used in the end user code instead of calls directly into the
//		classes above.  The *_LOG defines check to make sure that the given stream is actually
//		enabled for logging before any of the payload elements are inserted into the stringstream.
//


#define DECLARE_LOGGER 							static std::unique_ptr<SEFUtility::ILogger>		__logger__;
#define INSTANTIATE_LOGGER( CLASS_NAME )		std::unique_ptr<SEFUtility::ILogger>	CLASS_NAME::__logger__( std::move( SEFUtility::Logging::GetLogger( #CLASS_NAME )));

#define LOCAL_LOGGER( LOCAL_NAME )				std::unique_ptr<SEFUtility::ILogger>	__logger__( std::move( SEFUtility::Logging::GetLogger( #LOCAL_NAME )));

#define TRACE_LOG 		if( __logger__->TraceStream().isLogging() ) __logger__->TraceStream()
#define INFO_LOG		if( __logger__->InfoStream().isLogging() ) __logger__->InfoStream()
#define LOG 			if( __logger__->NormalStream().isLogging() ) __logger__->NormalStream()
#define WARNING_LOG 	if( __logger__->WarningStream().isLogging() ) __logger__->WarningStream()
#define ERROR_LOG 		if( __logger__->ErrorStream().isLogging() ) __logger__->ErrorStream()
#define CRITICAL_LOG 	if( __logger__->CriticalStream().isLogging() ) __logger__->CriticalStream()
#define FATAL_LOG 		if( __logger__->FatalStream().isLogging() ) __logger__->FatalStream()



#endif /* LOGGING_H_ */
