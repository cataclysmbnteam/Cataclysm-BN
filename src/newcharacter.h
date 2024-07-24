#pragma once
#ifndef CATA_SRC_NEWCHARACTER_H
#define CATA_SRC_NEWCHARACTER_H

#include <string>

#include "type_id.h"

class Character;

struct points_left {
    int stat_points = 0;
    int trait_points = 0;
    int skill_points = 0;

    enum point_limit : int {
        FREEFORM = 0,
        ONE_POOL,
        MULTI_POOL,
        TRANSFER,
    };
    point_limit limit = point_limit::FREEFORM;

    points_left();
    void init_from_options();
    // Highest amount of points to spend on stats without points going invalid
    int stat_points_left() const;
    int trait_points_left() const;
    int skill_points_left() const;
    bool is_freeform();
    bool is_valid();
    bool has_spare();
    std::string to_string();
};

struct character_preview_window {
        enum OrientationType : std::uint8_t {
            TOP_LEFT,
            TOP_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_RIGHT
        };
        struct Margin {
            int left = 0;
            int right = 0;
            int top = 0;
            int bottom = 0;
        };
        struct Orientation {
            OrientationType type = TOP_RIGHT;
            Margin margin = Margin{};
        };

        catacurses::window w_preview;

        void init( Character *character );
        /** Window preparations before displaying. Sets desirable position. Could also be usefull for ui-rescale **/
        void prepare( int nlines, int ncols, const Orientation *orientation, int hide_below_ncols );
        void zoom_in();
        void zoom_out();
        void toggle_clothes();
        void display() const;
        /** Use it as you done with preview **/
        void clear() const;
        bool clothes_showing() const;

    private:
        point pos;
        int termx_pixels = 0;
        int termy_pixels = 0;
        const int MIN_ZOOM = 32;
        const int MAX_ZOOM = 128;
        const int DEFAULT_ZOOM = 128;
        int zoom = DEFAULT_ZOOM;
        int hide_below_ncols = 0;
        int ncols_width = 0;
        int nlines_width = 0;
        Character *character = nullptr;
        std::vector<detached_ptr<item>> clothes;
        bool show_clothes = true;

        point calc_character_pos() const;
};

namespace newcharacter
{
/** Returns the id of a random starting trait that costs >= 0 points */
trait_id random_good_trait();
/** Returns the id of a random starting trait that costs < 0 points */
trait_id random_bad_trait();
/**
 * Adds mandatory scenario and profession traits unless character already has them.
 * And if they do, refunds the points.
 */
void add_traits( Character &ch );
void add_traits( Character &ch, points_left &points );

/** Returns true if character has a conflicting trait to the entered trait. */
bool has_conflicting_trait( const Character &ch, const trait_id &t );
/** Returns true if charcater has a trait which upgrades into the entered trait. */
bool has_lower_trait( const Character &ch, const trait_id &t );
/** Returns true if character has a trait which is an upgrade of the entered trait. */
bool has_higher_trait( const Character &ch, const trait_id &t );
/** Returns true if character has a trait that shares a type with the entered trait. */
bool has_same_type_trait( const Character &ch, const trait_id &t );
} // namespace newcharacter

#endif // CATA_SRC_NEWCHARACTER_H
