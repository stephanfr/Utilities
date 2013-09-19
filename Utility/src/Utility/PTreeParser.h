/*
 * PTreeParser.h
 *
 *  Created on: Sep 3, 2013
 *      Author: steve
 */

#ifndef PTREEPARSER_H_
#define PTREEPARSER_H_


#include <string>
#include <list>
#include <map>
#include <iostream>

#include "Result.h"

#include <boost/property_tree/ptree.hpp>



namespace SEFUtility
{

	typedef std::list<std::string>									PTreeFieldList;
	typedef std::map<std::string, std::string>						PTreeFieldMap;
	typedef std::map<std::string, std::string>::const_iterator		PTreeFieldMapIterator;


	template <class ConfigType> class PTreeSectionParser
	{
	public :

		enum class ErrorCodes { SUCCESS = 0, MISSING_REQUIRED_FIELD };

		typedef ResultWithReturnPtr< ErrorCodes, ConfigType >	Result;

		static Result		Parse( const boost::property_tree::ptree::value_type& 		section )

		{
			std::unique_ptr<ConfigType>		configSettings( new ConfigType() );

			for( const std::string& requiredField : configSettings->requiredFields() )
			{
				if( section.second.find( requiredField ) == section.second.not_found() )
				{
					return( Result::Failure( ErrorCodes::MISSING_REQUIRED_FIELD, std::string( "Field: " ) + requiredField + std::string( " missing from section: " ) + section.first.data() ) );
				}
			}

			std::unique_ptr<PTreeFieldMap>		fieldMap( new PTreeFieldMap() );

			for( const boost::property_tree::ptree::value_type& field : section.second )
			{
				configSettings->SetField( field.first, field.second.get_value<std::string>() );
			}

			return( Result( configSettings ) );
		}
	};



	class PTreeConfigSettings
	{
	public :

		PTreeConfigSettings( const PTreeFieldList&&		requiredFields,
							 const PTreeFieldList&&		optionalFields )
			: m_requiredFields( requiredFields ),
			  m_optionalFields( optionalFields )
			  {};

		virtual ~PTreeConfigSettings() {};



		const PTreeFieldList&				requiredFields() const
		{
			return( m_requiredFields );
		}

		const PTreeFieldList&				optionalFields() const
		{
			return( m_optionalFields );
		}

		const PTreeFieldMapIterator			requiredFieldValue( const char*				fieldName ) const
		{
			return( requiredFieldValue( std::string( fieldName ) ));
		}

		const PTreeFieldMapIterator			requiredFieldValue( const std::string&		fieldName ) const
		{
			assert( std::find( m_requiredFields.begin(), m_requiredFields.end(), fieldName ) != m_requiredFields.end() );
			assert( m_fields.find( fieldName ) != m_fields.end() );

			return( m_fields.find( fieldName ) );
		}


		const PTreeFieldMap&				fields() const
		{
			return( m_fields );
		}

		const PTreeFieldMapIterator			fieldValue( const char*				fieldName ) const
		{
			return( fieldValue( std::string( fieldName ) ));
		}

		const PTreeFieldMapIterator			fieldValue( const std::string&		fieldName ) const
		{
			return( m_fields.find( fieldName ) );
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

		template <class ConfigType> friend typename PTreeSectionParser<ConfigType>::Result		PTreeSectionParser<ConfigType>::Parse( const boost::property_tree::ptree::value_type& );
	};



}	//	namespace SEFUtility


#endif	//	PTREEPARSER_H_
