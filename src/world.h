#pragma once
#ifndef CATA_SRC_WORLDDB_H
#define CATA_SRC_WORLDDB_H

#include <functional>
#include <string>
#include "json.h"

struct WORLDINFO;

class world
{
    public:
        world( WORLDINFO* info );
        ~world();

        WORLDINFO* info;

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
        const std::string worlddir;
};

#endif // CATA_SRC_WORLDDB_H
