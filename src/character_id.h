#pragma once
#ifndef CATA_SRC_CHARACTER_ID_H
#define CATA_SRC_CHARACTER_ID_H

#include <cassert>
#include <ostream>

class JsonIn;
class JsonOut;

class character_id
{
    public:
        character_id() : value( -1 ) {}

        explicit character_id( int i ) : value( i ) {
        }

        auto is_valid() const -> bool {
            return value > 0;
        }

        auto get_value() const -> int {
            return value;
        }

        auto operator++() -> character_id & {
            ++value;
            return *this;
        }

        void serialize( JsonOut & ) const;
        void deserialize( JsonIn & );

        friend inline auto operator==( character_id l, character_id r ) -> bool {
            return l.get_value() == r.get_value();
        }

        friend inline auto operator!=( character_id l, character_id r ) -> bool {
            return l.get_value() != r.get_value();
        }

        friend inline auto operator<( character_id l, character_id r ) -> bool {
            return l.get_value() < r.get_value();
        }

        friend inline auto operator<<( std::ostream &o, character_id id ) -> std::ostream & {
            return o << id.get_value();
        }
    private:
        int value;
};

#endif // CATA_SRC_CHARACTER_ID_H
