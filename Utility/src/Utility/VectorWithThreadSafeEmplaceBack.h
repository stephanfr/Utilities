
#pragma once


#include <vector>

#include <tbb/spin_mutex.h>





template <typename T>
class VectorWithThreadSafeEmplaceBack : public std::vector<T>
{
	typedef std::vector<T>			BaseType;

public :

	VectorWithThreadSafeEmplaceBack()
	{}


//	using BaseType::iterator;
//	using BaseType::const_iterator;

//	using BaseType::reserve;
//	using BaseType::clear;
//	using BaseType::size;

//	using BaseType::begin;
//	using BaseType::end;


	template<class... _Valty>
	T&		emplace_back(_Valty&&... _Val)
	{
		T*		newElement;

		m_emplaceLock.lock();
		BaseType::emplace_back( std::forward<_Valty>(_Val)... );
		newElement = &BaseType::back();
		m_emplaceLock.unlock();

		return( *newElement );
	}

private :

	tbb::spin_mutex			m_emplaceLock;

};






