#pragma once
#include <map>
#include <string>
#include <shared_mutex>

namespace Symbols {

    class CSymbol;  //incomplete type declaration

    /*  
    *   SymbolMap is a thread safe implementation of std::map.
    *   It works with multiple threads at the same time reading or writing into map.
    */
    template<typename Key,
        typename T,
        typename Compare = std::less<Key>,
        typename Alloc = std::allocator<std::pair<const Key, T>>>
    class SymbolMap : public std::map<Key, T, Compare, Alloc>
    {
        using iterator = typename std::map<Key, T, Compare, Alloc>::iterator ;
        using const_iterator = typename std::map<Key, T, Compare, Alloc>::const_iterator ;
        using key_type = typename std::map<Key, T, Compare, Alloc>::key_type;
        using key_compare = typename std::map<Key, T, Compare, Alloc>::key_compare;
        using allocator_type = typename std::map<Key, T, Compare, Alloc>::allocator_type;
        using value_type = typename std::map<Key, T, Compare, Alloc>::value_type;
        using mapped_type = typename std::map<Key, T, Compare, Alloc>::mapped_type;
        using size_type = typename std::map<Key, T, Compare, Alloc>::size_type;
        using map = typename std::map<Key, T, Compare, Alloc>::map;
        using reference = value_type&;
        using const_reference = const value_type&;
    public:

        explicit SymbolMap(const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type()) : std::map<Key, T, Compare, Alloc>(comp, alloc) {}
        explicit SymbolMap(const allocator_type& alloc) : std::map<Key, T, Compare, Alloc>(alloc) {}
        template <typename InputIterator> SymbolMap(InputIterator first,
            InputIterator last,
            const key_compare& comp = key_compare(),
            const allocator_type& alloc = allocator_type()) : std::map<Key, T, Compare, Alloc>(first, last, comp, alloc) {}
        SymbolMap(const map& x) : std::map<Key, T, Compare, Alloc>(x) {}
        SymbolMap(const SymbolMap& x) : std::map<Key, T, Compare, Alloc>(x) {}
        SymbolMap(const SymbolMap& x, const allocator_type& alloc) : std::map<Key, T, Compare, Alloc>(x, alloc) {}
        SymbolMap(map&& x) : std::map<Key, T, Compare, Alloc>(std::move(x)) {}
        SymbolMap(SymbolMap&& x) : std::map<Key, T, Compare, Alloc>(std::move(x)) {}
        SymbolMap(map&& x, const allocator_type& alloc) : std::map<Key, T, Compare, Alloc>(std::move(x), alloc) {}
        SymbolMap(SymbolMap&& x, const allocator_type& alloc) : std::map<Key, T, Compare, Alloc>(std::move(x), alloc) {}
        SymbolMap(std::initializer_list<value_type> il, const key_compare& comp = key_compare(), 
            const allocator_type& alloc = allocator_type()) : std::map<Key, T, Compare, Alloc>(il, comp, alloc) {}

        std::pair<iterator, bool> insert(const value_type& val)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::insert(val);
        }
        //-----------------------------------------------------------------------------
        iterator insert(iterator position, const value_type& val)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::insert(position, val);
        }
        //-----------------------------------------------------------------------------
        template <class InputIterator> void insert(InputIterator first, InputIterator last)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::insert(first, last);
        }

        iterator find(const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::find(k);
        }
        //-----------------------------------------------------------------------------
        const_iterator find(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::find(k);
        }

        //-----------------------------------------------------------------------------
        void clear() noexcept
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            std::map<Key, T, Compare, Alloc>::clear();
        }

        iterator erase(const_iterator position)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::erase(position);
        }
        //-----------------------------------------------------------------------------
        size_type erase(const key_type& k)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::erase(k);
        }
        //-----------------------------------------------------------------------------
        iterator erase(const_iterator first, const_iterator last)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::erase(first, last);
        }

        SymbolMap<Key, T, Compare, Alloc>& operator= (const map& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator=(x);
        }
        //-----------------------------------------------------------------------------  
        SymbolMap<Key, T, Compare, Alloc>& operator= (const SymbolMap<Key, T, Compare, Alloc>& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator=(x);
        }
        //-----------------------------------------------------------------------------  
        SymbolMap<Key, T, Compare, Alloc>& operator= (map&& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator=(std::move(x));
        }
        //-----------------------------------------------------------------------------  
        SymbolMap<Key, T, Compare, Alloc>& operator= (SymbolMap<Key, T, Compare, Alloc>&& x)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator=(std::move(x));
        }
        //-----------------------------------------------------------------------------
        SymbolMap<Key, T, Compare, Alloc>& operator= (std::initializer_list<value_type> il)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator=(il);
        }
        //-----------------------------------------------------------------------------  
        bool empty() const noexcept
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::empty();
        }
        //-----------------------------------------------------------------------------  
        size_type size() const noexcept
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::size();
        }
        //-----------------------------------------------------------------------------  
        mapped_type& operator[] (const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator[](k);
        }
        //-----------------------------------------------------------------------------
        mapped_type& operator[] (key_type&& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::operator[](std::move(k));
        }
        //-----------------------------------------------------------------------------
        mapped_type& at(const key_type& k)
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::at(k);
        }
        //-----------------------------------------------------------------------------
        const mapped_type& at(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::at(k);
        }

        size_type count(const key_type& k) const
        {
            // A shared mutex is used to enable mutiple concurrent reads
            std::shared_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::count(k);
        }

        template <typename... Args> std::pair<iterator, bool> emplace(Args&&... args)
        {
            //Exclusive lock to enable single write in the map
            std::unique_lock<std::shared_mutex> lock(mutex_);
            return std::map<Key, T, Compare, Alloc>::emplace(std::move(args...));
        }

    private:
        mutable std::shared_mutex mutex_; //The mutex for this map
    };

    //our map to hold whole datas
    using treeMap = SymbolMap<std::string, CSymbol>;    //sortable map class
}