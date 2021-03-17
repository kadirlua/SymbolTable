#pragma once
#include <map>
#include <shared_mutex>

namespace aricanli::container {

    /*  
    *   ThreadSafeMap is a thread safe implementation of std::map.
    *   It works with multiple threads at the same time reading or writing into map.
    */
    template<typename Key,
        typename T,
        typename Compare = std::less<Key>,
        typename Alloc = std::allocator<std::pair<const Key, T>>>
    class ThreadSafeMap : public std::map<Key, T, Compare, Alloc>
    {
        using _Mybase = std::map<Key, T, Compare, Alloc>;
        using iterator = typename _Mybase::iterator;
        using const_iterator = typename _Mybase::const_iterator;
        using key_type = typename _Mybase::key_type;
        using key_compare = typename _Mybase::key_compare;
        using allocator_type = typename _Mybase::allocator_type;
        using value_type = typename _Mybase::value_type;
        using mapped_type = typename _Mybase::mapped_type;
        using size_type = typename _Mybase::size_type;
        using map = typename _Mybase::map;
        using reference = value_type&;
        using const_reference = const value_type&;
    public:

        explicit ThreadSafeMap(const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type()) : _Mybase(comp, alloc) {}
        explicit ThreadSafeMap(const allocator_type& alloc) : _Mybase(alloc) {}
        template <typename InputIterator> ThreadSafeMap(InputIterator first,
            InputIterator last,
            const key_compare& comp = key_compare(),
            const allocator_type& alloc = allocator_type()) : _Mybase(first, last, comp, alloc) {}
        ThreadSafeMap(const map& x) : _Mybase(x) {}
        ThreadSafeMap(const ThreadSafeMap& x) : _Mybase(x) {}
        ThreadSafeMap(const ThreadSafeMap& x, const allocator_type& alloc) : _Mybase(x, alloc) {}
        ThreadSafeMap(map&& x) : _Mybase(std::move(x)) {}
        ThreadSafeMap(ThreadSafeMap&& x) : _Mybase(std::move(x)) {}
        ThreadSafeMap(map&& x, const allocator_type& alloc) : _Mybase(std::move(x), alloc) {}
        ThreadSafeMap(ThreadSafeMap&& x, const allocator_type& alloc) : _Mybase(std::move(x), alloc) {}
        ThreadSafeMap(std::initializer_list<value_type> il, const key_compare& comp = key_compare(), 
            const allocator_type& alloc = allocator_type()) : _Mybase(il, comp, alloc) {}

        std::pair<iterator, bool> insert(const value_type& val)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::insert(val);
        }
        //-----------------------------------------------------------------------------
        iterator insert(iterator position, const value_type& val)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::insert(position, val);
        }
        //-----------------------------------------------------------------------------
        template <class InputIterator> void insert(InputIterator first, InputIterator last)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::insert(first, last);
        }

        iterator find(const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::find(k);
        }
        //-----------------------------------------------------------------------------
        const_iterator find(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::find(k);
        }

        //-----------------------------------------------------------------------------
        void clear() noexcept
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::clear();
        }

        iterator erase(const_iterator position)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::erase(position);
        }
        //-----------------------------------------------------------------------------
        size_type erase(const key_type& k)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::erase(k);
        }
        //-----------------------------------------------------------------------------
        iterator erase(const_iterator first, const_iterator last)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::erase(first, last);
        }

        ThreadSafeMap<Key, T, Compare, Alloc>& operator= (const map& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::operator=(x);
            return *this;
        }
        //-----------------------------------------------------------------------------  
        ThreadSafeMap<Key, T, Compare, Alloc>& operator= (const ThreadSafeMap<Key, T, Compare, Alloc>& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::operator=(x);
            return *this;
        }
        //-----------------------------------------------------------------------------  
        ThreadSafeMap<Key, T, Compare, Alloc>& operator= (map&& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::operator=(std::move(x));
            return *this;
        }
        //-----------------------------------------------------------------------------  
        ThreadSafeMap<Key, T, Compare, Alloc>& operator= (ThreadSafeMap<Key, T, Compare, Alloc>&& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::operator=(std::move(x));
            return *this;
        }
        //-----------------------------------------------------------------------------
        ThreadSafeMap<Key, T, Compare, Alloc>& operator= (std::initializer_list<value_type> il)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            _Mybase::operator=(il);
            return *this;
        }
        //-----------------------------------------------------------------------------  
        bool empty() const noexcept
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::empty();
        }
        //-----------------------------------------------------------------------------  
        size_type size() const noexcept
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::size();
        }
        //-----------------------------------------------------------------------------  
        mapped_type& operator[] (const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::operator[](k);
        }
        //-----------------------------------------------------------------------------
        mapped_type& operator[] (key_type&& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::operator[](std::move(k));
        }
        //-----------------------------------------------------------------------------
        mapped_type& at(const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::at(k);
        }
        //-----------------------------------------------------------------------------
        const mapped_type& at(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::at(k);
        }

        size_type count(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::count(k);
        }

        template <typename... Args> std::pair<iterator, bool> emplace(Args&&... args)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return _Mybase::emplace(std::forward<Args>(args)...);
        }

    private:
        mutable std::shared_mutex mutex_; //The mutex for this map
    };
}