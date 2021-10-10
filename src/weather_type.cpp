#include "weather_type.h"

#include "game_constants.h"
#include "generic_factory.h"
#include "weather.h"

namespace
{
generic_factory<weather_type> weather_type_factory( "weather_type" );
} // namespace


namespace io
{
template<>
std::string enum_to_string<precip_class>( precip_class data )
{
    switch( data ) {
        case precip_class::none:
            return "none";
        case precip_class::very_light:
            return "very_light";
        case precip_class::light:
            return "light";
        case precip_class::heavy:
            return "heavy";
        case precip_class::last:
            break;
    }
    debugmsg( "Invalid precip_class" );
    abort();
}

template<>
std::string enum_to_string<sun_intensity_type>( sun_intensity_type data )
{
    switch( data ) {
        case sun_intensity_type::none:
            return "none";
        case sun_intensity_type::light:
            return "light";
        case sun_intensity_type::normal:
            return "normal";
        case sun_intensity_type::high:
            return "high";
        case sun_intensity_type::last:
            break;
    }
    debugmsg( "Invalid sun_intensity_type" );
    abort();
}

template<>
std::string enum_to_string<weather_time_requirement_type>( weather_time_requirement_type data )
{
    switch( data ) {
        case weather_time_requirement_type::day:
            return "day";
        case weather_time_requirement_type::night:
            return "night";
        case weather_time_requirement_type::both:
            return "both";
        case weather_time_requirement_type::last:
            break;
    }
    debugmsg( "Invalid time_requirement_type" );
    abort();
}

template<>
std::string enum_to_string<weather_sound_category>( weather_sound_category data )
{
    switch( data ) {
        case weather_sound_category::drizzle:
            return "drizzle";
        case weather_sound_category::flurries:
            return "flurries";
        case weather_sound_category::rainy:
            return "rainy";
        case weather_sound_category::snow:
            return "snow";
        case weather_sound_category::snowstorm:
            return "snowstorm";
        case weather_sound_category::thunder:
            return "thunder";
        case weather_sound_category::silent:
            return "silent";
        case weather_sound_category::last:
            break;
    }
    debugmsg( "Invalid time_requirement_type" );
    abort();
}

} // namespace io

template<>
const weather_type &weather_type_id::obj() const
{
    return weather_type_factory.obj( *this );
}

/** @relates string_id */
template<>
bool string_id<weather_type>::is_valid() const
{
    return weather_type_factory.is_valid( *this );
}

void weather_type::check() const
{
    for( const weather_type_id &required : requirements.required_weathers ) {
        if( !required.is_valid() ) {
            // This may be important, throw error and abort loading.
            throw string_format( R"(Weather type "%s" required for weather type "%s" does not exist.)",
                                 required, id );
        }
    }
}

void weather_type::load( const JsonObject &jo, const std::string & )
{
    mandatory( jo, was_loaded, "name", name );
    mandatory( jo, was_loaded, "id",  id );

    optional( jo, was_loaded, "color", color );
    optional( jo, was_loaded, "map_color", map_color );
    mandatory( jo, was_loaded, "glyph", symbol, unicode_codepoint_from_symbol_reader );

    mandatory( jo, was_loaded, "ranged_penalty", ranged_penalty );
    mandatory( jo, was_loaded, "sight_penalty", sight_penalty );
    mandatory( jo, was_loaded, "light_modifier", light_modifier );

    mandatory( jo, was_loaded, "sound_attn", sound_attn );
    mandatory( jo, was_loaded, "dangerous", dangerous );
    mandatory( jo, was_loaded, "precip", precip );
    mandatory( jo, was_loaded, "rains", rains );
    optional( jo, was_loaded, "acidic", acidic, false );
    optional( jo, was_loaded, "sound_category", sound_category, weather_sound_category::silent );
    mandatory( jo, was_loaded, "sun_intensity", sun_intensity );

    for( const JsonObject weather_effect : jo.get_array( "effects" ) ) {

        std::pair<std::string, int> pair = std::make_pair( weather_effect.get_string( "name" ),
                                           weather_effect.get_int( "intensity" ) );

        static const std::map<std::string, weather_effect_fn> all_weather_effects = {
            { "wet", &weather_effect::wet_player },
            { "thunder", &weather_effect::thunder },
            { "lightning", &weather_effect::lightning },
            { "light_acid", &weather_effect::light_acid },
            { "acid", &weather_effect::acid }
        };
        const auto iter = all_weather_effects.find( pair.first );
        if( iter == all_weather_effects.end() ) {
            weather_effect.throw_error( "Invalid weather effect", "name" );
        }
        effects.emplace_back( iter->second, pair.second );
    }

    if( jo.has_member( "animation" ) ) {
        animation = {};
        JsonObject j = jo.get_object( "animation" );

        mandatory( j, was_loaded, "factor", animation.factor );
        mandatory( j, was_loaded, "tile", animation.tile );
        mandatory( j, was_loaded, "color", animation.color );
        mandatory( j, was_loaded, "glyph", animation.symbol, unicode_codepoint_from_symbol_reader );
    }

    if( jo.has_member( "requirements" ) ) {
        requirements = {};
        JsonObject j = jo.get_object( "requirements" );

        optional( j, was_loaded, "pressure_min", requirements.pressure_min, INT_MIN );
        optional( j, was_loaded, "pressure_max", requirements.pressure_max, INT_MAX );
        optional( j, was_loaded, "humidity_min", requirements.humidity_min, INT_MIN );
        optional( j, was_loaded, "humidity_max", requirements.humidity_max, INT_MAX );
        optional( j, was_loaded, "temperature_min", requirements.temperature_min, INT_MIN );
        optional( j, was_loaded, "temperature_max", requirements.temperature_max, INT_MAX );
        optional( j, was_loaded, "windpower_min", requirements.windpower_min, INT_MIN );
        optional( j, was_loaded, "windpower_max", requirements.windpower_max, INT_MAX );
        optional( j, was_loaded, "humidity_and_pressure", requirements.humidity_and_pressure, true );
        optional( j, was_loaded, "acidic", requirements.acidic, false );
        optional( j, was_loaded, "time", requirements.time, weather_time_requirement_type::both );
        optional( j, was_loaded, "required_weathers", requirements.required_weathers );
    }
}

void weather_types::reset()
{
    weather_type_factory.reset();
}

void weather_types::finalize_all()
{
    weather_type_factory.finalize();
}

const std::vector<weather_type> &weather_types::get_all()
{
    return weather_type_factory.get_all();
}

void weather_types::check_consistency()
{
    weather_type_factory.check();
}

void weather_types::load( const JsonObject &jo, const std::string &src )
{
    weather_type_factory.load( jo, src );
}

