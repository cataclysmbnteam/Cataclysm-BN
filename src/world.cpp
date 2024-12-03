#include "world.h"

#include <sstream>
#include <cstring>
#include <chrono>

#include "game.h"
#include "avatar.h"
#include "debug.h"
#include "cata_utility.h"
#include "filesystem.h"
#include "output.h"
#include "worldfactory.h"
#include "mod_manager.h"
#include "path_info.h"

#define dbg(x) DebugLogFL((x),DC::Main)

save_t::save_t( const std::string &name ): name( name ) {}

std::string save_t::decoded_name() const
{
    return name;
}

std::string save_t::base_path() const
{
    return base64_encode( name );
}

save_t save_t::from_save_id( const std::string &save_id )
{
    return save_t( save_id );
}

save_t save_t::from_base_path( const std::string &base_path )
{
    return save_t( base64_decode( base_path ) );
}

std::string WORLDINFO::folder_path() const
{
    return PATH_INFO::savedir() + world_name;
}

WORLDINFO::WORLDINFO()
{
    world_name = world_generator->get_next_valid_worldname();
    WORLD_OPTIONS = get_options().get_world_defaults();
    world_save_format = save_format::V1;

    world_saves.clear();
    active_mod_order = world_generator->get_mod_manager().get_default_mods();
}

void WORLDINFO::COPY_WORLD( const WORLDINFO *world_to_copy )
{
    world_name = world_to_copy->world_name + "_copy";
    WORLD_OPTIONS = world_to_copy->WORLD_OPTIONS;
    world_save_format = world_to_copy->world_save_format;
    active_mod_order = world_to_copy->active_mod_order;
}

bool WORLDINFO::needs_lua() const
{
    for( const mod_id &mod : active_mod_order ) {
        if( mod.is_valid() && mod->lua_api_version ) {
            return true;
        }
    }
    return false;
}

bool WORLDINFO::save_exists( const save_t &name ) const
{
    return std::find( world_saves.begin(), world_saves.end(), name ) != world_saves.end();
}

void WORLDINFO::add_save( const save_t &name )
{
    if( !save_exists( name ) ) {
        world_saves.push_back( name );
    }
}

void WORLDINFO::load_options( JsonIn &jsin )
{
    auto &opts = get_options();

    jsin.start_array();
    while( !jsin.end_array() ) {
        JsonObject jo = jsin.get_object();
        jo.allow_omitted_members();
        const std::string name = opts.migrateOptionName( jo.get_string( "name" ) );
        const std::string value = opts.migrateOptionValue( jo.get_string( "name" ),
                                  jo.get_string( "value" ) );

        if( opts.has_option( name ) && opts.get_option( name ).getPage() == "world_default" ) {
            WORLD_OPTIONS[ name ].setValue( value );
        }
    }
}

void WORLDINFO::load_legacy_options( std::istream &fin )
{
    auto &opts = get_options();

    //load legacy txt
    std::string sLine;
    while( !fin.eof() ) {
        getline( fin, sLine );
        if( !sLine.empty() && sLine[0] != '#' && std::count( sLine.begin(), sLine.end(), ' ' ) == 1 ) {
            size_t ipos = sLine.find( ' ' );
            // make sure that the option being loaded is part of the world_default page in OPTIONS
            // In 0.C some lines consisted of a space and nothing else
            const std::string name = opts.migrateOptionName( sLine.substr( 0, ipos ) );
            const std::string value = opts.migrateOptionValue( sLine.substr( 0, ipos ), sLine.substr( ipos + 1,
                                      sLine.length() ) );

            if( ipos != 0 && opts.get_option( name ).getPage() == "world_default" ) {
                WORLD_OPTIONS[name].setValue( value );
            }
        }
    }
}

bool WORLDINFO::load_options()
{
    WORLD_OPTIONS = get_options().get_world_defaults();

    using namespace std::placeholders;
    const auto path = folder_path() + "/" + PATH_INFO::worldoptions();
    return read_from_file_json( path, [&]( JsonIn & jsin ) {
        load_options( jsin );
    }, true );
}

bool WORLDINFO::save( const bool is_conversion ) const
{
    if( !assure_dir_exist( folder_path() ) ) {
        DebugLog( DL::Error, DC::Main ) << "Unable to create or open world[" << world_name
                                        << "] directory for saving";
        return false;
    }

    if( !is_conversion ) {
        const auto savefile = folder_path() + "/" + PATH_INFO::worldoptions();
        const bool saved = write_to_file( savefile, [&]( std::ostream & fout ) {
            JsonOut jout( fout );

            jout.start_array();

            for( auto &elem : WORLD_OPTIONS ) {
                // Skip hidden option because it is set by mod and should not be saved
                if( !elem.second.getDefaultText().empty() ) {
                    jout.start_object();

                    jout.member( "info", elem.second.getTooltip() );
                    jout.member( "default", elem.second.getDefaultText( false ) );
                    jout.member( "name", elem.first );
                    jout.member( "value", elem.second.getValue( true ) );

                    jout.end_object();
                }
            }

            jout.end_array();
        }, _( "world data" ) );
        if( !saved ) {
            return false;
        }
    }

    world_generator->get_mod_manager().save_mods_list( const_cast<WORLDINFO *>( this ) );
    return true;
}

void load_world_option( const JsonObject &jo )
{
    auto arr = jo.get_array( "options" );
    if( arr.empty() ) {
        jo.throw_error( "no options specified", "options" );
    }
    for( const std::string line : arr ) {
        get_options().get_option( line ).setValue( "true" );
    }
}

//load external option from json
void load_external_option( const JsonObject &jo )
{
    auto name = jo.get_string( "name" );
    auto stype = jo.get_string( "stype" );
    options_manager &opts = get_options();
    if( !opts.has_option( name ) ) {
        auto sinfo = jo.get_string( "info" );
        opts.add_external( name, "external_options", stype, sinfo, sinfo );
    }
    options_manager::cOpt &opt = opts.get_option( name );
    if( stype == "float" ) {
        opt.setValue( static_cast<float>( jo.get_float( "value" ) ) );
    } else if( stype == "int" ) {
        opt.setValue( jo.get_int( "value" ) );
    } else if( stype == "bool" ) {
        if( jo.get_bool( "value" ) ) {
            opt.setValue( "true" );
        } else {
            opt.setValue( "false" );
        }
    } else if( stype == "string" ) {
        opt.setValue( jo.get_string( "value" ) );
    } else {
        jo.throw_error( "Unknown or unsupported stype for external option", "stype" );
    }
    // Just visit this member if it exists
    if( jo.has_member( "info" ) ) {
        jo.get_string( "info" );
    }
}

world::world( WORLDINFO *info )
    : info( info )
    , save_tx_start_ts( 0 )
{
    if( !assure_dir_exist( "" )
        || !assure_dir_exist( "/maps" ) ) {
        dbg( DL::Error ) << "Unable to create or open world directory structure: " << info->folder_path();
    }
}

world::~world()
{
}

void world::start_save_tx()
{
    if( save_tx_start_ts != 0 ) {
        throw std::runtime_error( "Attempted to start a save transaction while one was already in progress" );
    }
    save_tx_start_ts = std::chrono::duration_cast< std::chrono::milliseconds >(
                           std::chrono::system_clock::now().time_since_epoch()
                       ).count();
}

int64_t world::commit_save_tx()
{
    if( save_tx_start_ts == 0 ) {
        throw std::runtime_error( "Attempted to commit a save transaction while none was in progress" );
    }
    int64_t now = std::chrono::duration_cast< std::chrono::milliseconds >(
                      std::chrono::system_clock::now().time_since_epoch()
                  ).count();
    int64_t duration = now - save_tx_start_ts;
    save_tx_start_ts = 0;
    return duration;
}

/**
 * DOMAIN SPECIFIC: MAP
 */

static std::string get_quad_dirname( const tripoint &om_addr )
{
    const tripoint segment_addr = omt_to_seg_copy( om_addr );
    return string_format( "maps/%d.%d.%d", segment_addr.x, segment_addr.y, segment_addr.z );
}

static std::string get_quad_filename( const tripoint &om_addr )
{
    return string_format( "%d.%d.%d.map", om_addr.x, om_addr.y, om_addr.z );
}

bool world::read_map_quad( const tripoint &om_addr, file_read_json_fn reader ) const
{
    const std::string dirname = get_quad_dirname( om_addr );
    std::string quad_path = dirname + "/" + get_quad_filename( om_addr );

    if( !file_exist( quad_path ) ) {
        // Fix for old saves where the path was generated using std::stringstream, which
        // did format the number using the current locale. That formatting may insert
        // thousands separators, so the resulting path is "map/1,234.7.8.map" instead
        // of "map/1234.7.8.map".
        std::ostringstream buffer;
        buffer << dirname << "/" << om_addr.x << "." << om_addr.y << "." << om_addr.z << ".map";
        if( file_exist( buffer.str() ) ) {
            quad_path = buffer.str();
        }
    }

    return read_from_file_json( quad_path, reader, true );
}

bool world::write_map_quad( const tripoint &om_addr, file_write_fn writer ) const
{
    const std::string dirname = get_quad_dirname( om_addr );
    std::string quad_path = dirname + "/" + get_quad_filename( om_addr );

    assure_dir_exist( dirname );
    return write_to_file( quad_path, writer );
}

/**
 * DOMAIN SPECIFIC: OVERMAP
 */

std::string world::overmap_terrain_filename( const point_abs_om &p ) const
{
    return string_format( "o.%d.%d", p.x(), p.y() );
}

std::string world::overmap_player_filename( const point_abs_om &p ) const
{
    return string_format( ".seen.%d.%d", p.x(), p.y() );
}

bool world::overmap_exists( const point_abs_om &p ) const
{
    return file_exist( overmap_terrain_filename( p ) );
}

bool world::read_overmap( const point_abs_om &p, file_read_fn reader ) const
{
    return read_from_file( overmap_terrain_filename( p ), reader, true );
}

bool world::read_overmap_player_visibility( const point_abs_om &p, file_read_fn reader )
{
    return read_from_player_file( overmap_player_filename( p ), reader, true );
}

bool world::write_overmap( const point_abs_om &p, file_write_fn writer ) const
{
    return write_to_file( overmap_terrain_filename( p ), writer );
}

bool world::write_overmap_player_visibility( const point_abs_om &p, file_write_fn writer )
{
    return write_to_player_file( overmap_player_filename( p ), writer );
}

/**
 * DOMAIN SPECIFIC: MAP MEMORY
 */
static std::string get_mm_filename( const tripoint &p )
{
    return string_format( "%d.%d.%d.mmr", p.x, p.y, p.z );
}

bool world::read_player_mm_quad( const tripoint &p, file_read_json_fn reader )
{
    return read_from_player_file_json( ".mm1/" + get_mm_filename( p ), reader, true );
}

bool world::write_player_mm_quad( const tripoint &p, file_write_fn writer )
{
    const std::string descr = string_format(
                                  _( "memory map region for (%d,%d,%d)" ),
                                  p.x, p.y, p.z
                              );
    assure_dir_exist( get_player_path() + ".mm1" );
    return write_to_player_file( ".mm1/" + get_mm_filename( p ), writer, descr.c_str() );
}

/**
 * PLAYER OPERATIONS
 */

std::string world::get_player_path() const
{
    return base64_encode( g->u.get_save_id() );
}

bool world::player_file_exist( const std::string &path )
{
    return file_exist( get_player_path() + path );
}

bool world::write_to_player_file( const std::string &path, file_write_fn writer,
                                  const char *fail_message )
{
    return write_to_file( get_player_path() + path, writer, fail_message );
}

bool world::read_from_player_file( const std::string &path, file_read_fn reader,
                                   bool optional )
{
    return read_from_file( get_player_path() + path, reader, optional );
}

bool world::read_from_player_file_json( const std::string &path, file_read_json_fn reader,
                                        bool optional )
{
    return read_from_file_json( get_player_path() + path, reader, optional );
}

/**
 * GENERIC OPERATIONS
 */

bool world::assure_dir_exist( const std::string &path ) const
{
    return ::assure_dir_exist( info->folder_path() + "/" + path );
}

bool world::file_exist( const std::string &path ) const
{
    return ::file_exist( info->folder_path() + "/" + path );
}

bool world::write_to_file( const std::string &path, file_write_fn writer,
                           const char *fail_message ) const
{
    return ::write_to_file( info->folder_path() + "/" + path, writer, fail_message );
}

bool world::read_from_file( const std::string &path, file_read_fn reader,
                            bool optional ) const
{
    return ::read_from_file( info->folder_path() + "/" + path, reader, optional );
}

bool world::read_from_file_json( const std::string &path, file_read_json_fn reader,
                                 bool optional ) const
{
    return ::read_from_file_json( info->folder_path() + "/" + path, reader, optional );
}