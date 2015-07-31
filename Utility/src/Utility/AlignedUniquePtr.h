#pragma once


#include <boost\align\aligned_delete.hpp>
#include <memory>




template<class T>
using aligned_unique_ptr = std::unique_ptr<T,boost::alignment::aligned_delete>;



template<class T, class... Args>
inline aligned_unique_ptr<T> make_aligned(Args&&... args)
{
    auto p = boost::alignment::aligned_alloc(__alignof(T), sizeof(T));

    if (!p)
	{
        throw std::bad_alloc();
    }
    
	try
	{
        auto q = ::new(p) T(std::forward<Args>(args)...);
        return aligned_unique_ptr<T>(q);
    }
	catch (...)
	{
        boost::alignment::aligned_free(p);
        throw;
    }
}

