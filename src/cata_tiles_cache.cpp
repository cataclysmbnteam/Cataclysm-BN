#pragma optimize("", off)
#if defined(TILES)
#include "cata_tiles_cache.h"

#include "bionics.h"
#include "cata_tiles_ui_element.h"
#include "cata_tiles.h"
#include "effect.h"
#include "field_type.h"
#include "item_factory.h"
#include "magic.h"
#include "mapdata.h"
#include "monstergenerator.h"
#include "mtype.h"
#include "mutation.h"
#include "omdata.h"
#include "sdltiles.h"
#include "trap.h"
#include "weather_type.h"
#include "veh_type.h"

tile_ptr cata_tiles_cache::find_tile(
    const std::string &tile_id
)
{
    return find_tile( tile_id, TILE_CATEGORY::C_NONE, "" );
}

tile_ptr cata_tiles_cache::find_tile(
    const std::string &tile_id,
    TILE_CATEGORY category
)
{
    return find_tile( tile_id, category, "" );
}

tile_ptr cata_tiles_cache::find_tile(
    const std::string &tile_id,
    TILE_CATEGORY category,
    const std::string &subcategory
)
{
    cata::optional<tile_lookup_res> opt = tilecontext->find_tile_looks_like( tile_id, category );

    tile_ptr tt = nullptr;

    if( opt ) {
        return &opt->tile();
    }

    const tileset &tset = *tilecontext->tileset_ptr;

    // if id is not found, try to find a tile for the category+subcategory combination
    const std::string &category_id = get_category_ids()[category];
    if( !category_id.empty() && !subcategory.empty() ) {
        tt = tset.find_tile_type( "unknown_" + category_id + "_" + subcategory );
    }

    // if at this point we have no tile, try just the category
    if( !tt ) {
        const std::string &category_id = get_category_ids()[category];
        if( !category_id.empty() ) {
            tt = tset.find_tile_type( "unknown_" + category_id );
        }
    }

    // if we still have no tile, we're out of luck, fall back to unknown
    if( !tt ) {
        tt = tset.find_tile_type( "unknown" );
    }

    if( !tt ) {
        // This really shouldn't happen, but the tileset creator might have forgotten to define an unknown tile
        return nullptr;
    }

    return tt;
}

void cata_tiles_cache::build_cache()
{
    ascii.reserve( 256 * 16 );
    for (int col = 0; col < 16; col++) {
        for (int ch = 0; ch < 256; ch++) {
            int idx = col * 256 + ch;
            std::string id = get_ascii_tile_id( ch, col, -1 );
        }
    }

    ui_elems.resize( tiles_ui_element::get_all().size(), nullptr );
    for( const tiles_ui_element &it : tiles_ui_element::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( ui_elems.size() ) );
        const tile_type *tt = find_tile( it.id.str() );
        if( tt ) {
            ui_elems[idx] = tt;
        }
    }

    const std::vector<field_type> &all_fields = field_types::get_all();
    fld.resize( all_fields.size(), nullptr );
    for( const field_type &it : all_fields ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( fld.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_FIELD );
        if( tt ) {
            fld[idx] = tt;
        }
    }

    trp.resize( trap::get_all().size(), nullptr );
    for( const trap &it : trap::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( trp.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_TRAP );
        if( tt ) {
            trp[idx] = tt;
        }
    }

    ter.resize( ter_t::get_all().size(), nullptr );
    for( const ter_t &it : ter_t::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( ter.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_TERRAIN );
        if( tt ) {
            ter[idx] = tt;
        }
    }

    furn.resize( furn_t::get_all().size(), nullptr );
    for( const furn_t &it : furn_t::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( furn.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_FURNITURE );
        if( tt ) {
            furn[idx] = tt;
        }
    }

    const std::vector<mtype> &all_mtypes = MonsterGenerator::generator().get_all_mtypes();
    mon.resize( all_mtypes.size(), nullptr );
    mon_corpse.resize( all_mtypes.size(), nullptr );
    mon_rid.resize( all_mtypes.size(), nullptr );
    for( const mtype &it : all_mtypes ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( all_mtypes.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_MONSTER );
        if( tt ) {
            mon[idx] = tt;
        }
        tt = find_tile( "corpse_" + it.id.str(), TILE_CATEGORY::C_ITEM, "corpse" );
        if( tt ) {
            mon_corpse[idx] = tt;
        }
        tt = find_tile( "rid_" + it.id.str(), TILE_CATEGORY::C_MONSTER );
        if( tt ) {
            mon_rid[idx] = tt;
        }
    }

    const std::vector<const itype *> all_itypes = item_controller->all();
    itm.resize( all_itypes.size(), nullptr );
    itm_worn.resize( all_itypes.size(), nullptr );
    itm_wielded.resize( all_itypes.size(), nullptr );
    for( const itype *e : all_itypes ) {
        const itype_id &id = e->get_id();
        int idx = id.id().to_i();
        assert( idx < static_cast<int>( all_itypes.size() ) );
        const std::string it_category = e->get_item_type_string();
        const tile_type *tt = find_tile( id.str(), TILE_CATEGORY::C_ITEM, it_category );
        if( tt ) {
            itm[idx] = tt;
        }
        tt = find_tile( "overlay_worn_" + id.str(), TILE_CATEGORY::C_ITEM, it_category );
        if( tt ) {
            itm_worn[idx] = tt;
        }
        tt = find_tile( "overlay_wielded_" + id.str(), TILE_CATEGORY::C_ITEM, it_category );
        if( tt ) {
            itm_wielded[idx] = tt;
        }
    }

    om_ter.resize( overmap_terrains::get_all().size(), nullptr );
    for( const oter_t &it : overmap_terrains::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( om_ter.size() ) );
        const tile_type *tt = find_tile( it.id.str(), TILE_CATEGORY::C_OVERMAP_TERRAIN );
        if( tt ) {
            om_ter[idx] = tt;
        }
    }

    trait_passive_m.resize( mutation_branch::get_all().size() );
    trait_passive_f.resize( mutation_branch::get_all().size() );
    trait_active_m.resize( mutation_branch::get_all().size() );
    trait_active_f.resize( mutation_branch::get_all().size() );
    for( const mutation_branch &it : mutation_branch::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( trait_passive_m.size() ) );
        const tile_type *tt = find_tile( "overlay_male_mutation_" + it.id.str() );
        if( tt ) {
            trait_passive_m[idx] = tt;
        }
        tt = find_tile( "overlay_female_mutation_" + it.id.str() );
        if( tt ) {
            trait_passive_f[idx] = tt;
        }
        tt = find_tile( "overlay_male_mutation_" + it.id.str() + "_active" );
        if( tt ) {
            trait_active_m[idx] = tt;
        }
        tt = find_tile( "overlay_female_mutation_" + it.id.str() + "_active" );
        if( tt ) {
            trait_active_f[idx] = tt;
        }
    }

    bionic_passive_m.resize( bionic_data::get_all().size() );
    bionic_passive_f.resize( bionic_data::get_all().size() );
    bionic_active_m.resize( bionic_data::get_all().size() );
    bionic_active_f.resize( bionic_data::get_all().size() );
    for( const bionic_data &it : bionic_data::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( bionic_active_m.size() ) );
        const tile_type *tt = find_tile( "overlay_male_mutation_" + it.id.str() );
        if( tt ) {
            bionic_passive_m[idx] = tt;
        }
        tt = find_tile( "overlay_female_mutation_" + it.id.str() );
        if( tt ) {
            bionic_passive_f[idx] = tt;
        }
        tt = find_tile( "overlay_male_mutation_" + it.id.str() + "_active" );
        if( tt ) {
            bionic_active_m[idx] = tt;
        }
        tt = find_tile( "overlay_female_mutation_" + it.id.str() + "_active" );
        if( tt ) {
            bionic_active_f[idx] = tt;
        }
    }

    effect_overlay.resize( get_all_effect_types().size() );
    for( const auto &it : get_all_effect_types() ) {
        int idx = it.first.id().to_i();
        assert( idx < static_cast<int>( effect_overlay.size() ) );
        const tile_type *tt = find_tile( "overlay_effect_" + it.first.str() );
        if( tt ) {
            effect_overlay[idx] = tt;
        } else {
            std::string s = "abc";
        }
    }

    weather_om.resize( weather_types::get_all().size(), nullptr );
    weather_ter.resize( weather_types::get_all().size(), nullptr );
    for( const weather_type &it : weather_types::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( weather_om.size() ) );
        const tile_type *tt = find_tile( it.id.str() );
        if( tt ) {
            weather_om[idx] = tt;
        }
        tt = find_tile( "weather_ter_" + it.id.str() );
        if( tt ) {
            weather_ter[idx] = tt;
        }
    }

    spell_aoe.resize( spell_type::get_all().size(), nullptr );
    for( const spell_type &it : spell_type::get_all() ) {
        int idx = it.id.id().to_i();
        assert( idx < static_cast<int>( spell_aoe.size() ) );
        const tile_type *tt = find_tile( it.id.str() );
        if( tt ) {
            spell_aoe[idx] = tt;
        }
    }

    // TODO: other sizes?
    // TODO: horde fallbacks?
    constexpr int OM_HORDES_NUM = 100;
    om_hordes.resize( OM_HORDES_NUM );
    for( int idx = 0; idx < OM_HORDES_NUM; idx++ ) {
        std::string id = string_format( "overmap_horde_%d", idx );
        const tile_type *tt = find_tile( id );
        if( tt ) {
            om_hordes[idx] = tt;
        }
    }

    vp.resize( vpart_info::all().size(), nullptr );
    vp_open.resize( vpart_info::all().size(), nullptr );
    vp_broken.resize( vpart_info::all().size(), nullptr );
    for( const auto &it : vpart_info::all() ) {
        int idx = it.first.id().to_i();
        assert( idx < static_cast<int>( vp.size() ) );
        const tile_type *tt = find_tile( it.first.str(), TILE_CATEGORY::C_VEHICLE_PART );
        if( tt ) {
            vp[idx] = tt;
        }
        tt = find_tile( it.first.str() + "_open", TILE_CATEGORY::C_VEHICLE_PART );
        if( tt ) {
            vp_open[idx] = tt;
        }
        tt = find_tile( it.first.str() + "_broken", TILE_CATEGORY::C_VEHICLE_PART );
        if( tt ) {
            vp_broken[idx] = tt;
        }
    }
}

#endif // TILES
