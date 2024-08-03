#include "cached_options.h"

#include "options.h"

bool test_mode = false;
bool debug_mode = false;
bool json_report_strict = true;
bool use_tiles = false;
bool use_tiles_overmap = false;
bool log_from_top;
int message_ttl;
int message_cooldown;
bool display_mod_source;
bool display_object_ids;
bool trigdist;
bool fov_3d;
bool static_z_effect = false;
int fov_3d_z_range;
bool tile_iso;
bool pixel_minimap_option = false;
int PICKUP_RANGE;


void FungalOptions::init()
{
    young_allowed = ::get_option<bool>( "MON_FUNGALOID_YOUNG_ALLOWED" );
    spread_on_flat_tiles_allowed = ::get_option<bool>( "FUNGUS_SPREAD_ON_FLAT_TILES_ALLOWED" );
    young_spawn_base_rate = ::get_option<int>( "MON_FUNGALOID_YOUNG_SPAWN_BASE_RATE" );
    young_spawn_bubble_creatures_divider
        = ::get_option<int>( "MON_FUNGALOID_YOUNG_SPAWN_BUBBLE_CREATURES_DIVIDER" );
    spore_chance = ::get_option<float>( "FUNGUS_SPORE_CHANCE" );
    advanced_creatures_threshold = ::get_option<int>( "FUNGUS_ADVANCED_CREATURES_THRESHOLD" );
    spore_creatures_threshold = ::get_option<int>( "FUNGUS_SPORE_CREATURES_THRESHOLD" );
}

FungalOptions fungal_opt;

error_log_format_t error_log_format = error_log_format_t::human_readable;
