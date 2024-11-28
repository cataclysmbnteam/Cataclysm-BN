#include "world.h"

#include <sstream>
#include <cstring>

#include "debug.h"
#include "cata_utility.h"
#include "filesystem.h"
#include "json.h"
#include "output.h"
#include "fstream_utils.h"
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

void world::write_to_file( const std::string &path,
                             const std::function<void( std::ostream & )> &writer )
{
    ::write_to_file( info->folder_path() + "/" + path, writer );
}

bool world::write_to_file( const std::string &path,
                             const std::function<void( std::ostream & )> &writer,
                             const char *const fail_message )
{
    try {
        write_to_file( path, writer );
        return true;

    } catch( const std::exception &err ) {
        if( fail_message ) {
            popup( _( "Failed to write %1$s to \"%2$s\": %3$s" ), fail_message, path.c_str(), err.what() );
        }
        return false;
    }
}

bool world::read_from_file( const std::string &path,
                              const std::function<void( std::istream & )> &reader )
{
    return ::read_from_file( info->folder_path() + "/" + path, reader );
}

bool world::read_from_file_optional( const std::string &path,
                                       const std::function<void( std::istream & )> &reader )
{
    // Note: slight race condition here, but we'll ignore it. Worst case: the file
    // exists and got removed before reading it -> reading fails with a message
    // Or file does not exists, than everything works fine because it's optional anyway.
    return file_exist( path ) && read_from_file( path, reader );
}

bool world::read_from_file_optional_json( const std::string &path,
        const std::function<void( JsonIn & )> &reader )
{
    return read_from_file_optional( path, [&]( std::istream & fin ) {
        JsonIn jsin( fin, path );
        reader( jsin );
    } );
}
