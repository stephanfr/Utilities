

#pragma once


#include <EASTL\list.h>
#include <EASTL\fixed_vector.h>

#include <boost\mpl\and.hpp>
#include <boost\mpl\greater.hpp>
#include <boost\mpl\comparison.hpp>



template <typename T, unsigned int ChunkSize, typename CollectionType = void*, unsigned int CollectionReservation = 0, class enableCollection = void >
class ObjectPool
{
public:

	ObjectPool()
	{
		//	Start the chunk list with a new chunk

		m_poolChunks.push_back(new PoolChunk());
	}

	~ObjectPool()
	{
		reset();
	}



	template<class... _Valty>
	T*			newObject(_Valty&&... _Val)
	{
		if (m_poolChunks.back()->has_overflowed())
		{
			m_poolChunks.push_back(new PoolChunk());
		}

		T*		newObject = new (m_poolChunks.back()->push_back_uninitialized()) T(std::forward<_Valty>(_Val)...);

		return(newObject);
	}

	void			free(T*		objectToFree)
	{}


	void			reset()
	{
		while (!m_poolChunks.empty())
		{
			m_poolChunks.back()->reset();
			delete m_poolChunks.back();
			m_poolChunks.pop_back();
		}
	}


private:

	typedef eastl::fixed_vector<T, ChunkSize, false>			PoolChunk;

	eastl::list<PoolChunk*>										m_poolChunks;
};



template <typename T, unsigned int ChunkSize, typename CollectionType, unsigned int CollectionReservation >
class ObjectPool< T, ChunkSize, CollectionType, CollectionReservation, class std::enable_if< boost::mpl::and_< std::is_class<CollectionType>, boost::mpl::equal_to< boost::mpl::int_<CollectionReservation>, boost::mpl::int_<0> > >::value >::type >
{
public:

	ObjectPool()
	{
		//	Start the chunk list with a new chunk

		m_poolChunks.push_back(new PoolChunk());
	}

	~ObjectPool()
	{
		reset();
	}


	CollectionType&				activeObjects()
	{
		return( m_activeObjects );
	}


	template<class... _Valty>
	T*			newObject(_Valty&&... _Val)
	{
		if (m_poolChunks.back()->has_overflowed())
		{
			m_poolChunks.push_back(new PoolChunk());
		}

		T*		newObject = new (m_poolChunks.back()->push_back_uninitialized()) T(std::forward<_Valty>(_Val)...);

		m_activeObjects.push_back(newObject);

		return(newObject);
	}

	void			free(T*		objectToFree)
	{}


	void			reset()
	{
		while (!m_poolChunks.empty())
		{
			m_poolChunks.back()->reset();
			delete m_poolChunks.back();
			m_poolChunks.pop_back();
		}

		m_activeObjects.reset();
	}


private:

	typedef eastl::fixed_vector<T, ChunkSize, false>			PoolChunk;

	CollectionType												m_activeObjects;

	eastl::list<PoolChunk*>										m_poolChunks;
};




template <typename T, unsigned int ChunkSize, typename CollectionType, unsigned int CollectionReservation >
class ObjectPool< T, ChunkSize, CollectionType, CollectionReservation, class std::enable_if< boost::mpl::and_< std::is_class<CollectionType>, boost::mpl::greater< boost::mpl::int_<CollectionReservation>, boost::mpl::int_<0> > >::value >::type >
{
public:

	ObjectPool()
	{
		//	Start the chunk list with a new chunk

		m_poolChunks.push_back(new PoolChunk());

		m_activeObjects.reserve( CollectionReservation );
	}

	~ObjectPool()
	{
		reset();
	}


	CollectionType&				activeObjects()
	{
		return( m_activeObjects );
	}


	template<class... _Valty>
	T*			newObject(_Valty&&... _Val)
	{
		if (m_poolChunks.back()->has_overflowed())
		{
			m_poolChunks.push_back(new PoolChunk());
		}

		T*		newObject = new (m_poolChunks.back()->push_back_uninitialized()) T(std::forward<_Valty>(_Val)...);

		m_activeObjects.push_back(newObject);

		return(newObject);
	}

	void			free(T*		objectToFree)
	{}


	void			reset()
	{
		while (!m_poolChunks.empty())
		{
			m_poolChunks.back()->reset();
			delete m_poolChunks.back();
			m_poolChunks.pop_back();
		}

		m_activeObjects.reset();
	}


private:

	typedef eastl::fixed_vector<T, ChunkSize, false>			PoolChunk;

	CollectionType												m_activeObjects;

	eastl::list<PoolChunk*>										m_poolChunks;
};






