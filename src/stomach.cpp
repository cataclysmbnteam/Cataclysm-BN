#include <algorithm>
#include <cmath>
#include <utility>

#include "cata_utility.h"
#include "character.h"
#include "json.h"
#include "player.h"
#include "stomach.h"
#include "vitamin.h"

void nutrients::min_in_place( const nutrients &r )
{
    kcal = std::min( kcal, r.kcal );
    for( const std::pair<const vitamin_id, vitamin> &vit_pair : vitamin::all() ) {
        const vitamin_id &vit = vit_pair.first;
        int other = r.get_vitamin( vit );
        if( other == 0 ) {
            vitamins.erase( vit );
        } else {
            auto our_vit = vitamins.find( vit );
            if( our_vit != vitamins.end() ) {
                our_vit->second = std::min( our_vit->second, other );
            }
        }
    }
}

void nutrients::max_in_place( const nutrients &r )
{
    kcal = std::max( kcal, r.kcal );
    for( const std::pair<const vitamin_id, vitamin> &vit_pair : vitamin::all() ) {
        const vitamin_id &vit = vit_pair.first;
        int other = r.get_vitamin( vit );
        if( other != 0 ) {
            int &val = vitamins[vit];
            val = std::max( val, other );
        }
    }
}

auto nutrients::get_vitamin( const vitamin_id &vit ) const -> int
{
    auto it = vitamins.find( vit );
    if( it == vitamins.end() ) {
        return 0;
    }
    return it->second;
}

auto nutrients::operator==( const nutrients &r ) const -> bool
{
    if( kcal != r.kcal ) {
        return false;
    }
    // Can't just use vitamins == r.vitamins, because there might be zero
    // entries in the map, which need to compare equal to missing entries.
    for( const std::pair<const vitamin_id, vitamin> &vit_pair : vitamin::all() ) {
        const vitamin_id &vit = vit_pair.first;
        if( get_vitamin( vit ) != r.get_vitamin( vit ) ) {
            return false;
        }
    }
    return true;
}

auto nutrients::operator+=( const nutrients &r ) -> nutrients &
{
    kcal += r.kcal;
    for( const std::pair<const vitamin_id, int> &vit : r.vitamins ) {
        vitamins[vit.first] += vit.second;
    }
    return *this;
}

auto nutrients::operator-=( const nutrients &r ) -> nutrients &
{
    kcal -= r.kcal;
    for( const std::pair<const vitamin_id, int> &vit : r.vitamins ) {
        vitamins[vit.first] -= vit.second;
    }
    return *this;
}

auto nutrients::operator*=( int r ) -> nutrients &
{
    kcal *= r;
    for( std::pair<const vitamin_id, int> &vit : vitamins ) {
        vit.second *= r;
    }
    return *this;
}

auto nutrients::operator/=( int r ) -> nutrients &
{
    kcal = divide_round_up( kcal, r );
    for( std::pair<const vitamin_id, int> &vit : vitamins ) {
        vit.second = divide_round_up( vit.second, r );
    }
    return *this;
}

stomach_contents::stomach_contents()
{
    empty();
    last_ate = calendar::before_time_starts;
}

void stomach_contents::serialize( JsonOut &json ) const
{
    json.start_object();
    json.member( "vitamins", nutr.vitamins );
    json.member( "calories", nutr.kcal );
    json.member( "last_ate", last_ate );
    json.end_object();
}

void stomach_contents::deserialize( JsonIn &json )
{
    JsonObject jo = json.get_object();
    jo.read( "vitamins", nutr.vitamins );
    jo.read( "calories", nutr.kcal );
    jo.read( "last_ate", last_ate );
}

void stomach_contents::ingest( const food_summary &ingested )
{
    nutr += ingested.nutr;
    ate();
}

auto stomach_contents::digest( const needs_rates &metabolic_rates, int five_mins ) -> food_summary
{
    food_summary digested{};
    stomach_digest_rates rates = get_digest_rates( metabolic_rates );

    // Digest kCal -- use min_kcal by default, but no more than what's in stomach,
    // and no less than percentage_kcal of what's in stomach.
    int kcal_fraction = std::lround( nutr.kcal * rates.percent_kcal );
    digested.nutr.kcal = clamp( five_mins * rates.min_kcal, five_mins * kcal_fraction, nutr.kcal );

    // Digest vitamins just like we did kCal, but we need to do one at a time.
    for( const std::pair<const vitamin_id, int> &vit : nutr.vitamins ) {
        int vit_fraction = std::lround( vit.second * rates.percent_vitamin );
        digested.nutr.vitamins[vit.first] =
            five_mins * clamp( rates.min_vitamin, vit_fraction, vit.second );
    }

    nutr -= digested.nutr;
    return digested;
}

void stomach_contents::empty()
{
    nutr = nutrients{};
}

auto stomach_contents::get_digest_rates( const needs_rates &metabolic_rates ) -> stomach_digest_rates
{
    stomach_digest_rates rates;
    rates.min_vitamin = 1;
    rates.percent_vitamin = 1.0f / 6.0f;
    rates.min_kcal = 100;
    rates.percent_kcal = 0.5f;
    rates.min_vitamin = std::round( 100.0 / 24.0 * metabolic_rates.hunger );
    rates.percent_vitamin = 0.05f * metabolic_rates.hunger;
    return rates;
}

void stomach_contents::mod_calories( int cal )
{
    if( -cal >= nutr.kcal ) {
        nutr.kcal = 0;
        return;
    }
    nutr.kcal += cal;
}

auto stomach_contents::get_calories() const -> int
{
    return nutr.kcal;
}

void stomach_contents::ate()
{
    last_ate = calendar::turn;
}

auto stomach_contents::time_since_ate() const -> time_duration
{
    return calendar::turn - last_ate;
}

// sets default stomach contents when starting the game
void Character::initialize_stomach_contents()
{
    stomach = stomach_contents();
}
