#pragma once
#ifndef CATA_SRC_MESSAGES_H
#define CATA_SRC_MESSAGES_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "cached_options.h"
#include "string_formatter.h"
#include "enums.h"
#include "debug.h"

class JsonOut;
class JsonObject;
class translation;

namespace catacurses
{
class window;
} // namespace catacurses

namespace Messages
{
auto recent_messages( size_t count ) -> std::vector<std::pair<std::string, std::string>>;
void add_msg( std::string msg );
void add_msg( const game_message_params &params, std::string msg );
void clear_messages();
void deactivate();
auto size() -> size_t;
auto has_undisplayed_messages() -> bool;
void display_messages();
void display_messages( const catacurses::window &ipk_target, int left, int top, int right,
                       int bottom );
void serialize( JsonOut &json );
void deserialize( const JsonObject &json );
} // namespace Messages

void add_msg( std::string msg );
template<typename ...Args>
inline void add_msg( const std::string &msg, Args &&... args )
{
    return add_msg( string_format( msg, std::forward<Args>( args )... ) );
}
template<typename ...Args>
inline void add_msg( const char *const msg, Args &&... args )
{
    return add_msg( string_format( msg, std::forward<Args>( args )... ) );
}
template<typename ...Args>
inline void add_msg( const translation &msg, Args &&... args )
{
    return add_msg( string_format( msg, std::forward<Args>( args )... ) );
}

void add_msg( const game_message_params &params, std::string msg );
template<typename ...Args>
inline void add_msg( const game_message_params &params, const std::string &msg, Args &&... args )
{
    if( params.type == m_debug && !debug_mode ) {
        return;
    }
    return add_msg( params, string_format( msg, std::forward<Args>( args )... ) );
}
template<typename ...Args>
inline void add_msg( const game_message_params &params, const char *const msg, Args &&... args )
{
    if( params.type == m_debug && !debug_mode ) {
        return;
    }
    return add_msg( params, string_format( msg, std::forward<Args>( args )... ) );
}

#endif // CATA_SRC_MESSAGES_H
