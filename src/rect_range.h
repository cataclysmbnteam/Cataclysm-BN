#pragma once
#ifndef CATA_SRC_RECT_RANGE_H
#define CATA_SRC_RECT_RANGE_H

#include "point.h"

// This is a template parameter, it's usually SDL_Rect, but that way the class
// can be used without include any SDL header.
template<typename RectType>
class rect_range
{
    private:
        int width;
        int height;
        point count;

    public:
        rect_range( const int w, const int h, const point &c ) : width( w ), height( h ),
            count( c ) {
        }

        class iterator
        {
            private:
                friend class rect_range;
                const rect_range *range;
                int index;

                iterator( const rect_range *const r, const int i ) : range( r ), index( i ) {
                }

            public:
                auto operator==( const iterator &rhs ) const -> bool {
                    return range == rhs.range && index == rhs.index;
                }
                auto operator!=( const iterator &rhs ) const -> bool {
                    return !operator==( rhs );
                }
                auto operator*() const -> RectType {
                    return { ( index % range->count.x ) *range->width, ( index / range->count.x ) *range->height, range->width, range->height };
                }

                auto operator+( const int offset ) const -> iterator {
                    return iterator( range, index + offset );
                }
                auto operator-( const iterator &rhs ) const -> int {
                    return index - rhs.index;
                }
                auto operator++() -> iterator & {
                    ++index;
                    return *this;
                }
        };

        auto begin() const -> iterator {
            return iterator( this, 0 );
        }
        auto end() const -> iterator {
            return iterator( this, count.x * count.y );
        }
};

#endif // CATA_SRC_RECT_RANGE_H
