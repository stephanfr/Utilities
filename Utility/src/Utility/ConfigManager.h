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


#ifndef PTREEPARSER_H_
#define PTREEPARSER_H_


#include <string>
#include <list>
#include <map>
#include <iostream>

#include <boost/property_tree/ptree.hpp>

#include "Result.h"



namespace SEFUtility
{

	class ConfigManager
	{
	public :

		enum class ErrorCodes { SUCCESS = 0,
								ERROR_LOADING_FILE,
								SECTION_NOT_FOUND,
								EXCEPTION_CAUGHT };

		typedef Result<ErrorCodes>														LoadResult;
		typedef ResultWithReturnRef<ErrorCodes, const boost::property_tree::ptree>		GetSectionResult;


		LoadResult				Load( const char*		filename );



		bool					hasSection( const char*		sectionName ) const;

		GetSectionResult		GetSection( const char*		sectionName ) const;


	private :

		boost::property_tree::ptree			m_configFile;
	};




	typedef std::list<std::string>									PTreeFieldList;
	typedef std::map<std::string, std::string>						PTreeFieldMap;



	class PTreeConfigSettings
	{
	public :

		PTreeConfigSettings( const PTreeFieldList&&		requiredFields,
							 const PTreeFieldList&&		optionalFields )
			: m_requiredFields( requiredFields ),
			  m_optionalFields( optionalFields )
			  {};

		virtual ~PTreeConfigSettings() {};


		enum class ErrorCodes { SUCCESS = 0,
								MISSING_REQUIRED_FIELD,
								EXCEPTION_CAUGHT };


		typedef Result<ErrorCodes>			ParseResult;


		ParseResult							Parse( const boost::property_tree::ptree&		sectionToParse );


		const PTreeFieldList&				requiredFields() const
		{
			return( m_requiredFields );
		}

		const PTreeFieldList&				optionalFields() const
		{
			return( m_optionalFields );
		}

		const std::string&					requiredFieldValue( const char*				fieldName ) const
		{
			return( requiredFieldValue( std::string( fieldName ) ));
		}

		const std::string&					requiredFieldValue( const std::string&		fieldName ) const
		{
			assert( std::find( m_requiredFields.begin(), m_requiredFields.end(), fieldName ) != m_requiredFields.end() );
			assert( m_fields.find( fieldName ) != m_fields.end() );

			return( m_fields.find( fieldName )->second );
		}


		const PTreeFieldMap&								fields() const
		{
			return( m_fields );
		}

		const boost::optional<const std::string&>			fieldValue( const char*				fieldName ) const
		{
			return( fieldValue( std::string( fieldName ) ));
		}

		const boost::optional<const std::string&>			fieldValue( const std::string&		fieldName ) const
		{
			std::map<std::string, std::string>::const_iterator	itrField = m_fields.find( fieldName );

			if( itrField != m_fields.end() )
			{
				const std::string&	value = (*itrField).second;

				return( boost::optional<const std::string&>( value ) );
			}

			return( boost::optional<const std::string&>() );
		}

		friend inline std::ostream& operator<<( std::ostream&		outputStream, const PTreeConfigSettings& settingsToWrite )
		{
			for( std::pair<std::string,std::string> currentField : settingsToWrite.fields() )
			{
				outputStream << "\"" << currentField.first << "\"" << " : " << "\"" << currentField.second << "\"" << std::endl;
			}

			return( outputStream );
		}


	private :

		void					SetField( const std::string&		name,
										  const std::string&		value )
		{
			m_fields.insert( std::pair<std::string,std::string>( name, value ));
		}


		const PTreeFieldList		m_requiredFields;
		const PTreeFieldList		m_optionalFields;

		PTreeFieldMap				m_fields;
	};



}	//	namespace SEFUtility


#endif	//	PTREEPARSER_H_
