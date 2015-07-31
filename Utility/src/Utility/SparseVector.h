
#pragma once


#include <functional>
#include <unordered_map>
#include <set>

#include <boost\integer\static_min_max.hpp>
#include <boost\iterator\iterator_facade.hpp>
#include <boost\container\static_vector.hpp>

#include <tbb\concurrent_unordered_map.h>
#include <tbb\concurrent_unordered_set.h>




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

		static const unsigned int									CUTOVER_SIZE = 10;

		typedef std::unordered_map<unsigned int, T>					EntryMap;
		typedef typename EntryMap::iterator							EntryMapIterator;

	public :

		SparseVector()
			: m_cutover( false ),
			  m_array( (T*)m_arrayStorage ),
			  m_arraySize( 0 ),
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

		SparseVector&			operator=( const SparseVector&		vectorToCopy )
		{
			assert( false );
		}


		unsigned int			size() const
		{
			if( !m_cutover )
			{
				return( m_arraySize );
			}

			return( m_map->size() );
		}

		bool					empty() const
		{
			if (!m_cutover)
			{
				return( m_arraySize == 0 );
			}

			return( m_map->empty() );
		}


		T&		operator[]( unsigned int		index )
		{
			if( !m_cutover )
			{
				for( unsigned int i = 0; i < m_arraySize; i++ )
				{
					if( m_array[i].index() == index )
					{
						return( m_array[i] );
					}
				}
			}

			EntryMapIterator		itrEntry;

			itrEntry = m_map->find( index );

			if( itrEntry == m_map->end() )
			{
				//	We did not find the entry so there is no choice but assert	
				
				assert( false );
			}

			return( itrEntry->second );
		}



		T&			find_or_add( unsigned int		index )
		{
			if( !m_cutover )
			{
				for( unsigned int i = 0; i < m_arraySize; i++ )
				{
					if( m_array[i].index() == index )
					{
						return( m_array[i] );
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



		void			erase( unsigned int		index )
		{
			if (!m_cutover)
			{
				for ( unsigned int i = 0; i < m_arraySize; i++ )
				{
					if ( m_array[i].index() == index)
					{
						if( i < m_arraySize - 1 )
						{
							m_array[i] = m_array[m_arraySize--];
						}
					}
				}
			}
			else
			{
				itrEntry = m_map->erase( index );
			}
		}




		inline void for_each( std::function<void( T &entry )> action )
		{
			if( !m_cutover )
			{
				for( unsigned int i = 0; i < m_arraySize; i++ )
				{
					action( m_array[i] );
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

		char				m_arrayStorage[sizeof(T) * (CUTOVER_SIZE+1)];

		T*					m_array;
		unsigned int		m_arraySize;

		EntryMap*			m_map;


		T&						addEntry( unsigned int	index )
		{
			if( m_arraySize < CUTOVER_SIZE )
			{
				new ( m_array + m_arraySize ) T( index );
				
				return( m_array[m_arraySize++] );
			}
			else if( m_cutover )
			{
				return( m_map->emplace( index, index ).first->second );
			}
			else
			{
				m_map = new EntryMap( CUTOVER_SIZE * 4 );
				
				for( unsigned int i = 0; i < m_arraySize; i++ )
				{
					m_map->emplace( m_array[i].index(), m_array[i] );
				}

				m_cutover = true;

				return( m_map->emplace( index, index ).first->second );
			}
		}
	};



	

	template<class T>
	class SearchablePointerList
	{
	private:

		static const unsigned int																	CUTOVER_SIZE = 10;

		typedef boost::container::static_vector<T*, CUTOVER_SIZE>									EntryVector;
		typedef typename boost::container::static_vector<T*, CUTOVER_SIZE>::iterator				EntryVectorIterator;
		typedef typename boost::container::static_vector<T*, CUTOVER_SIZE>::const_iterator			EntryVectorConstIterator;

		typedef std::set<T*>																		EntrySet;
		typedef typename EntrySet::iterator															EntrySetIterator;
		typedef typename EntrySet::const_iterator													EntrySetConstIterator;


	public:


		class iterator : public boost::iterator_facade<iterator, T*, boost::forward_traversal_tag, T*>
		{

		protected:

			friend class SearchablePointerList;


			iterator( const EntryVectorIterator&	vectorIterator )
				: m_cutOver(false)
			{
				m_vectorIterator = new( m_buffer )EntryVectorIterator( vectorIterator );
			}

			iterator( const EntrySetIterator&		setIterator )
				: m_cutOver(true)
			{
				m_setIterator = new( m_buffer )EntrySetIterator( setIterator );
			}


			friend class boost::iterator_core_access;

			void increment()
			{
				if (m_cutOver)
				{
					(*m_setIterator)++;
				}
				else
				{
					(*m_vectorIterator)++;
				}
			}

			bool equal( iterator const& other ) const
			{
				if (m_cutOver)
				{
					return( *m_setIterator == *(other.m_setIterator) );
				}
				else
				{
					return( *m_vectorIterator == *(other.m_vectorIterator) );
				}
			}

			T*		dereference() const
			{
				if (m_cutOver)
				{
					return( *(*m_setIterator) );
				}
				else
				{
					return( *(*m_vectorIterator ));
				}
			}


			unsigned char		m_buffer[boost::static_unsigned_max< sizeof( EntryVectorIterator ), sizeof( EntrySetIterator ) >::value];

			bool				m_cutOver;

			union
			{
				EntryVectorIterator*		m_vectorIterator;
				EntrySetIterator*			m_setIterator;
			};
		};


		class const_iterator : public boost::iterator_facade<const_iterator, T*, boost::forward_traversal_tag, T* const>
		{

		protected:

			friend class SearchablePointerList;


			const_iterator( const EntryVectorConstIterator&	vectorIterator )
				: m_cutOver( false )
			{
				m_vectorIterator = new(m_buffer)EntryVectorConstIterator( vectorIterator );
			}

			const_iterator( const EntrySetConstIterator&		setIterator )
				: m_cutOver( true )
			{
				m_setIterator = new(m_buffer)EntrySetConstIterator( setIterator );
			}


			friend class boost::iterator_core_access;

			void increment()
			{
				if (m_cutOver)
				{
					( *m_setIterator )++;
				}
				else
				{
					( *m_vectorIterator )++;
				}
			}

			bool equal( const_iterator const& other ) const
			{
				if (m_cutOver)
				{
					return( *m_setIterator == *( other.m_setIterator ) );
				}
				else
				{
					return( *m_vectorIterator == *( other.m_vectorIterator ) );
				}
			}

			T*		dereference() const
			{
				if (m_cutOver)
				{
					return( *( *m_setIterator ) );
				}
				else
				{
					return( *( *m_vectorIterator ) );
				}
			}


			unsigned char		m_buffer[boost::static_unsigned_max< sizeof( EntryVectorConstIterator ), sizeof( EntrySetConstIterator ) >::value];

			bool				m_cutOver;

			union
			{
				EntryVectorConstIterator*		m_vectorIterator;
				EntrySetConstIterator*			m_setIterator;
			};
		};






		SearchablePointerList()
			: m_cutover( false ),
			  m_inserter( &SearchablePointerList<T>::insertIntoArray ),
			  m_map( NULL )
		{}

		//	Need the copy constructor to keep the compiler quiet about being unable to copy fixed_vectors,
		//		but we should never call it - thus the assert.

		SearchablePointerList( const SearchablePointerList&		vectorToCopy )
		{
			assert( false );
		}

		~SearchablePointerList()
		{
			if (m_map != NULL)
			{
				delete m_map;
			}
		}

		//	Need the assignment operator to keep the compiler quiet about being unable to copy fixed_vectors,
		//		but we should never call it - thus the assert.

		SearchablePointerList&		operator=( const SearchablePointerList&		vectorToCopy )
		{
			assert( false );
		}



		unsigned int			size() const
		{
			if (!m_cutover)
			{
				return( m_array.size() );
			}

			return( m_map->size() );
		}

		bool					empty() const
		{
			if (!m_cutover)
			{
				return( m_array.empty() );
			}

			return( m_map->empty() );
		}

		T*						front() const
		{
			if (!m_cutover)
			{
				return( m_array.front() );
			}

			return( *(m_map->begin()) );
		}



		iterator			begin()
		{
			if (!m_cutover)
			{
				return( iterator( m_array.begin() ) );
			}

			return( iterator( m_map->begin() ) );
		}

		const_iterator			begin() const
		{
			if (!m_cutover)
			{
				return( const_iterator( m_array.begin() ) );
			}

			return( const_iterator( m_map->begin() ) );
		}

		iterator			end()
		{
			if (!m_cutover)
			{
				return( iterator( m_array.end() ) );
			}

			return( iterator( m_map->end() ) );
		}

		const_iterator			end() const
		{
			if (!m_cutover)
			{
				return( const_iterator( m_array.end() ) );
			}

			return( const_iterator( m_map->end() ) );
		}

		


		T*		operator[]( unsigned int		index )
		{
			if (!m_cutover)
			{
				for (T& entry : m_array)
				{
					if (entry.index() == index)
					{
						return( entry );
					}
				}
			}

			EntryMapIterator		itrEntry;

			itrEntry = m_map->find( index );

			if (itrEntry != m_map->end())
			{
				return( itrEntry );
			}

			//	We should never get here, so assert.

			assert( false );
		}


		void			insert( T*		newValue )
		{
			(this->*m_inserter)( newValue );
/*
			if (!m_cutover)
			{
				if (m_array.size() < CUTOVER_SIZE)
				{
					m_array.push_back( newValue );
				}
				else
				{
					m_map = new EntrySet();

					for (T* value : m_array)
					{
						m_map->insert( value );
					}

					m_map->insert( newValue );

					m_cutover = true;
				}
			}
			else
			{
				m_map->insert( newValue );
			}
*/
		}



		void			erase( T*		index )
		{
			if (!m_cutover)
			{
				for (EntryVectorIterator itrElement = m_array.begin(); itrElement != m_array.end(); itrElement++)
				{
					if (*itrElement == index)
					{
						m_array.erase( itrElement );
						break;
					}
				}
			}
			else
			{
				m_map->erase( index );
			}
		}




		inline void for_each( std::function<void( T &entry )> action )
		{
			if (!m_cutover)
			{
				for (auto& currentEntry : m_array)
				{
					action( currentEntry );
				}
			}
			else
			{
				for (auto& currentEntry : *m_map)
				{
					action( currentEntry );
				}
			}
		}


	protected:

		typedef void (SearchablePointerList<T>::*InsertFunctionPointer)( T* );	


		bool						m_cutover;

		InsertFunctionPointer		m_inserter;

		EntryVector					m_array;

		EntrySet*					m_map;




		void			insertIntoArray( T*		newValue )
		{
			if (m_array.size() < CUTOVER_SIZE)
			{
				m_array.push_back( newValue );
			}
			else
			{
				m_map = new EntrySet();

				for (T* value : m_array)
				{
					m_map->insert( value );
				}

				m_map->insert( newValue );

				m_cutover = true;

				m_inserter = &SearchablePointerList<T>::insertIntoMap;
			}
		}

		void			insertIntoMap( T*	newValue )
		{
			m_map->insert( newValue );
		}


		friend class iterator;
	};





}	//	namespace SEFUtility

