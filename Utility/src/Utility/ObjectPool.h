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


#include <boost\ptr_container\ptr_vector.hpp>

#include <EASTL\list.h>
#include <EASTL\fixed_vector.h>




namespace SEFUtility
{

	template<class T>
	class ObjectPoolable
	{
	public :

		T*		m_next;
		T*		m_prev;
	};

	

	template <typename T, unsigned int ChunkSize> class ObjectPoolManager;




	template <typename T, unsigned int ChunkSize>
	class ObjectPool : boost::noncopyable
	{
	public:

		//	About the minimum possible iterator for traversing a pool.
		//		This is a forward only iterator, though it could be made bidirectional if needed.

		class iterator
		{
		public:

			iterator(T*		node)
				: m_element(node)
			{}

			iterator(const iterator&		itrToCopy)
				: m_element(itrToCopy.m_element)
			{}


			T&				operator*()
			{
				return(*m_element);
			}

			T*				operator->()
			{
				return(m_element);
			}

			iterator		operator++()
			{
				m_element = m_element->m_next;

				return(*this);
			}

			iterator		operator++(int)
			{
				iterator	returnValue = *this;

				m_element = m_element->m_next;

				return(returnValue);
			}

			bool			operator==(const iterator&		itrToCompare) const
			{
				return(m_element == itrToCompare.m_element);
			}

			bool			operator!=(const iterator&		itrToCompare) const
			{
				return(m_element != itrToCompare.m_element);
			}


		private:

			T*		m_element;
		};




		~ObjectPool()
		{
			for (PoolChunk* currentChunk : m_poolChunks)
			{
				currentChunk->reset();
				delete currentChunk;
			}
		}



		void		reset()
		{
			for (PoolChunk* currentChunk : m_poolChunks)
			{
				currentChunk->reset();
			}

			m_currentChunk = m_poolChunks.begin();

			//	Initialize with two uninitialized objects, one will be the start marker, the second will be the end marker.
			//		end() is defined as m_nextFreeObject, so this insures we can insert and delete safely without if statements.

			m_begin = (T*)((*m_currentChunk)->push_back_uninitialized());
			m_nextFreeObject = (T*)((*m_currentChunk)->push_back_uninitialized());

			m_begin->m_prev = m_begin;
			m_begin->m_next = m_nextFreeObject;
			m_lastObject = m_begin;

			m_nextFreeObject->m_prev = m_begin;
			m_nextFreeObject->m_next = m_nextFreeObject;

			m_size = 0;
		}


		iterator			begin()
		{
			return(iterator(m_begin->m_next));
		}

		iterator			end()
		{
			return(iterator(m_nextFreeObject));
		}


		T*			newObject()
		{
			if ((*m_currentChunk)->has_overflowed())
			{
				m_currentChunk++;

				if (m_currentChunk == m_poolChunks.end())
				{
					m_poolChunks.push_back(new PoolChunk());

					m_currentChunk = m_poolChunks.end();
					--m_currentChunk;
				}
			}

			T*		newObject = new(m_nextFreeObject)T;

			m_nextFreeObject = (T*)((*m_currentChunk)->push_back_uninitialized());

			m_size++;

			//	The new object becomes the last object, so link everything accordingly

			m_lastObject->m_next = newObject;
			newObject->m_prev = m_lastObject;
			m_lastObject = newObject;
			newObject->m_next = m_nextFreeObject;

			return(newObject);
		}


		template<class... _Valty>
		T*			newObject(_Valty&&... _Val)
		{
			if ((*m_currentChunk)->has_overflowed())
			{
				m_currentChunk++;

				if (m_currentChunk == m_poolChunks.end())
				{
					m_poolChunks.push_back(new PoolChunk());

					m_currentChunk = m_poolChunks.end();
					--m_currentChunk;
				}
			}

			T*		newObject = new (m_nextFreeObject)T(std::forward<_Valty>(_Val)...);

			m_nextFreeObject = (T*)((*m_currentChunk)->push_back_uninitialized());

			m_size++;

			//	The new object becomes the last object, so link everything accordingly

			m_lastObject->m_next = newObject;
			newObject->m_prev = m_lastObject;
			m_lastObject = newObject;
			newObject->m_next = m_nextFreeObject;

			return(newObject);
		}


		void			free(T*		objectToFree)
		{
			objectToFree->m_prev->m_next = objectToFree->m_next;
			objectToFree->m_next->m_prev = objectToFree->m_prev;

			m_size--;
		}


		size_t					size() const
		{
			return(m_size);
		}


	protected :

			ObjectPool()
			{
				//	Start the chunk list with a new chunk

				m_poolChunks.push_back(new PoolChunk());

				reset();
			}


			friend class ObjectPoolManager<T, ChunkSize>;


	private:

		typedef eastl::fixed_vector<T, ChunkSize, false>		PoolChunk;

		eastl::list<PoolChunk*>									m_poolChunks;
		typename eastl::list<PoolChunk*>::iterator				m_currentChunk;

		size_t													m_size;

		T*														m_begin;
		T*														m_lastObject;
		T*														m_nextFreeObject;
	};




	template <typename T, unsigned int ChunkSize>
	class ObjectPoolManager : boost::noncopyable
	{
	public  :

		typedef	ObjectPool<T, ChunkSize>			ObjectCollection;


		ObjectPoolManager()
			: m_freePools(10)
		{}

		~ObjectPoolManager()
		{}


		std::unique_ptr<ObjectCollection>		getPool()
		{
			if (!m_freePools.empty())
			{
				return(std::unique_ptr<ObjectCollection>(m_freePools.pop_back().release()));
			}

			return(std::unique_ptr<ObjectCollection>( new ObjectCollection() ));
		}

		void		returnPool(std::unique_ptr<ObjectCollection>&		poolToCheckin)
		{
			poolToCheckin->reset();
			
			m_freePools.push_back( poolToCheckin.release() );
		}


	private :

		boost::ptr_vector<ObjectCollection>			m_freePools;
	};





	template< typename T >
	class ObjectPoolHolder : boost::noncopyable
	{
	public:

		ObjectPoolHolder(T&		poolManager)
			: m_poolManager(poolManager),
			  m_pool(std::move(poolManager.getPool()))
		{}

		~ObjectPoolHolder()
		{
			m_poolManager.returnPool(m_pool);
		}


		typename T::ObjectCollection&			getPool()
		{
			return(*m_pool);
		}

	private:

		T&													m_poolManager;

		std::unique_ptr<typename T::ObjectCollection>		m_pool;
	};

}







