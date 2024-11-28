#pragma once
#ifndef CATA_SRC_WORLDDB_H
#define CATA_SRC_WORLDDB_H

#include <functional>
#include <string>
#include <sqlite3.h>
#include "json.h"

class worlddb
{
    public:
        worlddb( const std::string &path );
        ~worlddb();

        bool file_exist_in_db( const std::string &path );
        bool file_exist( const std::string &path );

        void write_to_file( const std::string &path,
                            const std::function<void( std::ostream & )> &writer );

        bool write_to_file( const std::string &path, const std::function<void( std::ostream & )> &writer,
                            const char *const fail_message );

        bool read_from_file( const std::string &path,
                             const std::function<void( std::istream & )> &reader );
        bool read_from_file_optional( const std::string &path,
                                      const std::function<void( std::istream & )> &reader );
        bool read_from_file_optional_json( const std::string &path,
                                           const std::function<void( JsonIn & )> &reader );
    private:
        sqlite3 *db;
        const std::string worlddir;
};

#endif // CATA_SRC_WORLDDB_H
