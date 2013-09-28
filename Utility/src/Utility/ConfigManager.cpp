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



#include "ConfigManager.h"
#include "Logging.h"

#include <boost/property_tree/json_parser.hpp>

#include <boost/lexical_cast.hpp>




namespace SEFUtility
{

	ConfigManager::LoadResult			ConfigManager::Load( const char*		filename )
	{
		LOCAL_LOGGER( "SEFUtility::ConfigFileLoader" )


		//	Start by reading and parsing the config file

		try
		{
			boost::property_tree::read_json( filename, m_configFile );
		}
		catch( const boost::property_tree::json_parser::json_parser_error&		exception )
		{
			std::string		fullMessage = exception.message() + std::string( "    Filename: " ) + exception.filename() + std::string( "    Line: " ) + boost::lexical_cast<std::string>( exception.line() );

			CRITICAL_LOG << "Caught Exception Reading JSON configuration file.  Reason: " <<  fullMessage << std::endl;

			return( LoadResult::Failure( ErrorCodes::ERROR_LOADING_FILE, fullMessage ) );
		}
		catch( ... )
		{
			FATAL_LOG << "Unexpected Exception Caught." << std::endl;

			return( LoadResult::Failure( ErrorCodes::EXCEPTION_CAUGHT, "Unexpected Exception Caught" ) );
		}

		return( LoadResult::Success() );
	}


	bool											ConfigManager::hasSection( const char*		sectionName ) const
	{
		boost::optional<const boost::property_tree::ptree&> ptreeSection = m_configFile.get_child_optional( sectionName );

		return( ptreeSection.is_initialized() );
	}


	ConfigManager::GetSectionResult					ConfigManager::GetSection( const char*		sectionName ) const
	{
		boost::optional<const boost::property_tree::ptree&> ptreeSection = m_configFile.get_child_optional( sectionName );

		if( !ptreeSection.is_initialized() )
		{
			return( GetSectionResult::Failure( ErrorCodes::SECTION_NOT_FOUND, std::string( "Section: " ) + sectionName + " Not Found." ) );
		}

		return( GetSectionResult::Success( *ptreeSection ));
	}


	PTreeConfigSettings::ParseResult				PTreeConfigSettings::Parse( const boost::property_tree::ptree&		sectionToParse )
	{
		for( const std::string& requiredField : m_requiredFields )
		{
			if( sectionToParse.find( requiredField ) == sectionToParse.not_found() )
			{
				return( ParseResult::Failure( ErrorCodes::MISSING_REQUIRED_FIELD, std::string( "Missing Required Field: " ) + requiredField ));
			}
		}

		for( const boost::property_tree::ptree::value_type& field : sectionToParse )
		{
			SetField( field.first, field.second.get_value<std::string>() );
		}

		return( ParseResult::Success() );
	}

}

