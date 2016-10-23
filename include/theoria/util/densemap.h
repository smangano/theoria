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

private:

    inline
    void _growFor(const key_type& k) 
    {
        _impl.resize(k + 50) ;
    }

public:

    densemap() : _impl(), _sz(0) {}
    explicit densemap(int n) : _impl(n), _sz(0) {}
    explicit densemap(allocator_type alloc) : _impl(alloc), _sz(0) {}

    densemap(const densemap& other) : _impl(other._impl.size()), _sz(other._sz) {_copy(other._impl);}
    densemap(const densemap& other, allocator_type alloc) : _impl(other._impl.size, alloc), _sz(other._sz)
    {_copy(other._impl);}

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
            if (static_cast<typename Impl::size_type>(iter->first) >= _impl.size())
                 _growFor(iter->first) ;
            _impl[iter->first] = new value_type(iter->first, iter->second) ;
        }
        _sz = il.size() ; 
    }

    ~densemap() ;

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

    size_type bucket_count() const noexcept {return static_cast<size_type>(_impl.size());}

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

    bool empty() const noexcept {return _sz == 0;}

    iterator find(const key_type& k) {return k < _sz && _impl[k] ? iterator(_impl.begin() + k, _impl.end()) : iterator(_impl.end(), _impl.end()) ; }
    const_iterator find(const key_type& k) const {return k < _sz && _impl[k] ? const_iterator(_impl.cbegin() + k, _impl.cend()) : const_iterator(_impl.cend(), _impl.cend()) ; }

    std::pair<iterator, bool> insert(value_type& val) 
    {
        if (val.first >= _sz)
            _growFor(val.first) ; 
        bool stat = false ;
        if (!_impl[val.first]) 
        { 
            stat = true ;
            _impl[val.first] = new value_type(val) ;
            _sz++ ;
        } 
        return std::make_pair(iterator(_impl.begin() + val.first, _impl.end()), stat) ;
    }
    
    size_t size() const noexcept {return _sz;}

    mapped_type& operator[] (const key_type& k)
    {
        return _impl[k]->second ;   
    }  

    const mapped_type& operator[] (const key_type& k) const
    {
        return _impl[k]->second ;   
    }  

    mapped_type& operator[] (key_type&& k)
    {
        if (static_cast<size_type>(k) >= _sz) _growFor(k) ;
        if (!_impl[k]) 
        {
             _impl[k] = new value_type(k, mapped_type()) ;
             _sz++ ;
        }
        return _impl[k]->second ;   
    }  

    key_type add(mapped_type value){
        key_type k = _sz++ ;
        _impl.push_back(value_type(k,value)) ;
        return k ;
    }

private:

    void _copy(const Impl& other) ;

    Impl _impl ;
    size_type _sz ;
} ;

template <typename KEY, typename T, typename Alloc>
densemap<KEY,T,Alloc>::~densemap()
{
    const int size = _impl.size() ;
    for (int i=0; i<size;++i)
        delete _impl[i] ;
}

template <typename KEY, typename T, typename Alloc>
void densemap<KEY,T,Alloc>::_copy(const Impl& other)
{
    const int size = other.size() ;
    for (int i=0; i<size;++i)
        if (other[i])
            _impl[i] = new value_type(*other[i]);
}


}} //theoria::util


