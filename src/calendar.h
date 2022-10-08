#pragma once
#ifndef CATA_SRC_CALENDAR_H
#define CATA_SRC_CALENDAR_H

#include <string>
#include <utility>
#include <vector>

class calendar_config;
class JsonIn;
class JsonOut;
class time_duration;
class time_point;
template<typename T> struct enum_traits;

/** Real world seasons */
enum season_type {
    SPRING = 0,
    SUMMER = 1,
    AUTUMN = 2,
    WINTER = 3,
    NUM_SEASONS
};

template<>
struct enum_traits<season_type> {
    static constexpr season_type last = season_type::NUM_SEASONS;
};

/** Phases of the moon */
enum moon_phase {
    /** New (completely dark) moon */
    MOON_NEW = 0,
    /** One quarter of moon lit, amount lit is increasing every day */
    MOON_WAXING_CRESCENT,
    /** One half of moon lit, amount lit is increasing every day */
    MOON_HALF_MOON_WAXING,
    /** Three quarters of moon lit, amount lit is increasing every day */
    MOON_WAXING_GIBBOUS,
    /** Full moon */
    MOON_FULL,
    /** Three quarters of moon lit, amount lit decreases every day */
    MOON_WANING_GIBBOUS,
    /** Half of moon is lit, amount lit decreases every day */
    MOON_HALF_MOON_WANING,
    /** One quarter of moon lit, amount lit decreases every day */
    MOON_WANING_CRESCENT,
    /** Not a valid moon phase, but can be used for iterating through enum */
    MOON_PHASE_MAX
};

/**
 * Time keeping namespace that holds global time.
 *
 * Encapsulates the current time of day, date, and year.  Also tracks seasonal variation in day
 * length, the passing of the seasons themselves, and the phases of the moon.
 */
namespace calendar
{
/**
 * Predicate to handle rate-limiting. Returns `true` after every @p event_frequency duration.
 */
auto once_every( const time_duration &event_frequency ) -> bool;

/**
 * A number that represents the longest possible action.
 *
 * This number should be regarded as a number of turns, and can safely be converted to a
 * number of seconds or moves (movement points) without integer overflow.  If used to
 * represent a larger unit (hours/days/years), then this will result in integer overflow.
 */
extern const int INDEFINITELY_LONG;

/**
 * The expected duration of the cataclysm
 *
 * Large duration that can be used to approximate infinite amounts of time.
 *
 * This number can't be safely converted to a number of moves without causing
 * an integer overflow.
 */
extern const time_duration INDEFINITELY_LONG_DURATION;

/// @returns Whether the eternal season is enabled.
auto eternal_season() -> bool;
void set_eternal_season( bool is_eternal_season );

/** @returns Time in a year, (configured in current world settings) */
auto year_length() -> time_duration;

/** @returns Time of a season (configured in current world settings) */
auto season_length() -> time_duration;
void set_season_length( int dur );

/**
 * @returns ratio of actual season length (a world option) to default season length. This
 * should be used to convert JSON values (that assume the default for the season length
 * option) to actual in-game length.
 */
auto season_ratio() -> float;

/** Returns the translated name of the season (with first letter being uppercase). */
auto name_season( season_type s ) -> std::string;

const extern time_point &start_of_cataclysm;
const extern time_point &start_of_game;
const extern season_type &initial_season;
extern time_point turn;

extern calendar_config config;

void set_calendar_config( const calendar_config & );

/**
 * A time point that is always before the current turn, even when the game has
 * just started. This implies `before_time_starts < calendar::turn` is always
 * true. It can be used to initialize `time_point` values that denote that last
 * time a cache was update.
 */
extern const time_point before_time_starts;
/**
 * Represents time point 0.
 * TODO: flesh out the documentation
 */
extern const time_point turn_zero;
} // namespace calendar

template<typename T>
constexpr auto to_turns( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_seconds( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_minutes( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_hours( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_days( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_weeks( const time_duration &duration ) -> T;
template<typename T>
constexpr auto to_moves( const time_duration &duration ) -> T;

template<typename T>
constexpr auto to_turn( const time_point &point ) -> T;

template<typename T>
constexpr auto operator/( const time_duration &lhs, T rhs ) -> time_duration;
template<typename T>
inline auto operator/=( time_duration &lhs, T rhs ) -> time_duration &;
template<typename T>
constexpr auto operator*( const time_duration &lhs, T rhs ) -> time_duration;
template<typename T>
constexpr auto operator*( T lhs, const time_duration &rhs ) -> time_duration;
template<typename T>
inline auto operator*=( time_duration &lhs, T rhs ) -> time_duration &;

/**
 * A duration defined as a number of specific time units.
 * Currently it stores the number (as integer) of turns.
 * Note: currently variable season length is ignored by this class (N turns are
 * always N turns) and there is no way to create an instance from units larger
 * than days (e.g. seasons or years) and no way to convert a duration into those
 * units directly. (You can still use @ref calendar to do this.)
 *
 * Operators for time points and time duration are defined as one may expect them:
 * -duration ==> duration (inverse)
 * point - point ==> duration (duration between to points in time)
 * point + duration ==> point (the revers of above)
 * point - duration ==> point (same as: point + -duration)
 * duration + duration ==> duration
 * duration - duration ==> duration (same as: duration + -duration)
 * duration * scalar ==> duration (simple scaling yields a double with precise value)
 * scalar * duration ==> duration (same as above)
 * duration / duration ==> scalar (revers of above)
 * duration / scalar ==> duration (same as: duration * 1/scalar)
 * duration % duration ==> duration ("remainder" of duration / some integer)
 * Also shortcuts: += and -= and *= and /=
 */
class time_duration
{
    private:
        friend class time_point;
        int turns_;

        explicit constexpr time_duration( const int t ) : turns_( t ) { }

    public:
        time_duration() : turns_( 0 ) {}

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        /**
         * Named constructors to get a duration representing a multiple of the named time
         * units. Note that a duration is stored as integer number of turns, so
         * `from_minutes( 0.0001 )` will be stored as "0 turns".
         * The template type is used for the conversion from given time unit to turns, so
         * `from_hours( 0.5 )` will yield "1800 turns".
         * Conversion of units greater than days (seasons) is not supported because they
         * depend on option settings ("season length").
         */
        /**@{*/
        template<typename T>
        static constexpr auto from_turns( const T t ) -> time_duration {
            return time_duration( t );
        }
        template<typename T>
        static constexpr auto from_seconds( const T t ) -> time_duration {
            return time_duration( t );
        }
        template<typename T>
        static constexpr auto from_minutes( const T m ) -> time_duration {
            return from_turns( m * 60 );
        }
        template<typename T>
        static constexpr auto from_hours( const T h ) -> time_duration {
            return from_minutes( h * 60 );
        }
        template<typename T>
        static constexpr auto from_days( const T d ) -> time_duration {
            return from_hours( d * 24 );
        }
        template<typename T>
        static constexpr auto from_weeks( const T d ) -> time_duration {
            return from_days( d * 7 );
        }
        /**@}*/

        /**
         * Converts the duration to an amount of the given units. The conversions is
         * done with values of the given template type. That means using an integer
         * type (e.g. `int`) will return a truncated value (amount of *full* minutes
         * that make up the duration, discarding the remainder).
         * Calling `to_minutes<double>` will return a precise number.
         * Example:
         * `to_hours<int>( from_minutes( 90 ) ) == 1`
         * `to_hours<double>( from_minutes( 90 ) ) == 1.5`
         */
        /**@{*/
        template<typename T>
        friend constexpr auto to_turns( const time_duration &duration ) -> T {
            return duration.turns_;
        }
        template<typename T>
        friend constexpr auto to_seconds( const time_duration &duration ) -> T {
            return duration.turns_;
        }
        template<typename T>
        friend constexpr auto to_minutes( const time_duration &duration ) -> T {
            return static_cast<T>( duration.turns_ ) / static_cast<T>( 60 );
        }
        template<typename T>
        friend constexpr auto to_hours( const time_duration &duration ) -> T {
            return static_cast<T>( duration.turns_ ) / static_cast<T>( 60 * 60 );
        }
        template<typename T>
        friend constexpr auto to_days( const time_duration &duration ) -> T {
            return static_cast<T>( duration.turns_ ) / static_cast<T>( 60 * 60 * 24 );
        }
        template<typename T>
        friend constexpr auto to_weeks( const time_duration &duration ) -> T {
            return static_cast<T>( duration.turns_ ) / static_cast<T>( 60 * 60 * 24 * 7 );
        }
        template<typename T>
        friend constexpr auto to_moves( const time_duration &duration ) -> T {
            return to_turns<int>( duration ) * 100;
        }
        /**@{*/

        constexpr auto operator<( const time_duration &rhs ) const -> bool {
            return turns_ < rhs.turns_;
        }
        constexpr auto operator<=( const time_duration &rhs ) const -> bool {
            return turns_ <= rhs.turns_;
        }
        constexpr auto operator>( const time_duration &rhs ) const -> bool {
            return turns_ > rhs.turns_;
        }
        constexpr auto operator>=( const time_duration &rhs ) const -> bool {
            return turns_ >= rhs.turns_;
        }
        constexpr auto operator==( const time_duration &rhs ) const -> bool {
            return turns_ == rhs.turns_;
        }
        constexpr auto operator!=( const time_duration &rhs ) const -> bool {
            return turns_ != rhs.turns_;
        }

        friend constexpr auto operator-( const time_duration &duration ) -> time_duration {
            return time_duration( -duration.turns_ );
        }
        friend constexpr auto operator+( const time_duration &lhs, const time_duration &rhs ) -> time_duration {
            return time_duration( lhs.turns_ + rhs.turns_ );
        }
        friend auto operator+=( time_duration &lhs, const time_duration &rhs ) -> time_duration & {
            return lhs = time_duration( lhs.turns_ + rhs.turns_ );
        }
        friend constexpr auto operator-( const time_duration &lhs, const time_duration &rhs ) -> time_duration {
            return time_duration( lhs.turns_ - rhs.turns_ );
        }
        friend auto operator-=( time_duration &lhs, const time_duration &rhs ) -> time_duration & {
            return lhs = time_duration( lhs.turns_ - rhs.turns_ );
        }
        // Using double here because it has the highest precision. Callers can cast it to whatever they want.
        friend auto operator/( const time_duration &lhs, const time_duration &rhs ) -> double {
            return static_cast<double>( lhs.turns_ ) / static_cast<double>( rhs.turns_ );
        }
        template<typename T>
        friend constexpr auto operator/( const time_duration &lhs, const T rhs ) -> time_duration {
            return time_duration( lhs.turns_ / rhs );
        }
        template<typename T>
        friend auto operator/=( time_duration &lhs, const T rhs ) -> time_duration & {
            return lhs = time_duration( lhs.turns_ / rhs );
        }
        template<typename T>
        friend constexpr auto operator*( const time_duration &lhs, const T rhs ) -> time_duration {
            return time_duration( lhs.turns_ * rhs );
        }
        template<typename T>
        friend constexpr auto operator*( const T lhs, const time_duration &rhs ) -> time_duration {
            return time_duration( lhs * rhs.turns_ );
        }
        template<typename T>
        friend auto operator*=( time_duration &lhs, const T rhs ) -> time_duration & {
            return lhs = time_duration( lhs.turns_ * rhs );
        }
        friend auto operator%( const time_duration &lhs, const time_duration &rhs ) -> time_duration {
            return time_duration( lhs.turns_ % rhs.turns_ );
        }

        /// Returns a random duration in the range [low, hi].
        friend auto rng( time_duration lo, time_duration hi ) -> time_duration;

        static const std::vector<std::pair<std::string, time_duration>> units;
};

/// @see x_in_y(int,int)
auto x_in_y( const time_duration &a, const time_duration &b ) -> bool;

/**
 * Convert the given number into an duration by calling the matching
 * `time_duration::from_*` function.
 */
/**@{*/
constexpr auto operator"" _turns( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_turns( v );
}
constexpr auto operator"" _seconds( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_seconds( v );
}
constexpr auto operator"" _minutes( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_minutes( v );
}
constexpr auto operator"" _hours( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_hours( v );
}
constexpr auto operator"" _days( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_days( v );
}
constexpr auto operator"" _weeks( const unsigned long long int v ) -> time_duration
{
    return time_duration::from_weeks( v );
}
/**@}*/

/**
 * Returns a string showing a duration. The string contains at most two numbers
 * along with their units. E.g. 3661 seconds will return "1 hour and 1 minute"
 * (the 1 additional second is clipped). An input of 3601 will return "1 hour"
 * (the second is clipped again and the number of additional minutes would be
 * 0 so it's skipped).
 */
auto to_string( const time_duration &d ) -> std::string;

enum class clipped_align {
    none,
    right,
};

enum class clipped_unit {
    forever,
    second,
    minute,
    hour,
    day,
    week,
    season,
    year,
};

/**
 * Returns a value representing the passed in duration truncated to an appropriate unit
 * along with the unit in question.
 * "10 days" or "1 minute".
 * The chosen unit will be the smallest unit, that is at least as much as the
 * given duration. E.g. an input of 60 minutes will return "1 hour", an input of
 * 59 minutes will return "59 minutes".
 */
auto clipped_time( const time_duration &d ) -> std::pair<int, clipped_unit>;

/**
 * Returns a string showing a duration as whole number of appropriate units, e.g.
 * "10 days" or "1 minute".
 * The chosen unit will be the smallest unit, that is at least as much as the
 * given duration. E.g. an input of 60 minutes will return "1 hour", an input of
 * 59 minutes will return "59 minutes".
 */
auto to_string_clipped( const time_duration &d, clipped_align align = clipped_align::none ) -> std::string;
/**
 * Returns approximate duration.
 * @param verbose If true, 'less than' and 'more than' will be printed instead of '<' and '>' respectively.
 */
auto to_string_approx( const time_duration &dur, bool verbose = true ) -> std::string;

/**
 * A point in the game time. Use `calendar::turn` to get the current point.
 * Modify it by adding/subtracting @ref time_duration.
 * This can be compared with the usual comparison operators.
 * It can be (de)serialized via JSON.
 *
 * Note that is does not handle variable sized season length. Changing the
 * season length has no effect on it.
 */
class time_point
{
    private:
        friend class time_duration;
        int turn_;

        explicit constexpr time_point( const int t ) : turn_( t ) { }

    public:
        time_point();

        // TODO: remove this, nobody should need it, one should use a constant `time_point`
        // (representing turn 0) and a `time_duration` instead.
        static constexpr auto from_turn( const int t ) -> time_point {
            return time_point( t );
        }

        void serialize( JsonOut &jsout ) const;
        void deserialize( JsonIn &jsin );

        // TODO: try to get rid of this
        template<typename T>
        friend constexpr auto to_turn( const time_point &point ) -> T {
            return point.turn_;
        }

        friend constexpr inline auto operator<( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) < to_turn<int>( rhs );
        }
        friend constexpr inline auto operator<=( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) <= to_turn<int>( rhs );
        }
        friend constexpr inline auto operator>( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) > to_turn<int>( rhs );
        }
        friend constexpr inline auto operator>=( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) >= to_turn<int>( rhs );
        }
        friend constexpr inline auto operator==( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) == to_turn<int>( rhs );
        }
        friend constexpr inline auto operator!=( const time_point &lhs, const time_point &rhs ) -> bool {
            return to_turn<int>( lhs ) != to_turn<int>( rhs );
        }

        friend constexpr inline auto operator-(
            const time_point &lhs, const time_point &rhs ) -> time_duration {
            return time_duration::from_turns( to_turn<int>( lhs ) - to_turn<int>( rhs ) );
        }
        friend constexpr inline auto operator+(
            const time_point &lhs, const time_duration &rhs ) -> time_point {
            return time_point::from_turn( to_turn<int>( lhs ) + to_turns<int>( rhs ) );
        }
        friend inline auto operator+=( time_point &lhs, const time_duration &rhs ) -> time_point & {
            return lhs = time_point::from_turn( to_turn<int>( lhs ) + to_turns<int>( rhs ) );
        }
        friend constexpr inline auto operator-(
            const time_point &lhs, const time_duration &rhs ) -> time_point {
            return time_point::from_turn( to_turn<int>( lhs ) - to_turns<int>( rhs ) );
        }
        friend inline auto operator-=( time_point &lhs, const time_duration &rhs ) -> time_point & {
            return lhs = time_point::from_turn( to_turn<int>( lhs ) - to_turns<int>( rhs ) );
        }

        // TODO: implement minutes_of_hour and so on and use it.
};

inline auto time_past_midnight( const time_point &p ) -> time_duration
{
    return ( p - calendar::turn_zero ) % 1_days;
}

inline auto time_past_new_year( const time_point &p ) -> time_duration
{
    return ( p - calendar::turn_zero ) % calendar::year_length();
}

template<typename T>
inline auto minute_of_hour( const time_point &p ) -> T
{
    return to_minutes<T>( ( p - calendar::turn_zero ) % 1_hours );
}

template<typename T>
inline auto hour_of_day( const time_point &p ) -> T
{
    return to_hours<T>( ( p - calendar::turn_zero ) % 1_days );
}

/// This uses the current season length.
template<typename T>
inline auto day_of_season( const time_point &p ) -> T
{
    return to_days<T>( ( p - calendar::turn_zero ) % calendar::season_length() );
}

/**
 * A class that keeps time data other than current time. Similar to @ref calendar namespace, but not static and so more testable.
 */
class calendar_config
{
    private:
        time_point _start_of_cataclysm;
        time_point _start_of_game;
        season_type _initial_season;

        bool _eternal_season;

        // TODO: Remove and make the class immutable, have set_calendar in game or something
        friend class game;
        friend void calendar::set_calendar_config( const calendar_config & );

        auto operator=( const calendar_config & ) -> calendar_config & = default;

    public:
        calendar_config( time_point start_of_cataclysm,
                         time_point start_of_game,
                         season_type initial_season,
                         bool eternal_season )
            : _start_of_cataclysm( start_of_cataclysm )
            , _start_of_game( start_of_game )
            , _initial_season( initial_season )
            , _eternal_season( eternal_season )
        {}
        calendar_config( const calendar_config & ) = default;

        // TODO: Remove reference bit after testing!
        auto start_of_cataclysm() const -> const time_point & {
            return _start_of_cataclysm;
        }
        auto start_of_game() const -> const time_point & {
            return _start_of_game;
        }
        auto initial_season() const -> const season_type & {
            return _initial_season;
        }
        auto eternal_season() const -> bool {
            return _eternal_season;
        }
        auto season_length() const -> time_duration {
            // TODO: Store it in calendar config once the rest of the game looks at config properly
            return calendar::season_length();
        }
};

/// @returns The season of the of the given time point. Returns the same season for
/// any input if the calendar::eternal_season yields true.
auto season_of_year( const time_point &p ) -> season_type;
/// @returns The time point formatted to be shown to the player. Contains year, season, day and time of day.
auto to_string( const time_point &p ) -> std::string;
/// @returns The time point formatted to be shown to the player. Contains only the time of day, not the year, day or season.
auto to_string_time_of_day( const time_point &p ) -> std::string;
/** Returns the current light level of the moon. */
auto get_moon_phase( const time_point &p ) -> moon_phase;
/** Returns the current sunrise time based on the time of year. */
auto sunrise( const time_point &p ) -> time_point;
/** Returns the current sunset time based on the time of year. */
auto sunset( const time_point &p ) -> time_point;
/** Returns the time it gets light based on sunrise */
auto daylight_time( const time_point &p ) -> time_point;
/** Returns the time it gets dark based on sunset */
auto night_time( const time_point &p ) -> time_point;
/** Returns true if it's currently night time - after dusk and before dawn. */
auto is_night( const time_point &p ) -> bool;
/** Returns true if it's currently day time - after dawn and before dusk. */
auto is_day( const time_point &p ) -> bool;
/** Returns true if it's currently dusk - between sunset and and twilight_duration after sunset. */
auto is_dusk( const time_point &p ) -> bool;
/** Returns true if it's currently dawn - between sunrise and twilight_duration after sunrise. */
auto is_dawn( const time_point &p ) -> bool;
/** Returns the current seasonally-adjusted maximum daylight level */
auto current_daylight_level( const time_point &p ) -> double;
/** How much light is provided in full daylight */
auto default_daylight_level() -> double;
/** Returns the current sunlight or moonlight level through the preceding functions.
 *  By default, returns sunlight level for vision, with moonlight providing a measurable amount
 *  of light.  with vision == false, returns sunlight for solar panel purposes, and moonlight
 *  provides 0 light */
auto sunlight( const time_point &p, bool vision = true ) -> float;

enum class weekdays : int {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
};

auto day_of_week( const time_point &p ) -> weekdays;

#endif // CATA_SRC_CALENDAR_H
