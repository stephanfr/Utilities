#pragma once


#include <memory>



template <typename T>
class FastStack
{
public :

	FastStack( unsigned int		initialSize )
		: m_size( initialSize + 2 ),
		  m_limit( initialSize ),
		  m_nextEmptyElement( 0 )
	{
		m_storage = (T*)_aligned_malloc( sizeof(T) * m_size, __alignof(T) );
	}

	~FastStack()
	{
		_aligned_free( m_storage );
	}



	bool		isEmpty() const
	{
		return( m_nextEmptyElement == 0 );
	}


	void		push( const T&		newElement )
	{
		m_storage[m_nextEmptyElement++] = newElement;
	
		if( m_nextEmptyElement >= m_limit )
		{
			growStorage();
		}
	}

	void		push2( const T&		newElement1,
					   const T&		newElement2 )
	{
		m_storage[m_nextEmptyElement++] = newElement1;
		m_storage[m_nextEmptyElement++] = newElement2;
	
		if( m_nextEmptyElement >= m_limit )
		{
			growStorage();
		}
	}


	bool		pop( T&			poppedElement )
	{
		if( m_nextEmptyElement != 0 )
		{
			poppedElement = m_storage[--m_nextEmptyElement];

			return( true );
		}

		return( false );
	}


private :

	unsigned int		m_size;
	unsigned int		m_limit;
	
	T*					m_storage;

	unsigned int		m_nextEmptyElement;



	void				growStorage()
	{
		std::cout << "growing stack" << std::endl;

		T*		newStorage = (T*)_aligned_malloc( sizeof(T) * ( m_limit * 2 ) + 2, __alignof(T) );

		memcpy( newStorage, m_storage, _aligned_msize( m_storage, __alignof(T), 0 ));

		_aligned_free( m_storage );

		m_storage = newStorage;
		m_limit *= 2;
		m_size = m_limit + 2;
	}

};
