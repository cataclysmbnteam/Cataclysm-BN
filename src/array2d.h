#pragma once

#include <array>

namespace detail
{

template<size_t SizeX, size_t SizeY, typename ValType>
struct array2d_info {
    using RowType = std::array<ValType, SizeX>;
    using ArrType = std::array<RowType, SizeY>;
};

template<size_t SizeX, size_t SizeY, typename F, typename Val = std::invoke_result_t<F, size_t, size_t>, size_t ... Xs>
constexpr static auto init_array_2d_xs( size_t py, F && fn, std::index_sequence<Xs...> )
{
    static_assert( sizeof...( Xs ) == SizeX );
    using RowType = typename array2d_info<SizeX, SizeY, Val>::RowType;
    return RowType {
        fn( Xs, py ) ...,
    };
}

template<size_t SizeX, size_t SizeY, typename F, typename Val = std::invoke_result_t<F, size_t, size_t>, size_t ... Ys>
constexpr static auto init_array_2d_ys( F && fn, std::integer_sequence<size_t, Ys...> )
{
    static_assert( sizeof...( Ys ) == SizeY );
    using ArrType = typename array2d_info<SizeX, SizeY, Val>::ArrType;
    return ArrType {
        detail::init_array_2d_xs<SizeX, SizeY>(
            Ys,
            std::forward<F>( fn ),
            std::make_index_sequence<SizeX>()
        ) ...,
    };
}

template<size_t SizeX, size_t SizeY, typename F>
constexpr static auto init_array_2d( F &&fn )
{
    return detail::init_array_2d_ys<SizeX, SizeY>(
               std::forward<F>( fn ),
               std::make_index_sequence<SizeY>()
           );
}

} // namespace detail

/// 2D Array Class
///
/// <tt>std::array<std::array<T,SizeX>, SizeY></tt> backed
///
/// Elements in are stored as rows of columns  \p SizeY rows of \p SizeX
/// elements:
///
/// For values without a default constructor, will initialize the elements from:
/// - A constant value (if copy-constructible)
/// - A functor or function that takes the \p x and \p y coordinates, and constructs \p T
///
/// @tparam T Element Type
/// @tparam SizeX X Dimensions
/// @tparam SizeY Y Dimensions
template <typename T, size_t SizeX, size_t SizeY>
struct array2d {
        struct sentinel;
        struct iterator;
        struct const_iterator;
        struct index;

        using index_type = index;
        using size_type = size_t;
        using value_type = T;

        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        struct index {
            size_t px;
            size_t py;

            constexpr size_t to_offset() const {
                return ( py * SizeX ) + px;
            }
            constexpr static index from_offset( const size_t pos ) {
                return index{pos / SizeX, pos % SizeX};
            }

            constexpr index &operator++() {
                ++px;
                if( px >= SizeX ) {
                    px = 0;
                    py++;
                }
                return *this;
            }

            constexpr index &operator--() {
                if( px == 0 ) {
                    px = SizeX - 1;
                    py--;
                } else {
                    --px;
                }
                return *this;
            }

            constexpr index operator++( int ) {
                index temp = *this;
                this->operator++();
                return temp;
            }

            constexpr index operator--( int ) {
                index temp = *this;
                this->operator--();
                return temp;
            }
        };

        struct iterator {
                friend array2d;

            private:
                array2d &arr;
                index_type index;

                iterator( array2d &arr, index_type i ) : arr( arr ), index( i ) {}

            public:
                reference operator*() const {
                    return arr[index];
                }
                pointer operator->() {
                    return &*arr[index];
                }

                iterator &operator++() {
                    ++index;
                    return *this;
                }

                iterator &operator--() {
                    --index;
                    return *this;
                }

                iterator operator++( int ) {
                    iterator tmp = *this;
                    ++index;
                    return tmp;
                }
                iterator operator--( int ) {
                    iterator tmp = *this;
                    --index;
                    return tmp;
                }

                bool operator== ( const sentinel & ) const { return index.px >= SizeX || index.py >= SizeY; };
                bool operator== ( const iterator &other ) const { return index == other.index && arr == other.arr; };
        };

        struct const_iterator {
                friend array2d;

            private:
                const array2d &arr;
                index_type index;

                const_iterator( const array2d &arr, index_type i ) : arr( arr ), index( i ) {}

            public:
                const_reference operator*() const {
                    return arr[index];
                }
                const_pointer operator->() {
                    return &*arr[index];
                }

                const_iterator &operator++() {
                    ++index;
                    return *this;
                }

                const_iterator &operator--() {
                    --index;
                    return *this;
                }

                const_iterator operator++( int ) {
                    const_iterator tmp = *this;
                    ++index;
                    return tmp;
                }
                const_iterator operator--( int ) {
                    const_iterator tmp = *this;
                    --index;
                    return tmp;
                }

                bool operator== ( const sentinel & ) const { return index.px >= SizeX || index.py >= SizeY; };
                bool operator== ( const iterator &other ) const { return index == other.index && arr == other.arr; };
                bool operator== ( const const_iterator &other ) const { return index == other.index && arr == other.arr; };
        };

        struct sentinel {
            bool operator== ( const sentinel & ) { return true; }
            bool operator!= ( const sentinel & ) { return false; }
        };

        array2d() = default;
        array2d( const array2d & ) = default;
        array2d( array2d && ) = default;

        template <typename F>
        requires std::is_same_v<T, std::invoke_result_t<F, size_t, size_t >>
                explicit array2d( F &&fn )
                    : _data{detail::init_array_2d<SizeX, SizeY>( fn ) } { }

        explicit array2d( const T &value )
            : _data{detail::init_array_2d<SizeX, SizeY>( [ & ]( size_t, size_t ) { return value; } )} { }

        void reset() {
            _data = {};
        }

        void reset( const T &value ) {
            _data = detail::init_array_2d<SizeX, SizeY>( [&]( size_t, size_t ) { return value; } );
        }

        template <typename F>
        requires std::is_same_v<T, std::invoke_result_t<F, size_t, size_t >>
        void reset( F &&fn ) {
            _data = detail::init_array_2d<SizeX, SizeY>( fn );
        }

        pointer data() { return &_data[0][0]; }
        const_pointer data() const { return &_data[0][0]; }

        iterator begin() { return iterator( *this, {0, 0} ); }
        sentinel end() const { return sentinel{}; }
        const_iterator begin() const { return const_iterator( *this, {0, 0} ); }

        // _data is an Y sized array, of X sized arrays, of T elements
        // we access Y first then X to reach T

        reference operator[]( index_type pos ) { return _data[pos.py][pos.px]; }
        const_reference operator[]( index_type pos ) const { return _data[pos.py][pos.px]; }
        reference operator[]( size_type px, size_type py ) { return operator[]( index_type{px, py} ); }
        const_reference operator[]( size_type px, size_type py ) const { return operator[]( index_type{px, py} ); }
        reference operator[]( size_type offset ) { return operator[]( index_type::from_offset( offset ) ); }
        const_reference operator[]( size_type offset ) const { return operator[]( index_type::from_offset( offset ) ); }

        static constexpr size_type size() { return SizeX * SizeY; }
        static constexpr index_type size_2d() { return index_type{SizeX, SizeY}; }
        static constexpr index_type index_2d( size_type x, size_type y ) { return index_type{x, y}; }

        friend void swap( array2d &lhs, array2d &rhs ) noexcept { std::swap( lhs._data, rhs._data ); }

        bool operator== ( const array2d &other ) const { return data() == other.data() && size() == other.size(); }
    private:
        std::array<std::array<T, SizeX>, SizeY> _data;
};

