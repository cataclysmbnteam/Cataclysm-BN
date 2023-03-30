#include "map_functions.h"

#include "character.h"
#include "game.h"
#include "map.h"
#include "map_iterator.h"
#include "messages.h"
#include "monster.h"
#include "sounds.h"

static const mtype_id mon_mi_go_myrmidon( "mon_mi_go_myrmidon" );

namespace map_funcs
{

int climbing_cost( const map &m, const tripoint &from, const tripoint &to )
{
    // TODO: All sorts of mutations, equipment weight etc. for characters
    if( !m.valid_move( from, to, false, true ) ) {
        return 0;
    }
    const int diff = m.climb_difficulty( from );
    if( diff > 5 ) {
        return 0;
    }
    return 50 + diff * 100;
}

void migo_nerve_cage_removal( map &m, const tripoint &p, bool spawn_damaged )
{
    bool open = false;
    for( const tripoint &tmp : m.points_in_radius( p, 12 ) ) {
        if( m.ter( tmp ) == ter_id( "t_wall_resin_cage" ) ) {
            m.ter_set( tmp, ter_id( "t_floor_resin" ) );
            open = true;
        }
    }
    if( open ) {
        add_msg( m_good, _( "The nerve cluster collapses in on itself, and the nearby cages open!" ) );
    } else {
        add_msg( _( "The nerve cluster collapses in on itself, to no discernible effect." ) );
    }
    sounds::sound( p, 120, sounds::sound_t::combat,
                   _( "a loud alien shriek reverberating through the structure!" ), true,
                   "shout", "scream_tortured" );
    monster *const spawn = g->place_critter_around( mon_mi_go_myrmidon, p, 1 );
    if( spawn_damaged ) {
        spawn->set_hp( spawn->get_hp_max() / 2 );
    }
    if( get_player_character().sees( p ) ) {
        add_msg( m_bad, _( "Something stirs and clambers out of the ruined mass of flesh and nerves!" ) );
    }
}

} // namespace map_funcs
