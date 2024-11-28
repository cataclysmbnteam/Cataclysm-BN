#include "world.h"

#include <sstream>
#include <cstring>

#include "debug.h"
#include "cata_utility.h"
#include "filesystem.h"
#include "json.h"
#include "output.h"
#include "worldfactory.h"

#define dbg(x) DebugLogFL((x),DC::Main)

world::world( WORLDINFO* info )
    : info( info )
{
    if( !assure_dir_exist( info->folder_path() ) ) {
        dbg( DL::Error ) << "Unable to create or open world directory for saving: " << info->folder_path();
    }

    // std::string db_path = path + "/world.db";
    // int ret;

    // if( SQLITE_OK != ( ret = sqlite3_initialize() ) ) {
    //     dbg( DL::Error ) << "Failed to initialize sqlite3 (Error " << ret << ")";
    //     throw std::runtime_error( "Failed to initialize sqlite3" );
    // }

    // if( SQLITE_OK != ( ret = sqlite3_open_v2( db_path.c_str(), &db,
    //                          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL ) ) ) {
    //     dbg( DL::Error ) << "Failed to open db" << db_path << " (Error " << ret << ")";
    //     throw std::runtime_error( "Failed to open db" );
    // }

    // auto sql = "CREATE TABLE IF NOT EXISTS files ("  \
    //            "path           TEXT PRIMARY KEY NOT NULL," \
    //            "parent         TEXT NOT NULL," \
    //            "compression    TEXT DEFAULT NULL," \
    //            "data           BLOB NOT NULL" \
    //            ");";


    // char *sqlErrMsg = 0;
    // if( SQLITE_OK != ( ret = sqlite3_exec( db, sql, NULL, NULL, &sqlErrMsg ) ) ) {
    //     dbg( DL::Error ) << "Failed to init db" << db_path << " (" << sqlErrMsg << ")";
    //     throw std::runtime_error( "Failed to open db" );
    // }
}

world::~world()
{
    // sqlite3_close( db );
}

bool world::file_exist( const std::string &path )
{
    return ::file_exist( info->folder_path() + "/" + path );
}

bool world::write_to_file( const std::string &path, file_write_cb &writer, const char *fail_message )
{
    return ::write_to_file( info->folder_path() + "/" + path, writer, fail_message );
}

bool world::read_from_file( const std::string &path, file_read_cb reader, bool optional )
{
    return ::read_from_file( info->folder_path() + "/" + path, reader, optional );
}

bool world::read_from_file_json( const std::string &path, file_read_json_cb reader, bool optional )
{
    return ::read_from_file_json( info->folder_path() + "/" + path, reader, optional );
}