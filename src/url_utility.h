#pragma once
#ifndef CATA_SRC_URL_UTILITY_H
#define CATA_SRC_URL_UTILITY_H

#include <string>

auto open_url( const std::string &url ) -> void;
auto encode_url( const std::string &text ) -> std::string;

#endif
