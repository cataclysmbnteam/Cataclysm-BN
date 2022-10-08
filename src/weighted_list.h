#pragma once
#ifndef CATA_SRC_WEIGHTED_LIST_H
#define CATA_SRC_WEIGHTED_LIST_H

#include <climits>
#include <cstdlib>
#include <functional>
#include <vector>

template <typename W, typename T> struct weighted_object {
    weighted_object( const T &obj, const W &weight ) : obj( obj ), weight( weight ) {}

    T obj;
    W weight;
};

namespace weighted_list_detail
{
auto gen_rand_i() -> unsigned int;
} // namespace weighted_list_detail

template <typename W, typename T> struct weighted_list {
        weighted_list() : total_weight( 0 ) { }

        virtual ~weighted_list() = default;

        /**
         * This will add a new object to the weighted list. Returns a pointer to
           the added object, or NULL if weight was zero.
         * @param obj The object that will be added to the list.
         * @param weight The weight of the object.
         */
        auto add( const T &obj, const W &weight ) -> T * {
            if( weight >= 0 ) {
                objects.emplace_back( obj, weight );
                total_weight += weight;
                invalidate_precalc();
                return &( objects[objects.size() - 1].obj );
            }
            return nullptr;
        }

        /**
         * This will check to see if the given object is already in the weighted
           list. If it is, we update its weight with the new weight value. If it
           is not, we add it normally. Returns a pointer to the added or updated
           object, or NULL if weight was zero.
         * @param obj The object that will be updated or added to the list.
         * @param weight The new weight of the object.
         */
        auto add_or_replace( const T &obj, const W &weight ) -> T * {
            if( weight >= 0 ) {
                invalidate_precalc();
                for( auto &itr : objects ) {
                    if( itr.obj == obj ) {
                        total_weight += ( weight - itr.weight );
                        itr.weight = weight;
                        return &( itr.obj );
                    }
                }

                // if not found, add to end of list
                return add( obj, weight );
            }
            return nullptr;
        }

        /**
         * This will call the given callback function once for every object in the weighted list.
         * @param func The callback function.
         */
        void apply( std::function<void( const T & )> func ) const {
            for( auto &itr : objects ) {
                func( itr.obj );
            }
        }

        /**
         * This will call the given callback function once for every object in the weighted list.
         * This is the non-const version.
         * @param func The callback function.
         */
        void apply( std::function<void( T & )> func ) {
            for( auto &itr : objects ) {
                func( itr.obj );
            }
        }

        /**
         * This will return a pointer to an object from the list randomly selected
         * and biased by weight. If the weighted list is empty or all items in it
         * have a weight of zero, it will return a NULL pointer.
         */
        auto pick( unsigned int randi ) const -> const T * {
            if( total_weight > 0 ) {
                return &( objects[pick_ent( randi )].obj );
            } else {
                return nullptr;
            }
        }
        auto pick() const -> const T * {
            return pick( weighted_list_detail::gen_rand_i() );
        }

        /**
         * This will return a pointer to an object from the list randomly selected
         * and biased by weight. If the weighted list is empty or all items in it
         * have a weight of zero, it will return a NULL pointer. This is the
         * non-const version so that the returned result may be modified.
         */
        auto pick( unsigned int randi ) -> T * {
            if( total_weight > 0 ) {
                return &( objects[pick_ent( randi )].obj );
            } else {
                return nullptr;
            }
        }
        auto pick() -> T * {
            return pick( weighted_list_detail::gen_rand_i() );
        }

        /**
         * This will remove all objects from the list.
         */
        void clear() {
            total_weight = 0;
            objects.clear();
            invalidate_precalc();
        }

        /**
         * This will return the weight of a specific object. If the object is not
         * in the weighted list it will return 0.
         */
        auto get_specific_weight( const T &obj ) const -> W {
            for( auto &itr : objects ) {
                if( itr.obj == obj ) {
                    return itr.weight;
                }
            }
            return 0;
        }

        /**
         * This will return the sum of all the object's weights in the list.
         */
        auto get_weight() const -> W {
            return total_weight;
        }

        auto begin() -> typename std::vector<weighted_object<W, T> >::iterator {
            return objects.begin();
        }
        auto end() -> typename std::vector<weighted_object<W, T> >::iterator {
            return objects.end();
        }
        auto begin() const -> typename std::vector<weighted_object<W, T> >::const_iterator {
            return objects.begin();
        }
        auto end() const -> typename std::vector<weighted_object<W, T> >::const_iterator {
            return objects.end();
        }
        auto erase(
            typename std::vector<weighted_object<W, T> >::iterator first,
            typename std::vector<weighted_object<W, T> >::iterator last ) -> typename std::vector<weighted_object<W, T> >::iterator {
            invalidate_precalc();
            return objects.erase( first, last );
        }
        auto size() const noexcept -> size_t {
            return objects.size();
        }
        auto empty() const noexcept -> bool {
            return objects.empty();
        }

        void precalc();

    protected:
        W total_weight;
        std::vector<weighted_object<W, T> > objects;

        virtual auto pick_ent( unsigned int ) const -> size_t = 0;
        virtual void invalidate_precalc() {}
};

template <typename T> struct weighted_int_list : public weighted_list<int, T> {

        // populate the precalc_array for O(1) lookups
        void precalc() {
            precalc_array.clear();
            // to avoid additional reallocations
            precalc_array.reserve( this->total_weight );
            // weights [3,1,5] will produce vector of indices [0,0,0,1,2,2,2,2,2]
            for( size_t i = 0; i < this->objects.size(); i++ ) {
                precalc_array.resize( precalc_array.size() + this->objects[i].weight, i );
            }
        }

    protected:

        auto pick_ent( unsigned int randi ) const -> size_t override {
            if( this->objects.size() == 1 ) {
                return 0;
            }
            size_t i;
            const int picked = ( randi % ( this->total_weight ) ) + 1;
            if( !precalc_array.empty() ) {
                // if the precalc_array is populated, use it for O(1) lookup
                i = precalc_array[picked - 1];
            } else {
                // otherwise do O(N) search through items
                int accumulated_weight = 0;
                for( i = 0; i < this->objects.size(); i++ ) {
                    accumulated_weight += this->objects[i].weight;
                    if( accumulated_weight >= picked ) {
                        break;
                    }
                }
            }
            return i;
        }

        void invalidate_precalc() override {
            precalc_array.clear();
        }

        std::vector<int> precalc_array;
};

template <typename T> struct weighted_float_list : public weighted_list<double, T> {

        // TODO: precalc using alias method

    protected:

        auto pick_ent( unsigned int randi ) const -> size_t override {
            const double picked = static_cast<double>( randi ) / UINT_MAX * this->total_weight;
            double accumulated_weight = 0;
            size_t i;
            for( i = 0; i < this->objects.size(); i++ ) {
                accumulated_weight += this->objects[i].weight;
                if( accumulated_weight >= picked ) {
                    break;
                }
            }
            return i;
        }

};

#endif // CATA_SRC_WEIGHTED_LIST_H
