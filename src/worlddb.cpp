#include "worlddb.h"

#include <sstream>
#include <cstring>

#include "debug.h"
#include "cata_utility.h"
#include "filesystem.h"
#include "json.h"
#include "output.h"
#include "fstream_utils.h"

#define dbg(x) DebugLogFL((x),DC::Main)

worlddb::worlddb( const std::string &path )
    : db( nullptr )
    , worlddir( path )
{
    // DLP: DEBUG
    std::cerr << "Opening Database" << std::endl;
    if( !assure_dir_exist( path ) ) {
        dbg( DL::Error ) << "Unable to create or open world directory for saving: " << path;
    }

    std::string db_path = path + "/world.db";
    int ret;

    if( SQLITE_OK != ( ret = sqlite3_initialize() ) ) {
        dbg( DL::Error ) << "Failed to initialize sqlite3 (Error " << ret << ")";
        throw std::runtime_error( "Failed to initialize sqlite3" );
    }

    if( SQLITE_OK != ( ret = sqlite3_open_v2( db_path.c_str(), &db,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL ) ) ) {
        dbg( DL::Error ) << "Failed to open db" << db_path << " (Error " << ret << ")";
        throw std::runtime_error( "Failed to open db" );
    }

    auto sql = "CREATE TABLE IF NOT EXISTS files ("  \
               "path           TEXT PRIMARY KEY NOT NULL," \
               "parent         TEXT NOT NULL," \
               "compression    TEXT DEFAULT NULL," \
               "data           BLOB NOT NULL" \
               ");";


    char *sqlErrMsg = 0;
    if( SQLITE_OK != ( ret = sqlite3_exec( db, sql, NULL, NULL, &sqlErrMsg ) ) ) {
        dbg( DL::Error ) << "Failed to init db" << db_path << " (" << sqlErrMsg << ")";
        throw std::runtime_error( "Failed to open db" );
    }
}

worlddb::~worlddb()
{
    // DLP: DEBUG
    std::cerr << "Closing Database" << std::endl;
    sqlite3_close( db );
}

bool worlddb::file_exist_in_db( const std::string &path )
{
    int fileCount = 0;
    const char *sql = "SELECT count() FROM files WHERE path = :path";
    sqlite3_stmt *stmt = nullptr;

    if( sqlite3_prepare_v2( db, sql, -1, &stmt, nullptr ) != SQLITE_OK ) {
        dbg( DL::Error ) << "Failed to prepare statement: " << sqlite3_errmsg( db ) << std::endl;
        throw std::runtime_error( "DB query failed" );
    }

    if( sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, ":path" ), path.c_str(), -1,
                           SQLITE_TRANSIENT ) != SQLITE_OK ) {
        dbg( DL::Error ) << "Failed to bind parameter: " << sqlite3_errmsg( db ) << std::endl;
        sqlite3_finalize( stmt );
        throw std::runtime_error( "DB query failed" );
    }

    if( sqlite3_step( stmt ) == SQLITE_ROW ) {
        // Retrieve the count result
        fileCount = sqlite3_column_int( stmt, 0 );
    } else {
        dbg( DL::Error ) << "Failed to execute query: " << sqlite3_errmsg( db ) << std::endl;
        sqlite3_finalize( stmt );
        throw std::runtime_error( "DB query failed" );
    }

    sqlite3_finalize( stmt );

    return fileCount > 0;
}

bool worlddb::file_exist( const std::string &path )
{
    // Fall back to old logic if the file was not found for backwards compatibility
    return file_exist_in_db( path ) || ::file_exist( worlddir + "/" + path );
}

void worlddb::write_to_file( const std::string &path,
                             const std::function<void( std::ostream & )> &writer )
{

    std::ostringstream oss;
    writer( oss );
    auto data = oss.str();

    size_t basePos = path.find_last_of( "/\\" );
    auto parent = ( basePos == std::string::npos ) ? "" : path.substr( 0, basePos );

    // DLP: DEBUG
    std::cerr << "SQL Write: " << path << " ..." << parent << " ...";

    // sqlite3_exec( db, "BEGIN TRANSACTION", 0, 0, 0 );
    const char *sql = "INSERT INTO files(path, parent, data) VALUES (:path, :parent, :data)";

    sqlite3_stmt *stmt = nullptr;

    if( sqlite3_prepare_v2( db, sql, -1, &stmt, nullptr ) != SQLITE_OK ) {
        dbg( DL::Error ) << "Failed to prepare statement: " << sqlite3_errmsg( db ) << std::endl;
        throw std::runtime_error( "DB query failed" );
    }

    if( sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, ":path" ), path.c_str(), -1,
                           SQLITE_TRANSIENT ) != SQLITE_OK ||
        sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, ":parent" ), parent.c_str(), -1,
                           SQLITE_TRANSIENT ) != SQLITE_OK ||
        sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, ":data" ), data.c_str(), -1,
                           SQLITE_TRANSIENT ) != SQLITE_OK ) {
        dbg( DL::Error ) << "Failed to bind parameters: " << sqlite3_errmsg( db ) << std::endl;
        sqlite3_finalize( stmt );
        throw std::runtime_error( "DB query failed" );
    }

    if( sqlite3_step( stmt ) != SQLITE_DONE ) {
        dbg( DL::Error ) << "Failed to execute query: " << sqlite3_errmsg( db ) << std::endl;
    }
    sqlite3_finalize( stmt );
    // sqlite3_exec( db, "COMMIT", 0, 0, 0 );

    // DLP: DEBUG
    std::cerr << " ... DONE" << std::endl;
}

bool worlddb::write_to_file( const std::string &path,
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

bool worlddb::read_from_file( const std::string &path,
                              const std::function<void( std::istream & )> &reader )
{
    if( file_exist_in_db( path ) ) {
        // DLP: DEBUG
        std::cerr << "SQL Read: " << path << std::endl;
        const char *sql = "SELECT data FROM files WHERE path = :path LIMIT 1";

        sqlite3_stmt *stmt = nullptr;

        if( sqlite3_prepare_v2( db, sql, -1, &stmt, nullptr ) != SQLITE_OK ) {
            dbg( DL::Error ) << "Failed to prepare statement: " << sqlite3_errmsg( db ) << std::endl;
            throw std::runtime_error( "DB query failed" );
        }

        if( sqlite3_bind_text( stmt, sqlite3_bind_parameter_index( stmt, ":path" ), path.c_str(), -1,
                               SQLITE_TRANSIENT ) != SQLITE_OK ) {
            dbg( DL::Error ) << "Failed to bind parameter: " << sqlite3_errmsg( db ) << std::endl;
            sqlite3_finalize( stmt );
            throw std::runtime_error( "DB query failed" );
        }

        if( sqlite3_step( stmt ) == SQLITE_ROW ) {
            // Retrieve the count result
            const void *blobData = sqlite3_column_blob( stmt, 0 );
            int blobSize = sqlite3_column_bytes( stmt, 0 );

            if( blobData == nullptr ) {
                return false; // Return an empty string if there's no data
            }

            std::string blobString( static_cast<const char *>( blobData ), blobSize );
            std::istringstream stream( blobString );
            reader( stream );
            sqlite3_finalize( stmt );
        } else {
            dbg( DL::Error ) << "Failed to execute query: " << sqlite3_errmsg( db ) << std::endl;
            sqlite3_finalize( stmt );
            throw std::runtime_error( "DB query failed" );
        }

        return true;
    } else {
        // DLP: DEBUG
        std::cerr << "Redirected ";
        return ::read_from_file( worlddir + "/" + path, reader );
    }
}

bool worlddb::read_from_file_optional( const std::string &path,
                                       const std::function<void( std::istream & )> &reader )
{
    // Note: slight race condition here, but we'll ignore it. Worst case: the file
    // exists and got removed before reading it -> reading fails with a message
    // Or file does not exists, than everything works fine because it's optional anyway.
    return file_exist( path ) && read_from_file( path, reader );
}

bool worlddb::read_from_file_optional_json( const std::string &path,
        const std::function<void( JsonIn & )> &reader )
{
    return read_from_file_optional( path, [&]( std::istream & fin ) {
        JsonIn jsin( fin, path );
        reader( jsin );
    } );
}
