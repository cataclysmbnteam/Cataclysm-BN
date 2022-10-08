#pragma once
#ifndef CATA_SRC_OPTIONAL_H
#define CATA_SRC_OPTIONAL_H

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>

namespace cata
{

class bad_optional_access : public std::logic_error
{
    public:
        bad_optional_access() : logic_error( "cata::optional: no value contained" ) { }
};

struct nullopt_t {
    explicit constexpr nullopt_t( int ) {}
};
static constexpr nullopt_t nullopt{ 0 };

struct in_place_t {
    explicit in_place_t() = default;
};
static constexpr in_place_t in_place{ };

template<typename T>
class optional
{
    private:
        using StoredType = typename std::remove_const<T>::type;
        union {
            // `volatile` suppresses -Wmaybe-uninitialized false positive
            // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80635#c53
            volatile char dont_use;
            char dummy;
            StoredType data;
        };
        bool full;

        auto get() -> T & {
            assert( full );
            return data;
        }
        auto get() const -> const T & {
            assert( full );
            return data;
        }

        template<typename... Args>
        void construct( Args &&... args ) {
            assert( !full );
            new( &data )StoredType( std::forward<Args>( args )... );
            full = true;
        }
        void destruct() {
            data.~StoredType();
        }

    public:
        constexpr optional() noexcept : dummy(), full( false ) { }
        constexpr optional( const nullopt_t ) noexcept : dummy(), full( false ) { }

        optional( const optional &other ) : full( false ) {
            if( other.full ) {
                construct( other.get() );
            }
        }
        optional( optional &&other ) : full( false ) {
            if( other.full ) {
                construct( std::move( other.get() ) );
            }
        }
        template<typename... Args>
        explicit optional( in_place_t, Args &&... args ) : data( std::forward<Args>( args )... ),
            full( true ) { }

        template<typename U, typename... Args>
        explicit optional( in_place_t, std::initializer_list<U> ilist,
                           Args &&... args ) : data( ilist,
                                       std::forward<Args>( args )... ), full( true ) { }

        template < typename U = T,
                   typename std::enable_if <
                       !std::is_same<optional<T>, typename std::decay<U>::type>::value &&
                       std::is_constructible < T, U && >::value &&
                       std::is_convertible < U &&, T >::value, bool >::type = true >
        // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
        optional( U && t )
            : optional( in_place, std::forward<U>( t ) ) { }

        template < typename U = T,
                   typename std::enable_if <
                       !std::is_same<optional<T>, std::decay<U>>::value &&
                       std::is_constructible < T, U && >::value &&
                       !std::is_convertible < U &&, T >::value, bool >::type = false >
        // NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
        explicit optional( U && t )
            : optional( in_place, std::forward<U>( t ) ) { }

        ~optional() {
            reset();
        }

        constexpr auto operator->() const -> const T * {
            return &get();
        }
        auto operator->() -> T * {
            return &get();
        }
        constexpr auto operator*() const -> const T & {
            return get();
        }
        auto operator*() -> T & {
            return get();
        }

        constexpr explicit operator bool() const noexcept {
            return full;
        }
        constexpr auto has_value() const noexcept -> bool {
            return full;
        }

        auto value() -> T & {
            if( !full ) {
                throw bad_optional_access();
            }
            return get();
        }
        auto value() const -> const T & {
            if( !full ) {
                throw bad_optional_access();
            }
            return get();
        }

        template<typename O>
        auto value_or( O &&other ) const -> T {
            return full ? get() : static_cast<T>( other );
        }

        template<class... Args>
        auto emplace( Args &&... args ) -> T & {
            reset();
            construct( std::forward<Args>( args )... );
            return get();
        }
        template<class U, class... Args>
        auto emplace( std::initializer_list<U> ilist, Args &&... args ) -> T & {
            reset();
            construct( ilist, std::forward<Args>( args )... );
            return get();
        }

        void reset() noexcept {
            if( full ) {
                full = false;
                destruct();
            }
        }

        auto operator=( nullopt_t ) noexcept -> optional & {
            reset();
            return *this;
        }
        auto operator=( const optional &other ) -> optional & {
            if( full && other.full ) {
                get() = other.get();
            } else if( full ) {
                reset();
            } else if( other.full ) {
                construct( other.get() );
            }
            return *this;
        }
        auto operator=( optional &&other ) -> optional & {
            if( full && other.full ) {
                get() = std::move( other.get() );
            } else if( full ) {
                reset();
            } else if( other.full ) {
                construct( std::move( other.get() ) );
            }
            return *this;
        }
        template < class U = T,
                   typename std::enable_if <
                       !std::is_same<optional<T>, typename std::decay<U>::type>::value &&
                       std::is_constructible < T, U && >::value &&
                       std::is_convertible < U &&, T >::value, bool >::type = true >
        auto operator=( U && value ) -> optional & {
            if( full ) {
                get() = std::forward<U>( value );
            } else {
                construct( std::forward<U>( value ) );
            }
            return *this;
        }
        template<class U>
        auto operator=( const optional<U> &other ) -> optional & {
            if( full && other.full ) {
                get() = other.get();
            } else if( full ) {
                reset();
            } else if( other.full ) {
                construct( other.get() );
            }
            return *this;
        }
        template<class U>
        auto operator=( optional<U> &&other ) -> optional & {
            if( full && other.full ) {
                get() = std::move( other.get() );
            } else if( full ) {
                reset();
            } else if( other.full ) {
                construct( std::move( other.get() ) );
            }
            return *this;
        }

        void swap( optional &other ) {
            using std::swap;

            if( full && other.full ) {
                swap( get(), other.get() );
            } else if( other.full() ) {
                construct( std::move( other.get() ) );
                other.destruct();
            } else if( full ) {
                other.construct( std::move( get() ) );
                destruct();
            }
        }
};

template<class T, class U>
constexpr auto operator==( const optional<T> &lhs, const optional<U> &rhs ) -> bool
{
    if( lhs.has_value() != rhs.has_value() ) {
        return false;
    } else if( !lhs ) {
        return true;
    } else {
        return *lhs == *rhs;
    }
}

template< class T, class U >
constexpr auto operator!=( const optional<T> &lhs, const optional<U> &rhs ) -> bool
{
    return !operator==( lhs, rhs );
}

} // namespace cata

#endif // CATA_SRC_OPTIONAL_H
