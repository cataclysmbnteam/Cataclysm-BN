#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "messages.h"
#include "enums.h"

class JsonObject;
class JsonOut;

namespace catacurses
{
class window;
}  // namespace catacurses

/**
 * Stubs to turn all Messages calls into no-ops for unit testing.
 */

std::vector<std::pair<std::string, std::string>> Messages::recent_messages( size_t )
{
    return std::vector<std::pair<std::string, std::string>>();
}
void Messages::add_msg( std::string ) {}
void Messages::add_msg( const game_message_params &, std::string ) {}
void Messages::clear_messages() {}
void Messages::deactivate() {}
size_t Messages::size()
{
    return 0;
}
bool Messages::has_undisplayed_messages()
{
    return false;
}
void Messages::display_messages() {}
void Messages::display_messages( const catacurses::window &, int, int, int, int ) {}
void Messages::serialize( JsonOut & ) {}
void Messages::deserialize( const JsonObject & ) {}

// Returns pairs of message log type id and untranslated name
const std::vector<std::pair<game_message_type, const char *>> &msg_type_and_names()
{
    static const std::vector<std::pair<game_message_type, const char *>> stub_type_n_names = {
        { m_good, "good" },
        { m_bad, "bad" },
        { m_mixed, "mixed" },
        { m_warning, "warning" },
        { m_info, "info" },
        { m_neutral, "neutral" },
        { m_debug, "debug" }
    };
    return stub_type_n_names;
}
void add_msg( std::string ) {}
void add_msg( const game_message_params &, std::string ) {}
