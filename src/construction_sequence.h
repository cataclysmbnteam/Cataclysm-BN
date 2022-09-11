#pragma once
#ifndef CATA_SRC_CONSTRUCTION_SEQUENCE_H
#define CATA_SRC_CONSTRUCTION_SEQUENCE_H

#include <string>
#include <vector>

#include "type_id.h"

class JsonObject;

struct construction_sequence {
    void load( const JsonObject &jo, const std::string &src );
    void check() const;

    construction_sequence_id id;
    bool was_loaded = false;

    std::vector<construction_str_id> elems;

    ter_str_id post_terrain;
    furn_str_id post_furniture;

    bool blacklisted = false;
};

namespace construction_sequences
{
void load( const JsonObject &jo, const std::string &src );
void reset();
void finalize();
void check_consistency();

const construction_sequence *lookup_sequence( const ter_str_id &id );
const construction_sequence *lookup_sequence( const furn_str_id &id );

} // namespace construction_sequences

#endif // CATA_SRC_CONSTRUCTION_SEQUENCE_H
