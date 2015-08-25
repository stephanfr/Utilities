

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

	~ShortVector()
	{}


	void		push_back( const T&		value )
	{
		m_vector.push_back( value );
	}

	void		push_back( T&&			value )
	{
		m_vector.push_back( value );
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




