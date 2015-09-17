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

#pragma once


#include <vector>

#include "ShortAlloc.h"




template <class T, int FIXED_SIZE>
class ShortVector
{
public :

	typedef short_alloc<T,FIXED_SIZE * sizeof(T)>		Allocator;
	typedef arena<FIXED_SIZE * sizeof(T)>				Arena;

	typedef std::vector<T, Allocator>					VectorBase;

	typedef typename VectorBase::iterator				iterator;
	typedef typename VectorBase::const_iterator			const_iterator;


	ShortVector()
		: m_vector( { Allocator( m_arena ) } )
	{}

	ShortVector( const ShortVector&		vectorToMove )
		: m_vector( { Allocator( m_arena ) } )
	{}

	~ShortVector()
	{}



	void		clear()
	{
		m_vector.clear();
	}


	void		push_back( const T&		value )
	{
		m_vector.push_back( value );
	}

	void		push_back( T&&			value )
	{
		m_vector.push_back( value );
	}

	bool		empty() const
	{
		return(m_vector.empty());
	}


	const T&			operator[]( size_t		index ) const
	{
		return( m_vector[index] );
	}


	iterator			begin()
	{
		return( m_vector.begin() );
	}

	const_iterator		begin() const
	{
		return( m_vector.begin() );
	}

	
	iterator			end()
	{
		return( m_vector.end() );
	}

	const_iterator		end() const
	{
		return( m_vector.end() );
	}


private :

	Arena				m_arena;

	VectorBase			m_vector;
};




