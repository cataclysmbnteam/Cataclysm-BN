#pragma once
#ifndef CATA_SRC_MAP_SELECTOR_H
#define CATA_SRC_MAP_SELECTOR_H

#include <vector>

#include "point.h"
#include "visitable.h"

class map_cursor : public visitable<map_cursor>
{
    private:
        tripoint pos_;

    public:
        map_cursor( const tripoint &pos );
        operator tripoint() const;
};

class map_selector : public visitable<map_selector>
{
        friend visitable<map_selector>;

    public:
        using value_type = map_cursor;
        using size_type = std::vector<value_type>::size_type;
        using iterator = std::vector<value_type>::iterator;
        using const_iterator = std::vector<value_type>::const_iterator;
        using reference = std::vector<value_type>::reference;
        using const_reference = std::vector<value_type>::const_reference;

        /**
         *  Constructs map_selector used for querying items located on map tiles
         *  @param pos position on map at which to start each query
         *  @param radius number of adjacent tiles to include (searching from pos outwards)
         *  @param accessible whether found items must be accessible from pos to be considered
         */
        map_selector( const tripoint &pos, int radius = 0, bool accessible = true );

        // similar to item_location you are not supposed to store this class between turns
        map_selector( const map_selector &that ) = delete;
        auto operator=( const map_selector & ) -> map_selector & = delete;
        map_selector( map_selector && ) = default;

        auto size() const -> size_type {
            return data.size();
        }
        auto begin() -> iterator {
            return data.begin();
        }
        auto end() -> iterator {
            return data.end();
        }
        auto cbegin() const -> const_iterator {
            return data.cbegin();
        }
        auto cend() const -> const_iterator {
            return data.cend();
        }
        auto front() -> reference {
            return data.front();
        }
        auto front() const -> const_reference {
            return data.front();
        }
        auto back() -> reference {
            return data.back();
        }
        auto back() const -> const_reference {
            return data.back();
        }

    private:
        std::vector<value_type> data;
};

#endif // CATA_SRC_MAP_SELECTOR_H
