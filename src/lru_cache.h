#pragma once
#ifndef CATA_SRC_LRU_CACHE_H
#define CATA_SRC_LRU_CACHE_H

#include <list>
#include <unordered_map>
#include <utility>

#include "enums.h" // IWYU pragma: keep

template<typename Key, typename Value>
class lru_cache
{
    public:
        using Pair = std::pair<Key, Value>;

        void insert( int limit, const Key &, const Value & );
        auto get( const Key &, const Value &default_ ) const -> Value;
        void remove( const Key & );

        void clear();
        auto list() const -> const std::list<Pair> &;
    private:
        void trim( int limit );
        std::list<Pair> ordered_list;
        std::unordered_map<Key, typename std::list<Pair>::iterator> map;
};

#endif // CATA_SRC_LRU_CACHE_H
