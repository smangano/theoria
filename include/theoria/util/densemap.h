#pragma once

#include <theoria/util/strings.h>

#include <memory>
#include <vector>


namespace theoria { namespace util {

/**
 * A map where KEY is integral and values are usually densely packed from 0 to some max key
 */
template <typename KEY, typename T, typename Alloc=std::allocator<std::pair<const KEY, T> > >
class densemap {
    static const KEY _ENSURE_INTEGRAL_KEY = 1 ;
    using Impl = std::vector<std::pair<KEY,T>*> ;

public:

    /**
     * Const Iterator implementation for densemap
     */
    class ConstIter
    {
    public:

        /**
         * The value type iterator is over
         */
        using value_type = std::pair<KEY,T> ;

        /**
         * Empty iterator constructor
         */
        ConstIter() {}

        /**
         * Copy constructor
         */
        ConstIter(const ConstIter& other) : _impl(other._impl), _end(other._end) {}

        /**
         * Constructor from underlying impl iterators
         */
        ConstIter(typename Impl::const_iterator iter, typename Impl::const_iterator end) : _impl(iter), _end(end) {} 

        /**
         * Assignemet
         */
        const ConstIter& operator =(const ConstIter& other) {if (this != &other) {_impl = other._impl; _end = other._end;} return *this ;}

        /**
         * Equallity test
         */
        bool operator == (const ConstIter& other) const { return _impl == other._impl; }

        /**
         * Inequallity test
         */
        bool operator != (const ConstIter& other) const { return _impl != other._impl; }

        /**
         * Dereference
         */
        const value_type& operator *() const  {return **_impl; }

        /**
         * Pointer dereference
         */
        const value_type* operator ->() const  {return *_impl;}

        /**
         * Increment the iterator
         */
        ConstIter operator++() {return ConstIter(advance(false), _end); }

        /**
         * Post increment the iterator
         */
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

    /**
     * Iterator implementation for densemap
     */
    class Iter
    {
    public:

        /**
         * The value type iterator is over
         */
        using value_type = std::pair<KEY,T> ;
 
        /**
         * Empty iterator constructor
         */
        Iter() {}

        /**
         * Copy constructor
         */
        Iter(const Iter& other) : _impl(other._impl), _end(other._end) {}

        /**
         * Constructor from underlying impl iterators
         */
        Iter(typename Impl::iterator iter, typename Impl::iterator end) : _impl(iter), _end(end) {init();} 

        /**
         * Assignemet
         */
        const Iter& operator =(const Iter& other) {if (this != &other) {_impl = other._impl; _end = other._end;} return *this ;}

        /**
         * Equallity test
         */
        bool operator == (const Iter& other) const { return _impl == other._impl; }

        /**
         * Inequallity test
         */
        bool operator != (const Iter& other) const { return _impl != other._impl; }

        /**
         * Dereference
         */
        value_type& operator *()  {return **_impl; }

        /**
         * Const Dereference
         */
        const value_type& operator *() const  {return **_impl; }

        /**
         * Const Pointer dereference
         */
        const value_type* operator ->() const  {return *_impl;}

        /**
         * Pointer dereference
         */
        value_type* operator ->() {return *_impl;}


        /**
         * Increment the iterator
         */
        Iter operator++() {return Iter(advance(false),_end ); }

        /**
         * Post increment the iterator
         */
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
  
    /**
     * The type used to indiacte the size of a dense map
     */
    using size_type = KEY ; 

    /**
     * The type used to indicate the type of key
     */
    using key_type  = KEY ;

    /**
     * The type used to indicate the type of value the key maps to
     */
    using mapped_type = T ;

    /**
     * The type used to indicate the type stored 
     */
    using value_type = std::pair<KEY,T> ;

    /**
     * The type used to indicate the allocator
     */
    using allocator_type = Alloc ;

    /**
     * The type used to indicate a reference to the value
     */
    using reference = typename Alloc::reference ;

    /**
     * The type used to indicate a const reference to the value
     */
    using const_reference = typename Alloc::const_reference ;

    /**
     * The type used to a pointer to the value
     */
    using pointer = typename Alloc::pointer ;

    /**
     * The type used to const pointer to the value
     */
    using const_pointer = typename Alloc::const_pointer ;

    /**
     * The type used to indicate an iterator over the map
     */
    using iterator = Iter ;

    /**
     * The type used to indicate a const iterator over the map
     */
    using const_iterator = ConstIter ;

private:

    inline
    void _growFor(const key_type& k) 
    {
        _impl.resize(k + 50) ;
    }

public:

    /**
     * Construct an empty map
     */
    densemap() : _impl(), _sz(0) {}

    /**
     * Construct an empty map but reserve initial space for n entries
     */
    explicit densemap(int n) : _impl(n), _sz(0) {}

    /**
     * Construct an empty map with specified allocator
     */
    explicit densemap(allocator_type alloc) : _impl(alloc), _sz(0) {}

    /**
     * Copy constructor
     */
    densemap(const densemap& other) : _impl(other._impl.size()), _sz(other._sz) {_copy(other._impl);}


    /**
     * Copy constructor with specific allocator
     */
    densemap(const densemap& other, allocator_type alloc) : _impl(other._impl.size, alloc), _sz(other._sz)
    {_copy(other._impl);}

    /**
     * Construct a dense map with values provided by iterators and successive keys starting from 0
     */
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

    /**
     * Construct a dense map from an initializer list
     */
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

    /**
     * Destructor
     */
    ~densemap() ;

    /**
     * Return value at key.
     * @throw std::out_of_range if does not exist
     */
    mapped_type at(const key_type& key) 
    {
        auto p = _impl.at(key) ;
        if (!p) {
            throw std::out_of_range(str(key)) ;
        }
        return p->second ;
    }    

    /**
     * Map iterator to first entry
     */
    iterator begin() noexcept 
    {
        return Iter(_impl.begin(), _impl.end()) ;
    }

    /**
     * map const iterator to first entry
     */
    const_iterator begin() const noexcept
    {
        return  Iter(_impl.begin(), _impl.end()) ;
    }

    /**
     * Map iterator to _past-the-end_ entry
     */
    iterator end() noexcept 
    {
        return Iter(_impl.end(), _impl.end()) ;
    }

    /**
     * Map const iterator to _past-the-end_ entry
     */
    const_iterator end() const noexcept
    {
        return  Iter(_impl.end(), _impl.end()) ;
    }

    /**
     * The bucket number for key. Always equal to key as buckets are effectively size 1
     */
    size_type bucket(const key_type& key) const noexcept {return key;}

    /**
     * The bucket count Always equal to size as buckets are effectively size 1
     */
    size_type bucket_count() const noexcept {return static_cast<size_type>(_impl.size());}

    /**
     * The bucket size. Always 1
     */
    size_type bucket_size() const noexcept {return 1;}

    /**
     * map const iterator to first entry
     */ 
    const_iterator cbegin() const noexcept 
    {
        return  ConstIter(_impl.cbegin(), _impl.cend()) ;
    }

    /**
     * map const iterator to _past-the-end_ entry
     */ 
    const_iterator cend() const noexcept 
    {
        return  ConstIter(_impl.cend(), _impl.cend()) ;
    }
  
    /**
     * Clear all entries in map
     */
    void clear() noexcept {_impl.clear() ; _sz= 0 ;}

    /**
     * Return number of entries in map for specified key. Always 0 or 1
     */
    size_t count(const key_type& k) const noexcept { return k < _sz && _impl[k] != nullptr;}

    /**
     * Return true if map is empty
     */
    bool empty() const noexcept {return _sz == 0;}

    /**
     * Return an iterator to entry with key otherwise end
     */
    iterator find(const key_type& k) {return k < _sz && _impl[k] ? iterator(_impl.begin() + k, _impl.end()) : iterator(_impl.end(), _impl.end()) ; }

    /**
     * Return a cosnt iterator to entry with key otherwise end
     */
    const_iterator find(const key_type& k) const {return k < _sz && _impl[k] ? const_iterator(_impl.cbegin() + k, _impl.cend()) : const_iterator(_impl.cend(), _impl.cend()) ; }

    /**
     * Insert an entry if it does not exists.
     * @return std::pair<iterator, bool> where iterator poinst to the entry and bool is true if it was inserted and false if already existed
     */
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
   
    /**
     * Number of entries in map
     */
    size_t size() const noexcept {return _sz;}

    /**
     * Access value at key. TODO: May be wrong
     */
    mapped_type& operator[] (const key_type& k)
    {
        return _impl[k]->second ;   
    }  

    /**
     * Access value at key
     */
    const mapped_type& operator[] (const key_type& k) const
    {
        return _impl[k]->second ;   
    }  

    /**
     * TODO: This is probably wrong!
     */
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

    /**
     * Add a value to the map assigning next key
     */
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


