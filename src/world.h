#pragma once
#ifndef CATA_SRC_WORLDDB_H
#define CATA_SRC_WORLDDB_H

#include <functional>
#include <string>
#include "json.h"
#include "fstream_utils.h"

struct WORLDINFO;

class world
{
    public:
        world( WORLDINFO* info );
        ~world();

        WORLDINFO* info;

        bool file_exist( const std::string &path );

        bool write_to_file( const std::string &path, file_write_cb &writer, const char *fail_message = nullptr );
        bool read_from_file( const std::string &path, file_read_cb reader, bool optional = false );
        bool read_from_file_json( const std::string &path, file_read_json_cb reader, bool optional = false );
};

#endif // CATA_SRC_WORLDDB_H
