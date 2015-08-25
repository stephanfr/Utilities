#ifndef SHORT_ALLOC_H
#define SHORT_ALLOC_H

#include <cstddef>
#include <cassert>


#ifdef NO_ALIGNAS
#define alignas( alignmnt ) __declspec(align( alignmnt ))
#endif

#define noexcept
#define constexpr



template <size_t N>
class arena
{
    static const size_t alignment = 16;
    __declspec(align( 16 )) char buf_[N];
    char* ptr_;

    bool
    pointer_in_buffer(char* p) noexcept
        {return buf_ <= p && p <= buf_ + N;}

public:
    arena() noexcept : ptr_(buf_) {}
    ~arena() {ptr_ = nullptr;}
    arena(const arena&) = delete;
    arena& operator=(const arena&) = delete;

    char* allocate(size_t n);
    void deallocate(char* p, size_t n) noexcept;

    static constexpr size_t size() {return N;}
    size_t used() const {return static_cast<size_t>(ptr_ - buf_);}
    void reset() {ptr_ = buf_;}
};

template <size_t N>
char*
arena<N>::allocate(size_t n)
{
    assert(pointer_in_buffer(ptr_) && "short_alloc has outlived arena");
    if ( size_t( buf_ + N - ptr_ ) >= n)
    {
        char* r = ptr_;
        ptr_ += n;
        return r;
    }

#ifdef SHORT_ALLOC_TUNE
	std::cout << "resizing ShortAlloc buffer to: " << n << std::endl;
#endif

    return static_cast<char*>(::operator new(n));
}

template <size_t N>
void
arena<N>::deallocate(char* p, size_t n) noexcept
{
    assert(pointer_in_buffer(ptr_) && "short_alloc has outlived arena");
    if (pointer_in_buffer(p))
    {
        if (p + n == ptr_)
            ptr_ = p;
    }
    else
        ::operator delete(p);
}

template <class T, size_t N>
class short_alloc
{
    arena<N>& a_;
public:
    typedef T value_type;

public:
    template <class _Up> struct rebind {typedef short_alloc<_Up, N> other;};

    short_alloc(arena<N>& a) noexcept : a_(a) {}
    template <class U>
        short_alloc(const short_alloc<U, N>& a) noexcept
            : a_(a.a_) {}
    short_alloc(const short_alloc&) = default;
    short_alloc& operator=(const short_alloc&) = delete;

    T* allocate(size_t n)
    {
        return reinterpret_cast<T*>(a_.allocate(n*sizeof(T)));
    }
    void deallocate(T* p, size_t n) noexcept
    {
        a_.deallocate(reinterpret_cast<char*>(p), n*sizeof(T));
    }

    template <class T1, size_t N1, class U, size_t M>
    friend
    bool
    operator==(const short_alloc<T1, N1>& x, const short_alloc<U, M>& y) noexcept;

    template <class U, size_t M> friend class short_alloc;
};

template <class T, size_t N, class U, size_t M>
inline
bool
operator==(const short_alloc<T, N>& x, const short_alloc<U, M>& y) noexcept
{
    return N == M && &x.a_ == &y.a_;
}

template <class T, size_t N, class U, size_t M>
inline
bool
operator!=(const short_alloc<T, N>& x, const short_alloc<U, M>& y) noexcept
{
    return !(x == y);
}

#endif  // SHORT_ALLOC_H

