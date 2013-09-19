/*
 * IndentTable.h
 *
 *  Created on: Sep 15, 2013
 *      Author: steve
 */

#ifndef INDENTTABLE_H_
#define INDENTTABLE_H_


#include <string>

#include <ostream>



namespace SEFUtility
{

	//
	//	This class is thanks to James Kanze (http://stackoverflow.com/users/649665/james-kanze) on StackOverflow:
	//
	//	http://stackoverflow.com/questions/9599807/how-to-add-indention-to-the-stream-operator
	//
	//
	//	Usage is simple:
	//
	//		ostream							outputStream;
	//		{
	//			IndentingOutputStreambuf		indent( outputStream );
	// 		}
	//
	//	When the instance 'indent' goes out of scope the indent level drops back to the prior level
	//


	class IndentingOutputStreambuf : public std::streambuf
	{
	public:

		const static int			DEFAULT_INDENT = 4;


		explicit		IndentingOutputStreambuf( std::streambuf* 	dest,
												  int 				indent = DEFAULT_INDENT )
	        : m_dest( dest )
	        , m_atStartOfLine( true )
	        , m_indent( indent, ' ' )
	        , m_owner( NULL )
	    {}

	    explicit		IndentingOutputStreambuf( std::ostream& 	dest,
	    										  int 				indent = DEFAULT_INDENT )
	        : m_dest( dest.rdbuf() )
	        , m_atStartOfLine( true )
	        , m_indent( indent, ' ' )
	        , m_owner( &dest )
	    {
	        m_owner->rdbuf( this );
	    }

	    virtual			~IndentingOutputStreambuf()
	    {
	        if( m_owner != NULL )
	        {
	            m_owner->rdbuf( m_dest );
	        }
	    }

	protected:

	    virtual int         overflow( int		ch )
	    {
	        if ( m_atStartOfLine && ( ch != '\n' ))
	        {
	            m_dest->sputn( m_indent.data(), m_indent.size() );
	        }

	        m_atStartOfLine = ( ch == '\n' );

	        return( m_dest->sputc( ch ));
	    }


	private :

	    std::streambuf*     m_dest;
	    bool                m_atStartOfLine;
	    std::string         m_indent;
	    std::ostream*       m_owner;
	};

}



#endif /* INDENTTABLE_H_ */
