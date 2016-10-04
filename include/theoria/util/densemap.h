#pragma once

#include <theoria/util/strings.h>

#include <memory>
#include <vector>


namespace theoria { namespace util {

template <typename KEY, typename T, typename Alloc=std::allocator<std::pair<const KEY, T> > >
class densemap {
    static const KEY _ENSURE_INTEGRAL_KEY = 0 ;
    using Impl = std::vector<std::pair<KEY,T>*> ;

public:

    class ConstIter
    {
    public:

        using value_type = std::pair<KEY,T> ;

        ConstIter() {}
        ConstIter(const ConstIter& other) : _impl(other._impl), _end(other._end) {}
        ConstIter(typename Impl::const_iterator iter, typename Impl::const_iterator end) : _impl(iter), _end(end) {} 
        const ConstIter& operator =(const ConstIter& other) {if (this != &other) {_impl = other._impl; _end = other._end;} return *this ;}
        bool operator == (const ConstIter& other) const { return _impl == other._impl; }
        bool operator != (const ConstIter& other) const { return _impl != other._impl; }

        const value_type& operator *() const  {return **_impl; }
        const value_type* operator ->() const  {return *_impl;}

        ConstIter operator++() {return ConstIter(advance(false), _end); }
        ConstIter operator++(int) {return ConstIter(advance(true), _end); }
   
    private:
        typename Impl::const_iterator advance(bool postfix) 
        {
            auto temp = _impl ;
            while (++_impl != _end) {
                if (*_impl != nullptr)
                    break ;
                temp = _impl ;
            }
            return postfix ? temp : _impl ;
        }

        typename Impl::const_iterator _impl ;
        typename Impl::const_iterator _end ;
    } ;

    class Iter
    {
    public:

        using value_type = std::pair<KEY,T> ;
 
        Iter() {}
        Iter(const Iter& other) : _impl(other._impl), _end(other._end) {}
        Iter(typename Impl::iterator iter, typename Impl::iterator end) : _impl(iter), _end(end) {init();} 
        const Iter& operator =(const Iter& other) {if (this != &other) {_impl = other._impl; _end = other._end;} return *this ;}
        bool operator == (const Iter& other) const { return _impl == other._impl; }
        bool operator != (const Iter& other) const { return _impl != other._impl; }

        value_type& operator *()  {return **_impl; }
        const value_type& operator *() const  {return **_impl; }

        const value_type* operator ->() const  {return *_impl;}
        value_type* operator ->() {return *_impl;}

        Iter operator++() {return Iter(advance(false),_end ); }
        Iter operator++(int) {return Iter(advance(true), _end); }
   
    private:

        void init()
        {
            while(_impl != _end && *_impl == nullptr) ++_impl ;
        }

        typename Impl::iterator advance(bool postfix) 
        {
            auto temp = _impl ;
            while (++_impl != _end) {
                if (*_impl != nullptr)
                    break ;
                temp = _impl ;
            }
            return postfix ? temp : _impl ;
        }

        typename Impl::iterator _impl ;
        typename Impl::iterator _end ;
    } ;

public:
   
    using size_type = KEY ; 
    using key_type  = KEY ;
    using mapped_type = T ;
    using value_type = std::pair<KEY,T> ;
    using allocator_type = Alloc ;
    using reference = typename Alloc::reference ;
    using const_reference = typename Alloc::const_reference ;
    using pointer = typename Alloc::pointer ;
    using const_pointer = typename Alloc::const_pointer ;
    using iterator = Iter ;
    using const_iterator = ConstIter ;

    densemap() : _impl(), _sz(0) {}
    explicit densemap(int n) : _impl(n), _sz(0) {}
    explicit densemap(allocator_type alloc) : _impl(alloc), _sz(0) {}

    densemap(const densemap& other) : _impl(other._impl), _sz(other._sz) {}
    densemap(const densemap& other, allocator_type alloc) : _impl(other._impl, alloc), _sz(other._sz) {}

    template <typename InputIter>
    densemap(InputIter first, InputIter last) 
    {
        int i = 0 ;
        for (auto iter=first; iter!=last; ++i,++iter)
        {   
            _impl.push_back(new std::pair<KEY,T>(i, *iter)) ;   
        }
        _sz = i ;   
    }

    densemap(std::initializer_list<value_type> il, size_type n = 50) 
        : _impl(n)
    {
        for (auto iter = il.begin(), end = il.end(); iter != end; ++iter)
        {
            if (iter->first >= _impl.size())
                 _impl.resize(2 * std::max(static_cast<typename Impl::size_type>(iter->first),_impl.size())) ;    
            _impl[iter->first] = new value_type(iter->first, iter->second) ;
        }
        _sz = il.size() ; 
    }

    mapped_type at(const key_type& key) 
    {
        auto p = _impl.at(key) ;
        if (!p) {
            throw std::out_of_range(str(key)) ;
        }
        return p->second ;
    }    

    iterator begin() noexcept 
    {
        return Iter(_impl.begin(), _impl.end()) ;
    }

    const_iterator begin() const noexcept
    {
        return  Iter(_impl.begin(), _impl.end()) ;
    }

    iterator end() noexcept 
    {
        return Iter(_impl.end(), _impl.end()) ;
    }

    const_iterator end() const noexcept
    {
        return  Iter(_impl.end(), _impl.end()) ;
    }

    size_type bucket(const key_type& key) const noexcept {return key;}

    size_type bucket_count() const noexcept {return _impl.size();}

    size_type bucket_size() const noexcept {return 1;}

    const_iterator cbegin() const noexcept 
    {
        return  ConstIter(_impl.cbegin(), _impl.cend()) ;
    }

    const_iterator cend() const noexcept 
    {
        return  ConstIter(_impl.cend(), _impl.cend()) ;
    }
   
    void clear() noexcept {_impl.clear() ; _sz= 0 ;}

    size_t count(const key_type& k) const noexcept { return k < _sz && _impl[k] != nullptr;}
    
    size_t size() {return _sz;}
 
private:

    Impl _impl ;
    size_t _sz ;
} ;

}} //theoria::util


