#include "options.h"
#include "options_category.h"

#include <locale>
#include <cfloat>
#include <climits>
#include <iterator>
#include <stdexcept>

#include "calendar.h"
#include "cata_utility.h"
#include "catacharset.h"
#include "color.h"
#include "cursesdef.h"
#include "cursesport.h"
#include "debug.h"
#include "enum_bitset.h"
#include "filesystem.h"
#include "fstream_utils.h"
#include "game.h"
#include "game_constants.h"
#include "input.h"
#include "json.h"
#include "language.h"
#include "line.h"
#include "mapsharing.h"
#include "output.h"
#include "path_info.h"
#include "point.h"
#include "popup.h"
#include "sdlsound.h"
#include "sdltiles.h"
#include "sounds.h"
#include "string_formatter.h"
#include "string_input_popup.h"
#include "string_utils.h"
#include "translations.h"
#include "ui_manager.h"
#include "worldfactory.h"

#if defined(TILES)
#include "cata_tiles.h"
#endif // TILES

#if defined(__ANDROID__)
#include <jni.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <memory>
#include <sstream>
#include <string>

#include "options_debug_level.h"

extern const std::vector<debug_log_level> debug_log_levels;
extern const std::vector<debug_log_class> debug_log_classes;

std::map<std::string, std::string> TILESETS; // All found tilesets: <name, tileset_dir>
std::map<std::string, std::string> SOUNDPACKS; // All found soundpacks: <name, soundpack_dir>

options_manager &get_options()
{
    static options_manager single_instance;
    return single_instance;
}


options_manager::options_manager()
{
    pages_.emplace_back( general, to_translation( "General" ) );
    pages_.emplace_back( interface, to_translation( "Interface" ) );
    pages_.emplace_back( graphics, to_translation( "Graphics" ) );
    // when sharing maps only admin is allowed to change these.
    if( !MAP_SHARING::isCompetitive() || MAP_SHARING::isAdmin() ) {
        pages_.emplace_back( world_default, to_translation( "World Defaults" ) );
        pages_.emplace_back( "debug", to_translation( "Debug" ) );
    }
#if defined(__ANDROID__)
    pages_.emplace_back( android, to_translation( "Android" ) );
#endif

    mMigrateOption = { {"DELETE_WORLD", { "WORLD_END", { {"no", "keep" }, {"yes", "delete"} } } } };

    enable_json( "DEFAULT_REGION" );
    // to allow class based init_data functions to add values to a 'string' type option, add:
    //   enable_json("OPTION_KEY_THAT_GETS_STRING_ENTRIES_ADDED_VIA_JSON");
    // then, in the my_class::load_json (or post-json setup) method:
    //   get_options().add_value("OPTION_KEY_THAT_GETS_STRING_ENTRIES_ADDED_VIA_JSON", "thisvalue");
}

static const std::string blank_value( 1, 001 ); // because "" might be valid

void options_manager::enable_json( const std::string &lvar )
{
    post_json_verify[ lvar ] = blank_value;
}

void options_manager::add_retry( const std::string &lvar, const::std::string &lval )
{
    std::map<std::string, std::string>::const_iterator it = post_json_verify.find( lvar );
    if( it != post_json_verify.end() && it->second == blank_value ) {
        // initialized with impossible value: valid
        post_json_verify[ lvar ] = lval;
    }
}

void options_manager::add_value( const std::string &lvar, const std::string &lval,
                                 const translation &lvalname )
{
    std::map<std::string, std::string>::const_iterator it = post_json_verify.find( lvar );
    if( it != post_json_verify.end() ) {
        auto ot = options.find( lvar );
        if( ot != options.end() && ot->second.sType == "string_select" ) {
            for( auto &vItem : ot->second.vItems ) {
                if( vItem.first == lval ) { // already in
                    return;
                }
            }
            ot->second.vItems.emplace_back( lval, lvalname );
            // our value was saved, then set to default, so set it again.
            if( it->second == lval ) {
                options[ lvar ].setValue( lval );
            }
        }

    }
}

void options_manager::addOptionToPage( const std::string &name, const std::string &page )
{
    for( Page &p : pages_ ) {
        if( p.id_ == page ) {
            // Don't add duplicate options to the page
            for( const PageItem &i : p.items_ ) {
                if( i.type == ItemType::Option && i.data == name ) {
                    return;
                }
            }
            p.items_.emplace_back( ItemType::Option, name, adding_to_group_ );
            return;
        }
    }
    // @TODO handle the case when an option has no valid page id (note: consider hidden external options as well)
}

options_manager::cOpt::cOpt()
{
    sType = "VOID";
    eType = CVT_VOID;
    hide = COPT_NO_HIDE;
}

static options_manager::cOpt::COPT_VALUE_TYPE get_value_type( const std::string &sType )
{
    using CVT = options_manager::cOpt::COPT_VALUE_TYPE;

    static std::unordered_map<std::string, CVT> vt_map = {
        { "float", CVT::CVT_FLOAT },
        { "bool", CVT::CVT_BOOL },
        { "int", CVT::CVT_INT },
        { "int_map", CVT::CVT_INT },
        { "string_select", CVT::CVT_STRING },
        { "string_input", CVT::CVT_STRING },
        { "VOID", CVT::CVT_VOID }
    };
    auto result = vt_map.find( sType );
    return result != vt_map.end() ? result->second : options_manager::cOpt::CVT_UNKNOWN;
}

//add hidden external option with value
void options_manager::add_external( const std::string &sNameIn, const std::string &sPageIn,
                                    const std::string &sType,
                                    const std::string &sMenuTextIn, const std::string &sTooltipIn )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = sType;
    thisOpt.verbose = false;

    thisOpt.eType = get_value_type( thisOpt.sType );

    switch( thisOpt.eType ) {
        case cOpt::CVT_BOOL:
            thisOpt.bSet = false;
            thisOpt.bDefault = false;
            break;
        case cOpt::CVT_INT:
            thisOpt.iMin = INT_MIN;
            thisOpt.iMax = INT_MAX;
            thisOpt.iDefault = 0;
            thisOpt.iSet = 0;
            break;
        case cOpt::CVT_FLOAT:
            thisOpt.fMin = FLT_MIN;
            thisOpt.fMax = FLT_MAX;
            thisOpt.fDefault = 0;
            thisOpt.fSet = 0;
            thisOpt.fStep = 1;
            break;
        default:
            // all other type-specific values have default constructors
            break;
    }

    thisOpt.hide = COPT_ALWAYS_HIDE;
    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add string select option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const std::vector<id_and_option> &sItemsIn, std::string sDefaultIn,
                           copt_hide_t opt_hide )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "string_select";
    thisOpt.eType = get_value_type( thisOpt.sType );

    thisOpt.hide = opt_hide;
    thisOpt.vItems = sItemsIn;

    if( thisOpt.getItemPos( sDefaultIn ) == -1 ) {
        sDefaultIn = thisOpt.vItems[0].first;
    }

    thisOpt.sDefault = sDefaultIn;
    thisOpt.sSet = sDefaultIn;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add string input option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const std::string &sDefaultIn, const int iMaxLengthIn,
                           copt_hide_t opt_hide )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "string_input";
    thisOpt.eType = get_value_type( thisOpt.sType );

    thisOpt.hide = opt_hide;

    thisOpt.iMaxLength = iMaxLengthIn;
    thisOpt.sDefault = thisOpt.iMaxLength > 0 ? sDefaultIn.substr( 0, thisOpt.iMaxLength ) : sDefaultIn;
    thisOpt.sSet = thisOpt.sDefault;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add bool option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const bool bDefaultIn, copt_hide_t opt_hide )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "bool";
    thisOpt.eType = get_value_type( thisOpt.sType );

    thisOpt.hide = opt_hide;

    thisOpt.bDefault = bDefaultIn;
    thisOpt.bSet = bDefaultIn;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add int option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const int iMinIn, int iMaxIn, int iDefaultIn,
                           copt_hide_t opt_hide, const std::string &format )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "int";
    thisOpt.eType = get_value_type( thisOpt.sType );

    thisOpt.format = format;

    thisOpt.hide = opt_hide;

    if( iMinIn > iMaxIn ) {
        iMaxIn = iMinIn;
    }

    thisOpt.iMin = iMinIn;
    thisOpt.iMax = iMaxIn;

    if( iDefaultIn < iMinIn || iDefaultIn > iMaxIn ) {
        iDefaultIn = iMinIn;
    }

    thisOpt.iDefault = iDefaultIn;
    thisOpt.iSet = iDefaultIn;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add int map option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const std::vector< std::tuple<int, std::string> > &mIntValuesIn,
                           int iInitialIn, int iDefaultIn, copt_hide_t opt_hide, const bool verbose )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "int_map";
    thisOpt.eType = get_value_type( thisOpt.sType );
    thisOpt.verbose = verbose;

    thisOpt.format = "%i";

    thisOpt.hide = opt_hide;

    thisOpt.mIntValues = mIntValuesIn;

    auto item = thisOpt.findInt( iInitialIn );
    if( !item ) {
        iInitialIn = std::get<0>( mIntValuesIn[0] );
    }

    item = thisOpt.findInt( iDefaultIn );
    if( !item ) {
        iDefaultIn = std::get<0>( mIntValuesIn[0] );
    }

    thisOpt.iDefault = iDefaultIn;
    thisOpt.iSet = iInitialIn;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

//add float option
void options_manager::add( const std::string &sNameIn, const std::string &sPageIn,
                           const std::string &sMenuTextIn, const std::string &sTooltipIn,
                           const float fMinIn, float fMaxIn, float fDefaultIn,
                           float fStepIn, copt_hide_t opt_hide, const std::string &format )
{
    cOpt thisOpt;

    thisOpt.sName = sNameIn;
    thisOpt.sPage = sPageIn;
    thisOpt.sMenuText = sMenuTextIn;
    thisOpt.sTooltip = sTooltipIn;
    thisOpt.sType = "float";
    thisOpt.eType = get_value_type( thisOpt.sType );

    thisOpt.format = format;

    thisOpt.hide = opt_hide;

    if( fMinIn > fMaxIn ) {
        fMaxIn = fMinIn;
    }

    thisOpt.fMin = fMinIn;
    thisOpt.fMax = fMaxIn;
    thisOpt.fStep = fStepIn;

    if( fDefaultIn < fMinIn || fDefaultIn > fMaxIn ) {
        fDefaultIn = fMinIn;
    }

    thisOpt.fDefault = fDefaultIn;
    thisOpt.fSet = fDefaultIn;

    addOptionToPage( sNameIn, sPageIn );

    options[sNameIn] = thisOpt;
}

void options_manager::add_empty_line( const std::string &sPageIn )
{
    for( Page &p : pages_ ) {
        if( p.id_ == sPageIn ) {
            p.items_.emplace_back( ItemType::BlankLine, "", adding_to_group_ );
            break;
        }
    }
}

void options_manager::add_option_group( const std::string &page_id,
                                        const options_manager::Group &group,
                                        std::function<void( const std::string & )> entries )
{
    if( !adding_to_group_.empty() ) {
        // Nested groups are not allowed
        debugmsg( "Tried to create option group '%s' from within group '%s'.",
                  group.id_, adding_to_group_ );
        return;
    }
    for( Group &g : groups_ ) {
        if( g.id_ == group.id_ ) {
            debugmsg( "Option group with id '%s' already exists", group.id_ );
            return;
        }
    }
    groups_.push_back( group );
    adding_to_group_ = groups_.back().id_;

    for( Page &p : pages_ ) {
        if( p.id_ == page_id ) {
            p.items_.emplace_back( ItemType::GroupHeader, group.id_, adding_to_group_ );
            break;
        }
    }

    entries( page_id );

    adding_to_group_.clear();
}

const options_manager::Group &options_manager::find_group( const std::string &id ) const
{
    static Group null_group;
    if( id.empty() ) {
        return null_group;
    }
    for( const Group &g : groups_ ) {
        if( g.id_ == id ) {
            return g;
        }
    }
    debugmsg( "Option group with id '%d' does not exist.", id );
    return null_group;
}

void options_manager::cOpt::setPrerequisites( const std::string &sOption,
        const std::vector<std::string> &sAllowedValues )
{
    const bool hasOption = get_options().has_option( sOption );
    if( !hasOption ) {
        debugmsg( "setPrerequisite: unknown option %s", sType );
        return;
    }

    const cOpt &existingOption = get_options().get_option( sOption );
    const std::string &existingOptionType = existingOption.getType();
    bool isOfSupportType = false;
    for( const std::string &sSupportedType : getPrerequisiteSupportedTypes() ) {
        if( existingOptionType == sSupportedType ) {
            isOfSupportType = true;
            break;
        }
    }

    if( !isOfSupportType ) {
        debugmsg( "setPrerequisite: option %s not of supported type", sType );
        return;
    }

    sPrerequisite = sOption;
    sPrerequisiteAllowedValues = sAllowedValues;
}

std::string options_manager::cOpt::getPrerequisite() const
{
    return sPrerequisite;
}

bool options_manager::cOpt::hasPrerequisite() const
{
    return !sPrerequisite.empty();
}

bool options_manager::cOpt::checkPrerequisite() const
{
    if( !hasPrerequisite() ) {
        return true;
    }
    bool isPrerequisiteFulfilled = false;
    const std::string prerequisite_option_value = get_options().get_option( sPrerequisite ).getValue();
    for( const std::string &sAllowedPrerequisiteValue : sPrerequisiteAllowedValues ) {
        if( prerequisite_option_value == sAllowedPrerequisiteValue ) {
            isPrerequisiteFulfilled = true;
            break;
        }
    }
    return isPrerequisiteFulfilled;
}

//helper functions
bool options_manager::cOpt::is_hidden() const
{
    switch( hide ) {
        case COPT_NO_HIDE:
            return false;

        case COPT_SDL_HIDE:
#if defined(TILES)
            return true;
#else
            return false;
#endif

        case COPT_CURSES_HIDE:
#if !defined(TILES) // If not defined.  it's curses interface.
            return true;
#else
            return false;
#endif

        case COPT_POSIX_CURSES_HIDE:
            // Check if we on windows and using wincurses.
#if defined(TILES) || defined(_WIN32)
            return false;
#else
            return true;
#endif

        case COPT_NO_SOUND_HIDE:
#if !defined(SDL_SOUND) // If not defined, we have no sound support.
            return true;
#else
            return false;
#endif

        case COPT_ALWAYS_HIDE:
            return true;
    }
    // Make compiler happy, this is unreachable.
    return false;
}

std::string options_manager::cOpt::getName() const
{
    return sName;
}

std::string options_manager::cOpt::getPage() const
{
    return sPage;
}

std::string options_manager::cOpt::getMenuText() const
{
    return _( sMenuText );
}

std::string options_manager::cOpt::getTooltip() const
{
    return _( sTooltip );
}

std::string options_manager::cOpt::getType() const
{
    return sType;
}

bool options_manager::cOpt::operator==( const cOpt &rhs ) const
{
    if( sType != rhs.sType ) {
        return false;
    } else if( sType == "string_select" || sType == "string_input" ) {
        return sSet == rhs.sSet;
    } else if( sType == "bool" ) {
        return bSet == rhs.bSet;
    } else if( sType == "int" || sType == "int_map" ) {
        return iSet == rhs.iSet;
    } else if( sType == "float" ) {
        return fSet == rhs.fSet;
    } else if( sType == "VOID" ) {
        return true;
    } else {
        debugmsg( "unknown option type %s", sType );
        return false;
    }
}

std::string options_manager::cOpt::getValue( bool classis_locale ) const
{
    if( sType == "string_select" || sType == "string_input" ) {
        return sSet;

    } else if( sType == "bool" ) {
        return bSet ? "true" : "false";

    } else if( sType == "int" || sType == "int_map" ) {
        return string_format( format, iSet );

    } else if( sType == "float" ) {
        std::ostringstream ssTemp;
        ssTemp.imbue( classis_locale ? std::locale::classic() : std::locale() );
        ssTemp.precision( 2 );
        ssTemp.setf( std::ios::fixed, std::ios::floatfield );
        ssTemp << fSet;
        return ssTemp.str();
    }

    return "";
}

template<>
std::string options_manager::cOpt::value_as<std::string>() const
{
    if( eType != CVT_STRING ) {
        debugmsg( "%s tried to get string value from option of type %s", sName, sType );
    }
    return sSet;
}

template<>
bool options_manager::cOpt::value_as<bool>() const
{
    if( eType != CVT_BOOL ) {
        debugmsg( "%s tried to get boolean value from option of type %s", sName, sType );
    }
    return bSet;
}

template<>
float options_manager::cOpt::value_as<float>() const
{
    if( eType != CVT_FLOAT ) {
        debugmsg( "%s tried to get float value from option of type %s", sName, sType );
    }
    return fSet;
}

template<>
int options_manager::cOpt::value_as<int>() const
{
    if( eType != CVT_INT ) {
        debugmsg( "%s tried to get integer value from option of type %s", sName, sType );
    }
    return iSet;
}

std::string options_manager::cOpt::getValueName() const
{
    if( sType == "string_select" ) {
        const auto iter = std::find_if( vItems.begin(),
        vItems.end(), [&]( const id_and_option & e ) {
            return e.first == sSet;
        } );
        if( iter != vItems.end() ) {
            return iter->second.translated();
        }

    } else if( sType == "bool" ) {
        return bSet ? _( "True" ) : _( "False" );

    } else if( sType == "int_map" ) {
        const std::string name = std::get<1>( *findInt( iSet ) );
        if( verbose ) {
            return string_format( _( "%d: %s" ), iSet, name );
        } else {
            return string_format( _( "%s" ), name );
        }
    }

    return getValue();
}

std::string options_manager::cOpt::getDefaultText( const bool bTranslated ) const
{
    if( sType == "string_select" ) {
        const auto iter = std::find_if( vItems.begin(), vItems.end(),
        [this]( const id_and_option & elem ) {
            return elem.first == sDefault;
        } );
        const std::string defaultName = iter == vItems.end() ? std::string() :
                                        bTranslated ? iter->second.translated() : iter->first;
        const std::string &sItems = enumerate_as_string( vItems.begin(), vItems.end(),
        [bTranslated]( const id_and_option & elem ) {
            return bTranslated ? elem.second.translated() : elem.first;
        }, enumeration_conjunction::none );
        return string_format( _( "Default: %s - Values: %s" ), defaultName, sItems );

    } else if( sType == "string_input" ) {
        return string_format( _( "Default: %s" ), sDefault );

    } else if( sType == "bool" ) {
        return bDefault ? _( "Default: True" ) : _( "Default: False" );

    } else if( sType == "int" ) {
        return string_format( _( "Default: %d - Min: %d, Max: %d" ), iDefault, iMin, iMax );

    } else if( sType == "int_map" ) {
        const std::string name = std::get<1>( *findInt( iDefault ) );
        if( verbose ) {
            return string_format( _( "Default: %d: %s" ), iDefault, name );
        } else {
            return string_format( _( "Default: %s" ), name );
        }

    } else if( sType == "float" ) {
        return string_format( _( "Default: %.2f - Min: %.2f, Max: %.2f" ), fDefault, fMin, fMax );
    }

    return "";
}

int options_manager::cOpt::getItemPos( const std::string &sSearch ) const
{
    if( sType == "string_select" ) {
        for( size_t i = 0; i < vItems.size(); i++ ) {
            if( vItems[i].first == sSearch ) {
                return i;
            }
        }
    }

    return -1;
}

std::vector<options_manager::id_and_option> options_manager::cOpt::getItems() const
{
    return vItems;
}

int options_manager::cOpt::getIntPos( const int iSearch ) const
{
    if( sType == "int_map" ) {
        for( size_t i = 0; i < mIntValues.size(); i++ ) {
            if( std::get<0>( mIntValues[i] ) == iSearch ) {
                return i;
            }
        }
    }

    return -1;
}

std::optional< std::tuple<int, std::string> > options_manager::cOpt::findInt(
    const int iSearch ) const
{
    int i = static_cast<int>( getIntPos( iSearch ) );
    if( i == -1 ) {
        return std::nullopt;
    }
    return mIntValues[i];
}

int options_manager::cOpt::getMaxLength() const
{
    if( sType == "string_input" ) {
        return iMaxLength;
    }

    return 0;
}

//set to next item
void options_manager::cOpt::setNext()
{
    if( sType == "string_select" ) {
        int iNext = getItemPos( sSet ) + 1;
        if( iNext >= static_cast<int>( vItems.size() ) ) {
            iNext = 0;
        }

        sSet = vItems[iNext].first;

    } else if( sType == "string_input" ) {
        int iMenuTextLength = utf8_width( _( sMenuText ) );
        string_input_popup()
        .width( iMaxLength > 80 ? 80 : iMaxLength < iMenuTextLength ? iMenuTextLength : iMaxLength + 1 )
        .description( _( sMenuText ) )
        .max_length( iMaxLength )
        .edit( sSet );

    } else if( sType == "bool" ) {
        bSet = !bSet;

    } else if( sType == "int" ) {
        iSet++;
        if( iSet > iMax ) {
            iSet = iMin;
        }

    } else if( sType == "int_map" ) {
        unsigned int iNext = getIntPos( iSet ) + 1;
        if( iNext >= mIntValues.size() ) {
            iNext = 0;
        }
        iSet = std::get<0>( mIntValues[iNext] );

    } else if( sType == "float" ) {
        fSet += fStep;
        if( fSet > fMax ) {
            fSet = fMin;
        }
    }
}

//set to previous item
void options_manager::cOpt::setPrev()
{
    if( sType == "string_select" ) {
        int iPrev = static_cast<int>( getItemPos( sSet ) ) - 1;
        if( iPrev < 0 ) {
            iPrev = vItems.size() - 1;
        }

        sSet = vItems[iPrev].first;

    } else if( sType == "string_input" ) {
        setNext();

    } else if( sType == "bool" ) {
        bSet = !bSet;

    } else if( sType == "int" ) {
        iSet--;
        if( iSet < iMin ) {
            iSet = iMax;
        }

    } else if( sType == "int_map" ) {
        int iPrev = static_cast<int>( getIntPos( iSet ) ) - 1;
        if( iPrev < 0 ) {
            iPrev = mIntValues.size() - 1;
        }
        iSet = std::get<0>( mIntValues[iPrev] );

    } else if( sType == "float" ) {
        fSet -= fStep;
        if( fSet < fMin ) {
            fSet = fMax;
        }
    }
}

//set value
void options_manager::cOpt::setValue( float fSetIn )
{
    if( sType != "float" ) {
        debugmsg( "tried to set a float value to a %s option", sType );
        return;
    }
    fSet = fSetIn;
    if( fSet < fMin || fSet > fMax ) {
        fSet = fDefault;
    }
}

//set value
void options_manager::cOpt::setValue( int iSetIn )
{
    if( sType != "int" ) {
        debugmsg( "tried to set an int value to a %s option", sType );
        return;
    }
    iSet = iSetIn;
    if( iSet < iMin || iSet > iMax ) {
        iSet = iDefault;
    }
}

//set value
void options_manager::cOpt::setValue( std::string sSetIn )
{
    if( sType == "string_select" ) {
        if( getItemPos( sSetIn ) != -1 ) {
            sSet = sSetIn;
        }

    } else if( sType == "string_input" ) {
        sSet = iMaxLength > 0 ? sSetIn.substr( 0, iMaxLength ) : sSetIn;

    } else if( sType == "bool" ) {
        bSet = sSetIn == "True" || sSetIn == "true" || sSetIn == "T" || sSetIn == "t";

    } else if( sType == "int" ) {
        iSet = atoi( sSetIn.c_str() );

        if( iSet < iMin || iSet > iMax ) {
            iSet = iDefault;
        }

    } else if( sType == "int_map" ) {
        iSet = atoi( sSetIn.c_str() );

        auto item = findInt( iSet );
        if( !item ) {
            iSet = iDefault;
        }

    } else if( sType == "float" ) {
        std::istringstream ssTemp( sSetIn );
        ssTemp.imbue( std::locale::classic() );
        float tmpFloat;
        ssTemp >> tmpFloat;
        if( ssTemp ) {
            setValue( tmpFloat );
        } else {
            debugmsg( "invalid floating point option: %s", sSetIn );
        }
    }
}

/** Fill a mapping with values.
 * Scans all directories in @p dirname directory for
 * a file named @p filename.
 * All found values added to resource_option as name, resource_dir.
 * Furthermore, it builds possible values list for cOpt class.
 */
static std::vector<options_manager::id_and_option> build_resource_list(
    std::map<std::string, std::string> &resource_option, const std::string &operation_name,
    const std::string &dirname, const std::string &filename )
{
    std::vector<options_manager::id_and_option> resource_names;

    resource_option.clear();
    const auto resource_dirs = get_directories_with( filename, dirname, true );

    for( auto &resource_dir : resource_dirs ) {
        read_from_file( resource_dir + "/" + filename, [&]( std::istream & fin ) {
            std::string resource_name;
            std::string view_name;
            // should only have 2 values inside it, otherwise is going to only load the last 2 values
            while( !fin.eof() ) {
                std::string sOption;
                fin >> sOption;

                if( sOption.empty() ) {
                    getline( fin, sOption );    // Empty line, chomp it
                } else if( sOption[0] == '#' ) { // # indicates a comment
                    getline( fin, sOption );
                } else {
                    if( sOption.find( "NAME" ) != std::string::npos ) {
                        resource_name.clear();
                        getline( fin, resource_name );
                        resource_name = trim( resource_name );
                    } else if( sOption.find( "VIEW" ) != std::string::npos ) {
                        view_name.clear();
                        getline( fin, view_name );
                        view_name = trim( view_name );
                        break;
                    }
                }
            }
            resource_names.emplace_back( resource_name,
                                         view_name.empty() ? no_translation( resource_name ) : to_translation( view_name ) );
            if( resource_option.count( resource_name ) != 0 ) {
                debugmsg( "Found \"%s\" duplicate with name \"%s\" (new definition will be ignored)",
                          operation_name, resource_name );
            } else {
                resource_option.insert( std::pair<std::string, std::string>( resource_name, resource_dir ) );
            }
        } );
    }

    return resource_names;
}

std::vector<options_manager::id_and_option> options_manager::load_tilesets_from(
    const std::string &path )
{
    // Use local map as build_resource_list will clear the first parameter
    std::map<std::string, std::string> local_tilesets;
    auto tileset_names = build_resource_list( local_tilesets, "tileset", path,
                         PATH_INFO::tileset_conf() );

    // Copy found tilesets
    TILESETS.insert( local_tilesets.begin(), local_tilesets.end() );

    return tileset_names;
}

std::vector<options_manager::id_and_option> options_manager::build_tilesets_list()
{
    // Clear tilesets
    TILESETS.clear();
    std::vector<id_and_option> result;

    // Load from data directory
    std::vector<options_manager::id_and_option> data_tilesets = load_tilesets_from(
                PATH_INFO::gfxdir() );
    result.insert( result.end(), data_tilesets.begin(), data_tilesets.end() );

    // Load from user directory
    std::vector<options_manager::id_and_option> user_tilesets = load_tilesets_from(
                PATH_INFO::user_gfx() );
    for( options_manager::id_and_option id : user_tilesets ) {
        if( std::find( result.begin(), result.end(), id ) == result.end() ) {
            result.emplace_back( id );
        }
    }

    // Default values
    if( result.empty() ) {
        result.emplace_back( "hoder", to_translation( "Hoder's" ) );
        result.emplace_back( "deon", to_translation( "Deon's" ) );
    }
    return result;
}

std::vector<options_manager::id_and_option> options_manager::load_soundpack_from(
    const std::string &path )
{
    // build_resource_list will clear &resource_option - first param
    std::map<std::string, std::string> local_soundpacks;
    auto soundpack_names = build_resource_list( local_soundpacks, "soundpack", path,
                           PATH_INFO::soundpack_conf() );

    // Copy over found soundpacks
    SOUNDPACKS.insert( local_soundpacks.begin(), local_soundpacks.end() );

    // Return found soundpack names for further processing
    return soundpack_names;
}

std::vector<options_manager::id_and_option> options_manager::build_soundpacks_list()
{
    // Clear soundpacks before loading
    SOUNDPACKS.clear();
    std::vector<id_and_option> result;

    // Search data directory for sound packs
    auto data_soundpacks = load_soundpack_from( PATH_INFO::data_sound() );
    result.insert( result.end(), data_soundpacks.begin(), data_soundpacks.end() );

    // Search user directory for sound packs
    auto user_soundpacks = load_soundpack_from( PATH_INFO::user_sound() );
    result.insert( result.end(), user_soundpacks.begin(), user_soundpacks.end() );

    // Select default built-in sound pack
    if( result.empty() ) {
        result.emplace_back( "basic", to_translation( "Basic" ) );
    }
    return result;
}

#if defined(__ANDROID__)
bool android_get_default_setting( const char *settings_name, bool default_value )
{
    JNIEnv *env = ( JNIEnv * )SDL_AndroidGetJNIEnv();
    jobject activity = ( jobject )SDL_AndroidGetActivity();
    jclass clazz( env->GetObjectClass( activity ) );
    jmethodID method_id = env->GetMethodID( clazz, "getDefaultSetting", "(Ljava/lang/String;Z)Z" );
    jboolean ans = env->CallBooleanMethod( activity, method_id, env->NewStringUTF( settings_name ),
                                           default_value );
    env->DeleteLocalRef( activity );
    env->DeleteLocalRef( clazz );
    return ans;
}
#endif

void options_manager::Page::removeRepeatedEmptyLines()
{
    const auto empty = [&]( const PageItem & it ) -> bool {
        return it.type == ItemType::BlankLine ||
        ( it.type == ItemType::Option && get_options().get_option( it.data ).is_hidden() );
    };

    while( !items_.empty() && empty( items_.front() ) ) {
        items_.erase( items_.begin() );
    }
    while( !items_.empty() && empty( items_.back() ) ) {
        items_.erase( items_.end() - 1 );
    }
    for( auto iter = std::next( items_.begin() ); iter != items_.end(); ) {
        if( empty( *std::prev( iter ) ) && empty( *iter ) ) {
            iter = items_.erase( iter );
        } else {
            ++iter;
        }
    }
}

void options_manager::init()
{
    options.clear();
    for( Page &p : pages_ ) {
        p.items_.clear();
    }

    add_options_general();
    add_options_interface();
    add_options_graphics();
    add_options_debug();
    add_options_world_default();
    add_options_android();

    for( Page &p : pages_ ) {
        p.removeRepeatedEmptyLines();
    }
}

#if defined(TILES)
// Helper method to isolate #ifdeffed tiles code.
static void refresh_tiles( bool used_tiles_changed, bool pixel_minimap_height_changed, bool ingame,
                           bool force_tile_change )
{
    if( used_tiles_changed ) {
        // Disable UIs below to avoid accessing the tile context during loading.
        ui_adaptor dummy( ui_adaptor::disable_uis_below {} );
        //try and keep SDL calls limited to source files that deal specifically with them
        try {
            tilecontext->reinit();
            std::vector<mod_id> dummy;

            tilecontext->load_tileset(
                get_option<std::string>( "TILES" ),
                ingame ? world_generator->active_world->active_mod_order : dummy,
                /*precheck=*/false,
                /*force=*/force_tile_change,
                /*pump_events=*/true
            );
            //game_ui::init_ui is called when zoom is changed
            g->reset_zoom();
            g->mark_main_ui_adaptor_resize();
            tilecontext->do_tile_loading_report( []( std::string str ) {
                DebugLog( DL::Info, DC::Main ) << str;
            } );
        } catch( const std::exception &err ) {
            popup( _( "Loading the tileset failed: %s" ), err.what() );
            use_tiles = false;
            use_tiles_overmap = false;
        }
    } else if( ingame && pixel_minimap_option && pixel_minimap_height_changed ) {
        g->mark_main_ui_adaptor_resize();
    }
}
#else
static void refresh_tiles( bool, bool, bool, bool )
{
}
#endif // TILES

static void draw_borders_external(
    const catacurses::window &w, int horizontal_level, const std::set<int> &vert_lines,
    const bool world_options_only )
{
    if( !world_options_only ) {
        draw_border( w, BORDER_COLOR, _( " OPTIONS " ) );
    }
    // intersections
    mvwputch( w, point( 0, horizontal_level ), BORDER_COLOR, LINE_XXXO ); // |-
    mvwputch( w, point( getmaxx( w ) - 1, horizontal_level ), BORDER_COLOR, LINE_XOXX ); // -|
    for( auto &x : vert_lines ) {
        mvwputch( w, point( x + 1, getmaxy( w ) - 1 ), BORDER_COLOR, LINE_XXOX ); // _|_
    }
    wnoutrefresh( w );
}

static void draw_borders_internal( const catacurses::window &w, std::set<int> &vert_lines )
{
    for( int i = 0; i < getmaxx( w ); ++i ) {
        if( vert_lines.count( i ) != 0 ) {
            // intersection
            mvwputch( w, point( i, 0 ), BORDER_COLOR, LINE_OXXX );
        } else {
            // regular line
            mvwputch( w, point( i, 0 ), BORDER_COLOR, LINE_OXOX );
        }
    }
    wnoutrefresh( w );
}

std::string
options_manager::PageItem::fmt_tooltip( const Group &group,
                                        const options_manager::options_container &cont ) const
{
    switch( type ) {
        case ItemType::BlankLine:
            return "";
        case ItemType::GroupHeader: {
            return group.tooltip_.translated();
        }
        case ItemType::Option: {
            const std::string &opt_name = data;
            const cOpt &opt = cont.find( opt_name )->second;
            std::string ret = string_format( "%s #%s",
                                             opt.getTooltip(),
                                             opt.getDefaultText() );
#if defined(TILES) || defined(_WIN32)
            if( opt_name == "TERMINAL_X" ) {
                int new_window_width = 0;
                new_window_width = projected_window_width();

                ret += " -- ";
                ret += string_format(
                           vgettext( "The window will be %d pixel wide with the selected value.",
                                     "The window will be %d pixels wide with the selected value.",
                                     new_window_width ), new_window_width );
            } else if( opt_name == "TERMINAL_Y" ) {
                int new_window_height = 0;
                new_window_height = projected_window_height();

                ret += " -- ";
                ret += string_format(
                           vgettext( "The window will be %d pixel tall with the selected value.",
                                     "The window will be %d pixels tall with the selected value.",
                                     new_window_height ), new_window_height );
            }
#endif
            return ret;
        }
        default:
            abort();
    }
}

/** String with color */
struct string_col {
    std::string s;
    nc_color col;

    string_col() : col( c_black ) { }
    string_col( const std::string &s, nc_color col ) : s( s ), col( col ) { }
};

std::string options_manager::show( bool ingame, const bool world_options_only,
                                   const std::function<bool()> &on_quit )
{
    const int iWorldOptPage = std::find_if( pages_.begin(), pages_.end(), [&]( const Page & p ) {
        return p.id_ == world_default;
    } ) - pages_.begin();

    // temporary alias so the code below does not need to be changed
    options_container &OPTIONS = options;
    options_container &ACTIVE_WORLD_OPTIONS = world_options.has_value() ?
            *world_options.value() :
            OPTIONS;

    auto OPTIONS_OLD = OPTIONS;
    auto WOPTIONS_OLD = ACTIVE_WORLD_OPTIONS;
    if( world_generator->active_world == nullptr ) {
        ingame = false;
    }

    std::set<int> vert_lines;
    vert_lines.insert( 4 );
    vert_lines.insert( 60 );

    int iCurrentPage = world_options_only ? iWorldOptPage : 0;
    int iCurrentLine = 0;
    int iStartPos = 0;

    std::unordered_map<std::string, bool> groups_state;
    groups_state.emplace( "", true ); // Non-existent group
    for( const Group &g : groups_ ) {
        // Start collapsed
        groups_state.emplace( g.id_, false );
    }

    input_context ctxt( "OPTIONS" );
    ctxt.register_cardinal();
    ctxt.register_action( "QUIT" );
    ctxt.register_action( "NEXT_TAB" );
    ctxt.register_action( "PREV_TAB" );
    ctxt.register_action( "CONFIRM" );
    ctxt.register_action( "HELP_KEYBINDINGS" );

    const int iWorldOffset = world_options_only ? 2 : 0;
    int iMinScreenWidth = 0;
    const int iTooltipHeight = 4;
    int iContentHeight = 0;

    catacurses::window w_options_border;
    catacurses::window w_options_tooltip;
    catacurses::window w_options_header;
    catacurses::window w_options;

    const auto init_windows = [&]( ui_adaptor & ui ) {
        if( OPTIONS.find( "TERMINAL_X" ) != OPTIONS.end() ) {
            if( OPTIONS_OLD.find( "TERMINAL_X" ) != OPTIONS_OLD.end() ) {
                OPTIONS_OLD["TERMINAL_X"] = OPTIONS["TERMINAL_X"];
            }
            if( WOPTIONS_OLD.find( "TERMINAL_X" ) != WOPTIONS_OLD.end() ) {
                WOPTIONS_OLD["TERMINAL_X"] = OPTIONS["TERMINAL_X"];
            }
        }
        if( OPTIONS.find( "TERMINAL_Y" ) != OPTIONS.end() ) {
            if( OPTIONS_OLD.find( "TERMINAL_Y" ) != OPTIONS_OLD.end() ) {
                OPTIONS_OLD["TERMINAL_Y"] = OPTIONS["TERMINAL_Y"];
            }
            if( WOPTIONS_OLD.find( "TERMINAL_Y" ) != WOPTIONS_OLD.end() ) {
                WOPTIONS_OLD["TERMINAL_Y"] = OPTIONS["TERMINAL_Y"];
            }
        }

        iMinScreenWidth = std::max( FULL_SCREEN_WIDTH, TERMX / 2 );
        const int iOffsetX = TERMX > FULL_SCREEN_WIDTH ? ( TERMX - iMinScreenWidth ) / 2 : 0;
        iContentHeight = TERMY - 3 - iTooltipHeight - iWorldOffset;

        w_options_border  = catacurses::newwin( TERMY, iMinScreenWidth,
                                                point( iOffsetX, 0 ) );
        w_options_tooltip = catacurses::newwin( iTooltipHeight, iMinScreenWidth - 2,
                                                point( 1 + iOffsetX, 1 + iWorldOffset ) );
        w_options_header  = catacurses::newwin( 1, iMinScreenWidth - 2,
                                                point( 1 + iOffsetX, 1 + iTooltipHeight + iWorldOffset ) );
        w_options         = catacurses::newwin( iContentHeight, iMinScreenWidth - 2,
                                                point( 1 + iOffsetX, iTooltipHeight + 2 + iWorldOffset ) );

        ui.position_from_window( w_options_border );
    };

    ui_adaptor ui;
    ui.on_screen_resize( init_windows );
    init_windows( ui );
    ui.on_redraw( [&]( const ui_adaptor & ) {
        werase( w_options_header );
        werase( w_options_border );
        werase( w_options_tooltip );
        werase( w_options );

        if( world_options_only ) {
            worldfactory::draw_worldgen_tabs( w_options_border, 1 );
        }

        draw_borders_external( w_options_border, iTooltipHeight + 1 + iWorldOffset, vert_lines,
                               world_options_only );
        draw_borders_internal( w_options_header, vert_lines );

        auto &cOPTIONS = ( ingame || world_options_only ) && iCurrentPage == iWorldOptPage ?
                         ACTIVE_WORLD_OPTIONS : OPTIONS;

        const Page &page = pages_[iCurrentPage];
        const std::vector<PageItem> &page_items = page.items_;

        // Cache visible entries
        std::vector<int> visible_items;
        visible_items.reserve( page_items.size() );
        int curr_line_visible = 0;
        const auto is_visible = [&]( int i ) -> bool {
            const PageItem &it = page_items[i];
            switch( it.type )
            {
                case ItemType::GroupHeader:
                    return true;
                case ItemType::BlankLine:
                case ItemType::Option:
                    return groups_state[it.group];
                default:
                    abort();
            }
        };
        for( int i = 0; i < static_cast<int>( page_items.size() ); i++ ) {
            if( is_visible( i ) ) {
                visible_items.push_back( i );
                if( i == iCurrentLine ) {
                    curr_line_visible = static_cast<int>( visible_items.size() ) - 1;
                }
            }
        }

        // Format name & value strings for given entry
        const auto fmt_name_value = [&]( const PageItem & it, bool is_selected )
        -> std::pair<string_col, string_col> {
            const char *IN_GROUP_PREFIX = ": ";
            switch( it.type )
            {
                case ItemType::BlankLine: {
                    std::string name = it.group.empty() ? "" : IN_GROUP_PREFIX;
                    return { string_col( name, c_white ), string_col() };
                }
                case ItemType::GroupHeader: {
                    bool expanded = groups_state[it.group];
                    std::string name = expanded ? "- " : "+ ";
                    name += find_group( it.group ).name_.translated();
                    return std::make_pair( string_col( name, c_white ), string_col() );
                }
                case options_manager::ItemType::Option: {
                    const cOpt &opt = cOPTIONS.find( it.data )->second;
                    const bool hasPrerequisite = opt.hasPrerequisite();
                    const bool hasPrerequisiteFulfilled = opt.checkPrerequisite();

                    std::string name_prefix = it.group.empty() ? "" : IN_GROUP_PREFIX;
                    string_col name( name_prefix + opt.getMenuText(), !hasPrerequisite ||
                                     hasPrerequisiteFulfilled ? c_white : c_light_gray );

                    nc_color cLineColor;
                    if( hasPrerequisite && !hasPrerequisiteFulfilled ) {
                        cLineColor = c_light_gray;
                    } else if( opt.getValue() == "false" || opt.getValue() == "disabled" || opt.getValue() == "off" ) {
                        cLineColor = c_light_red;
                    } else {
                        cLineColor = c_light_green;
                    }

                    string_col value( opt.getValueName(), is_selected ? hilite( cLineColor ) : cLineColor );

                    return std::make_pair( name, value );
                }
                default:
                    abort();
            }
        };

        // Draw separation lines
        for( int x : vert_lines ) {
            for( int y = 0; y < iContentHeight; y++ ) {
                mvwputch( w_options, point( x, y ), BORDER_COLOR, LINE_XOXO );
            }
        }

        // Update scroll position
        calcStartPos( iStartPos, curr_line_visible, iContentHeight,
                      static_cast<int>( visible_items.size() ) );

        // where the column with the names starts
        const size_t name_col = 5;
        // where the column with the values starts
        const size_t value_col = 62;
        // 2 for the space between name and value column, 3 for the ">> "
        const size_t name_width = value_col - name_col - 2 - 3;
        const size_t value_width = getmaxx( w_options ) - value_col;
        // Draw options
        for( int i = iStartPos;
             i < iStartPos + ( iContentHeight > static_cast<int>( visible_items.size() ) ?
                               static_cast<int>( visible_items.size() ) : iContentHeight ); i++ ) {

            int line_pos = i - iStartPos; // Current line position in window.

            mvwprintz( w_options, point( 1, line_pos ), c_white, "%d", visible_items[i] + 1 );

            bool is_selected = visible_items[i] == iCurrentLine;
            if( is_selected ) {
                mvwprintz( w_options, point( name_col, line_pos ), c_yellow, ">>" );
            }

            const PageItem &curr_item = page_items[visible_items[i]];
            auto name_value = fmt_name_value( curr_item, is_selected );

            const std::string name = utf8_truncate( name_value.first.s, name_width );
            mvwprintz( w_options, point( name_col + 3, line_pos ), name_value.first.col, name );

            const std::string value = utf8_truncate( name_value.second.s, value_width );
            mvwprintz( w_options, point( value_col, line_pos ), name_value.second.col, value );
        }

        draw_scrollbar( w_options_border, iCurrentLine, iContentHeight,
                        static_cast<int>( visible_items.size() ),
                        point( 0, iTooltipHeight + 2 + iWorldOffset ), BORDER_COLOR );
        wnoutrefresh( w_options_border );

        //Draw Tabs
        if( !world_options_only ) {
            mvwprintz( w_options_header, point( 7, 0 ), c_white, "" );
            for( int i = 0; i < static_cast<int>( pages_.size() ); i++ ) {
                wprintz( w_options_header, c_white, "[" );
                if( ingame && i == iWorldOptPage ) {
                    wprintz( w_options_header, iCurrentPage == i ? hilite( c_light_green ) : c_light_green,
                             _( "Current world" ) );
                } else {
                    wprintz( w_options_header, iCurrentPage == i ? hilite( c_light_green ) : c_light_green,
                             "%s", pages_[i].name_ );
                }
                wprintz( w_options_header, c_white, "]" );
                wputch( w_options_header, BORDER_COLOR, LINE_OXOX );
            }
        }

        wnoutrefresh( w_options_header );

        const PageItem &curr_item = page_items[iCurrentLine];
        std::string tooltip = curr_item.fmt_tooltip( find_group( curr_item.group ), cOPTIONS );
        fold_and_print( w_options_tooltip, point_zero, iMinScreenWidth - 2, c_white, tooltip );

        if( ingame && iCurrentPage == iWorldOptPage ) {
            mvwprintz( w_options_tooltip, point( 3, 3 ), c_light_red, "%s", _( "Note: " ) );
            wprintz( w_options_tooltip, c_white, "%s",
                     _( "Some of these options may produce unexpected results if changed." ) );
        }
        wnoutrefresh( w_options_tooltip );

        wnoutrefresh( w_options );
    } );

    while( true ) {
        ui_manager::redraw();

        Page &page = pages_[iCurrentPage];
        auto &page_items = page.items_;

        auto &cOPTIONS = ( ingame || world_options_only ) && iCurrentPage == iWorldOptPage ?
                         ACTIVE_WORLD_OPTIONS : OPTIONS;

        const std::string action = ctxt.handle_input();

        if( world_options_only && ( action == "NEXT_TAB" || action == "PREV_TAB" ||
                                    ( action == "QUIT" && ( !on_quit || on_quit() ) ) ) ) {
            return action;
        }

        const PageItem &curr_item = page_items[iCurrentLine];

        const auto on_select_option = [&]() {
            cOpt &current_opt = cOPTIONS[curr_item.data];

            bool hasPrerequisite = current_opt.hasPrerequisite();
            bool hasPrerequisiteFulfilled = current_opt.checkPrerequisite();

            if( hasPrerequisite && !hasPrerequisiteFulfilled ) {
                popup( _( "Prerequisite for this option not met!\n(%s)" ),
                       get_options().get_option( current_opt.getPrerequisite() ).getMenuText() );
                return;
            }

            if( action == "LEFT" ) {
                current_opt.setPrev();
            } else if( action == "RIGHT" ) {
                current_opt.setNext();
            } else {
                assert( action == "CONFIRM" );
                if( current_opt.getType() == "bool" || current_opt.getType() == "string_select" ||
                    current_opt.getType() == "string_input" || current_opt.getType() == "int_map" ) {
                    current_opt.setNext();
                } else {
                    const bool is_int = current_opt.getType() == "int";
                    const bool is_float = current_opt.getType() == "float";
                    const std::string old_opt_val = current_opt.getValueName();
                    const std::string opt_val = string_input_popup()
                                                .title( current_opt.getMenuText() )
                                                .width( 10 )
                                                .text( old_opt_val )
                                                .only_digits( is_int )
                                                .query_string();
                    if( !opt_val.empty() && opt_val != old_opt_val ) {
                        if( is_float ) {
                            std::istringstream ssTemp( opt_val );
                            // This uses the current locale, to allow the users
                            // to use their own decimal format.
                            float tmpFloat;
                            ssTemp >> tmpFloat;
                            if( ssTemp ) {
                                current_opt.setValue( tmpFloat );

                            } else {
                                popup( _( "Invalid input: not a number" ) );
                            }
                        } else {
                            // option is of type "int": string_input_popup
                            // has taken care that the string contains
                            // only digits, parsing is done in setValue
                            current_opt.setValue( opt_val );
                        }
                    }
                }
            }
        };

        const auto is_selectable = [&]( int i ) -> bool {
            const PageItem &curr_item = page_items[i];
            switch( curr_item.type )
            {
                case ItemType::BlankLine:
                    return false;
                case ItemType::GroupHeader:
                    return true;
                case ItemType::Option:
                    return groups_state[curr_item.group];
                default:
                    abort();
            }
        };

        if( action == "DOWN" ) {
            do {
                iCurrentLine++;
                if( iCurrentLine >= static_cast<int>( page_items.size() ) ) {
                    iCurrentLine = 0;
                }
            } while( !is_selectable( iCurrentLine ) );
        } else if( action == "UP" ) {
            do {
                iCurrentLine--;
                if( iCurrentLine < 0 ) {
                    iCurrentLine = page_items.size() - 1;
                }
            } while( !is_selectable( iCurrentLine ) );
        } else if( action == "NEXT_TAB" ) {
            iCurrentLine = 0;
            iStartPos = 0;
            iCurrentPage++;
            if( iCurrentPage >= static_cast<int>( pages_.size() ) ) {
                iCurrentPage = 0;
            }
            sfx::play_variant_sound( "menu_move", "default", 100 );
        } else if( action == "PREV_TAB" ) {
            iCurrentLine = 0;
            iStartPos = 0;
            iCurrentPage--;
            if( iCurrentPage < 0 ) {
                iCurrentPage = pages_.size() - 1;
            }
            sfx::play_variant_sound( "menu_move", "default", 100 );
        } else if( action == "RIGHT" || action == "LEFT" || action == "CONFIRM" ) {
            switch( curr_item.type ) {
                case ItemType::Option: {
                    on_select_option();
                    break;
                }
                case ItemType::GroupHeader: {
                    bool &state = groups_state[curr_item.data];
                    state = !state;
                    break;
                }
                case ItemType::BlankLine: {
                    // Should never happen
                    break;
                }
                default:
                    abort();
            }
        } else if( action == "QUIT" ) {
            break;
        }
    }

    //Look for changes
    bool options_changed = false;
    bool world_options_changed = false;
    bool lang_changed = false;
    bool used_tiles_changed = false;
    bool pixel_minimap_changed = false;
    bool terminal_size_changed = false;
    bool force_tile_change = false;

    for( auto &iter : OPTIONS_OLD ) {
        if( iter.second != OPTIONS[iter.first] ) {
            options_changed = true;

            if( iter.second.getPage() == world_default ) {
                world_options_changed = true;
            }

            if( iter.first == "PIXEL_MINIMAP_HEIGHT"
                || iter.first == "PIXEL_MINIMAP_RATIO"
                || iter.first == "PIXEL_MINIMAP_MODE"
                || iter.first == "PIXEL_MINIMAP_SCALE_TO_FIT" ) {
                pixel_minimap_changed = true;

            } else if( iter.first == "TILES" || iter.first == "USE_TILES" || iter.first == "STATICZEFFECT" ||
                       iter.first == "MEMORY_MAP_MODE" ) {
                used_tiles_changed = true;
                if( iter.first == "STATICZEFFECT" || iter.first == "MEMORY_MAP_MODE" ) {
                    force_tile_change = true;
                }
            } else if( iter.first == "USE_LANG" ) {
                lang_changed = true;

            } else if( iter.first == "TERMINAL_X" || iter.first == "TERMINAL_Y" ) {
                terminal_size_changed = true;
            }
        }
    }
    for( auto &iter : WOPTIONS_OLD ) {
        if( iter.second != ACTIVE_WORLD_OPTIONS[iter.first] ) {
            options_changed = true;
            world_options_changed = true;
        }
    }

    if( options_changed ) {
        if( query_yn( _( "Save changes?" ) ) ) {
            static_popup popup;
            popup.message( "%s", _( "Please wait…\nApplying option changes…" ) );
            ui_manager::redraw();
            refresh_display();

            save();
            if( ingame && world_options_changed ) {
                world_generator->active_world->WORLD_OPTIONS = ACTIVE_WORLD_OPTIONS;
                world_generator->active_world->save();
            }
            g->on_options_changed();
        } else {
            lang_changed = false;
            terminal_size_changed = false;
            used_tiles_changed = false;
            pixel_minimap_changed = false;
            OPTIONS = OPTIONS_OLD;
            if( ingame && world_options_changed ) {
                ACTIVE_WORLD_OPTIONS = WOPTIONS_OLD;
            }
        }
    }

    if( lang_changed ) {
        set_language();
    }
    calendar::set_eternal_season( ::get_option<bool>( "ETERNAL_SEASON" ) );
    calendar::set_season_length( ::get_option<int>( "SEASON_LENGTH" ) );

#if !defined(__ANDROID__) && (defined(TILES) || defined(_WIN32))
    if( terminal_size_changed ) {
        int scaling_factor = get_scaling_factor();
        int TERMX = ::get_option<int>( "TERMINAL_X" );
        int TERMY = ::get_option<int>( "TERMINAL_Y" );
        TERMX -= TERMX % scaling_factor;
        TERMY -= TERMY % scaling_factor;
        get_option( "TERMINAL_X" ).setValue( std::max( FULL_SCREEN_WIDTH * scaling_factor, TERMX ) );
        get_option( "TERMINAL_Y" ).setValue( std::max( FULL_SCREEN_HEIGHT * scaling_factor, TERMY ) );
        save();

        resize_term( ::get_option<int>( "TERMINAL_X" ), ::get_option<int>( "TERMINAL_Y" ) );
    }
#else
    ( void ) terminal_size_changed;
#endif

    refresh_tiles( used_tiles_changed, pixel_minimap_changed, ingame, force_tile_change );

    return "";
}

void options_manager::serialize( JsonOut &json ) const
{
    json.start_array();

    for( const Page &p : pages_ ) {
        for( const PageItem &it : p.items_ ) {
            if( it.type != ItemType::Option ) {
                continue;
            }
            const auto iter = options.find( it.data );
            if( iter != options.end() ) {
                const auto &opt = iter->second;

                json.start_object();

                json.member( "info", opt.getTooltip() );
                json.member( "default", opt.getDefaultText( false ) );
                json.member( "name", opt.getName() );
                json.member( "value", opt.getValue( true ) );

                json.end_object();
            }
        }
    }

    json.end_array();
}

void options_manager::deserialize( JsonIn &jsin )
{
    jsin.start_array();
    while( !jsin.end_array() ) {
        JsonObject joOptions = jsin.get_object();
        joOptions.allow_omitted_members();

        const std::string name = migrateOptionName( joOptions.get_string( "name" ) );
        const std::string value = migrateOptionValue( joOptions.get_string( "name" ),
                                  joOptions.get_string( "value" ) );

        add_retry( name, value );
        options[ name ].setValue( value );
    }
}

std::string options_manager::migrateOptionName( const std::string &name ) const
{
    const auto iter = mMigrateOption.find( name );
    return iter != mMigrateOption.end() ? iter->second.first : name;
}

std::string options_manager::migrateOptionValue( const std::string &name,
        const std::string &val ) const
{
    const auto iter = mMigrateOption.find( name );
    if( iter == mMigrateOption.end() ) {
        return val;
    }

    const auto iter_val = iter->second.second.find( val );
    return iter_val != iter->second.second.end() ? iter_val->second : val;
}

void options_manager::cache_to_globals()
{
    enum_bitset<DL> levels;
    levels.set( DL::Error );
    for( const debug_log_level &e : debug_log_levels ) {
        levels.set( e.id, ::get_option<bool>( e.opt_id ) );
    }
    enum_bitset<DC> classes;
    classes.set( DC::DebugMsg ).set( DC::Main );
    for( const debug_log_class &e : debug_log_classes ) {
        classes.set( e.id, ::get_option<bool>( e.opt_id ) );
    }
    setDebugLogLevels( levels );
    setDebugLogClasses( classes );

    json_report_strict = test_mode || ::get_option<bool>( "STRICT_JSON_CHECKS" );
    display_mod_source = ::get_option<bool>( "MOD_SOURCE" );
    display_object_ids = ::get_option<bool>( "SHOW_IDS" );
    trigdist = ::get_option<bool>( "CIRCLEDIST" );
#if defined(TILES)
    use_tiles = ::get_option<bool>( "USE_TILES" );
    use_tiles_overmap = ::get_option<bool>( "USE_TILES_OVERMAP" );
#else
    use_tiles = false;
    use_tiles_overmap = false;
#endif
    log_from_top = ::get_option<std::string>( "LOG_FLOW" ) == "new_top";
    message_ttl = ::get_option<int>( "MESSAGE_TTL" );
    message_cooldown = ::get_option<int>( "MESSAGE_COOLDOWN" );
    fov_3d = ::get_option<bool>( "FOV_3D" );
    fov_3d_z_range = ::get_option<int>( "FOV_3D_Z_RANGE" );
    static_z_effect = ::get_option<bool>( "STATICZEFFECT" );
    PICKUP_RANGE = ::get_option<int>( "PICKUP_RANGE" );
#if defined(SDL_SOUND)
    sounds::sound_enabled = ::get_option<bool>( "SOUND_ENABLED" );
#endif
}

bool options_manager::save()
{
    const auto savefile = PATH_INFO::options();
    cache_to_globals();
    update_volumes();

    return write_to_file( savefile, [&]( std::ostream & fout ) {
        JsonOut jout( fout, true );
        serialize( jout );
    }, _( "options" ) );
}

void options_manager::load()
{
    const auto file = PATH_INFO::options();
    read_from_file_optional_json( file, [&]( JsonIn & jsin ) {
        deserialize( jsin );
    } );

    cache_to_globals();
}

bool options_manager::has_option( const std::string &name ) const
{
    return options.count( name );
}

options_manager::cOpt &options_manager::get_option( const std::string &name )
{
    if( options.count( name ) == 0 ) {
        debugmsg( "requested non-existing option %s", name );
    }
    if( !world_options.has_value() ) {
        // Global options contains the default for new worlds, which is good enough here.
        return options[name];
    }
    auto &wopts = *world_options.value();
    if( wopts.count( name ) == 0 ) {
        auto &opt = options[name];
        if( opt.getPage() != world_default ) {
            // Requested a non-world option, deliver it.
            return opt;
        }
        // May be a new option and an old world - import default from global options.
        wopts[name] = opt;
    }
    return wopts[name];
}

options_manager::options_container options_manager::get_world_defaults() const
{
    std::unordered_map<std::string, cOpt> result;
    for( auto &elem : options ) {
        if( elem.second.getPage() == world_default ) {
            result.insert( elem );
        }
    }
    return result;
}

void options_manager::set_world_options( options_container *options )
{
    if( options == nullptr ) {
        world_options.reset();
    } else {
        world_options = options;
    }
}
