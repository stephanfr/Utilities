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
