#include "cached_options.h"

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
error_log_format_t error_log_format = error_log_format_t::human_readable;
