
#pragma once


#include <functional>
#include <unordered_map>

#include <EASTL\fixed_vector.h>



namespace SEFUtility
{
	class SparseVectorEntry
	{
	public :

		SparseVectorEntry( unsigned int		index )
			: m_index( index )
		{}


		unsigned int		index() const
		{
			return( m_index );
		}


	private :

		unsigned int		m_index;
	};



    template<class T>
	class SparseVector
	{
	private :

		static const unsigned int								CUTOVER_SIZE = 10;

		typedef eastl::fixed_vector<T,CUTOVER_SIZE,false>		EntryVector;

		typedef std::unordered_map<unsigned int, T>				EntryMap;
		typedef typename EntryMap::iterator						EntryMapIterator;

	public :

		SparseVector()
			: m_cutover( false ),
			  m_map( NULL )
		{}

		//	Need the copy constructor to keep the compiler quiet about being unable to copy fixed_vectors,
		//		but we should never call it - thus the assert.

		SparseVector( const SparseVector&		vectorToCopy )
		{
			assert( false );
		}

		~SparseVector()
		{
			if( m_map != NULL )
			{
				delete m_map;
			}
		}

		//	Need the assignment operator to keep the compiler quiet about being unable to copy fixed_vectors,
		//		but we should never call it - thus the assert.

		SparseVector&		operator=( const SparseVector&		vectorToCopy )
		{
			assert( false );
		}


		unsigned int			size() const
		{
			if( !m_cutover )
			{
				return( m_array.size() );
			}

			return( m_map->size() );
		}

		T&		operator[]( unsigned int		index )
		{
			if( !m_cutover )
			{
				for( T& entry : m_array )
				{
					if( entry.index() == index )
					{
						return( entry );
					}
				}
			}

			EntryMapIterator		itrEntry;

			itrEntry = m_map->find( index );

			if( itrEntry != m_map->end() )
			{
				return( itrEntry->second );
			}

			//	We should never get here, so assert.
			
			assert( false );
		}



		T&			find_or_add( unsigned int		index )
		{
			if( !m_cutover )
			{
				for( T& entry : m_array )
				{
					if( entry.index() == index )
					{
						return( entry );
					}
				}
			}
			else
			{
				EntryMapIterator		itrEntry;

				itrEntry = m_map->find( index );

				if( itrEntry != m_map->end() )
				{
					return( itrEntry->second );
				}
			}

			return( addEntry( index ));
		}


		inline void for_each( std::function<void( T &entry )> action )
		{
			if( !m_cutover )
			{
				for( auto& currentEntry : m_array )
				{
					action( currentEntry);
				}
			}
			else
			{
				for( auto& currentEntry : *m_map )
				{
					action( currentEntry.second );
				}
			}
		}


	private :

		bool				m_cutover;

		EntryVector			m_array;

		EntryMap*			m_map;


		T&						addEntry( unsigned int	index )
		{
			if( m_array.size() < CUTOVER_SIZE )
			{
				return( *(new ( m_array.push_back_uninitialized() ) T( index )) );
			}
			else if( m_cutover )
			{
				return( m_map->emplace( index, index ).first->second );
			}
			else
			{
				m_map = new EntryMap( CUTOVER_SIZE * 4 );
				
				for( T& entry : m_array )
				{
					m_map->emplace( entry.index(), entry );
				}

				m_cutover = true;

				return( m_map->emplace( index, index ).first->second );
			}
		}
	};

}	//	namespace SEFUtility

