#include "avatar.h"
#include "item.h"
#include "flag.h"
#include "iteminfo_query.h"
#include "iteminfo_format_utils.h"
#include "itype.h"
#include "output.h"
#include "string_id.h"
#include "string_id_utils.h"
#include "units_utility.h"

namespace
{

struct armor_portion_type {
    int encumber;
    int max_encumber;
    int coverage;

    // TODO: use default constructor when we can use C++20
    auto operator==( const armor_portion_type &other ) -> bool {
        return encumber == other.encumber
               && max_encumber == other.max_encumber
               && coverage == other.coverage;
    };
};

struct body_part_display_info {
    translation to_display;
    armor_portion_type portion;
    bool active;
};

auto which_layer( const item &it ) -> std::string
{
    if( it.has_flag( flag_PERSONAL ) ) {
        return _( "<stat>Personal aura</stat>. " );
    } else if( it.has_flag( flag_SKINTIGHT ) ) {
        return  _( "<stat>Close to skin</stat>. " );
    } else if( it.has_flag( flag_BELTED ) ) {
        return  _( "<stat>Strapped</stat>. " );
    } else if( it.has_flag( flag_OUTER ) ) {
        return  _( "<stat>Outer</stat>. " );
    } else if( it.has_flag( flag_WAIST ) ) {
        return  _( "<stat>Waist</stat>. " );
    } else if( it.has_flag( flag_AURA ) ) {
        return  _( "<stat>Outer aura</stat>. " );
    } else {
        return  _( "<stat>Normal</stat>. " );
    }
}

auto sizing_info( const item::sizing sizing_level ) -> std::optional<std::string>
{
    using sizing = item::sizing;

    switch( sizing_level ) {
        case sizing::human_sized_small_char:
            return _( " <bad>(too big)</bad>" );
        case sizing::big_sized_small_char:
            return _( " <bad>(huge!)</bad>" );
        case sizing::small_sized_human_char:
        case sizing::human_sized_big_char:
            return _( " <bad>(too small)</bad>" );
        case sizing::small_sized_big_char:
            return _( " <bad>(tiny!)</bad>" );
        default:
            return {};
    }
}

} // namespace


void item::armor_info( std::vector<iteminfo> &info, const iteminfo_query *parts, int batch,
                       bool debug ) const
{
    if( !is_armor() ) {
        return;
    }

    avatar &you = get_avatar();
    const std::string space = "  ";
    body_part_set covered_parts = get_covered_body_parts();
    bool covers_anything = covered_parts.any();

    const auto names = enumerate_as_string( covered_parts,
    []( const bodypart_str_id & bp ) -> std::string {
        return bp.obj().name.translated();
    } );

    if( parts->test( iteminfo_parts::ARMOR_BODYPARTS ) ) {
        insert_separation_line( info );
        std::string coverage = _( "<bold>Covers</bold>: " );
        if( covers( bodypart_id( "head" ) ) ) {
            coverage += _( "The <info>head</info>. " );
        }
        if( covers( bodypart_id( "eyes" ) ) ) {
            coverage += _( "The <info>eyes</info>. " );
        }
        if( covers( bodypart_id( "mouth" ) ) ) {
            coverage += _( "The <info>mouth</info>. " );
        }
        if( covers( bodypart_id( "torso" ) ) ) {
            coverage += _( "The <info>torso</info>. " );
        }

        if( is_sided() && ( covers( bodypart_id( "arm_l" ) ) || covers( bodypart_id( "arm_r" ) ) ) ) {
            coverage += _( "Either <info>arm</info>. " );
        } else if( covers( bodypart_id( "arm_l" ) ) && covers( bodypart_id( "arm_r" ) ) ) {
            coverage += _( "The <info>arms</info>. " );
        } else if( covers( bodypart_id( "arm_l" ) ) ) {
            coverage += _( "The <info>left arm</info>. " );
        } else if( covers( bodypart_id( "arm_r" ) ) ) {
            coverage += _( "The <info>right arm</info>. " );
        }

        if( is_sided() && ( covers( bodypart_id( "hand_l" ) ) || covers( bodypart_id( "hand_r" ) ) ) ) {
            coverage += _( "Either <info>hand</info>. " );
        } else if( covers( bodypart_id( "hand_l" ) ) && covers( bodypart_id( "hand_r" ) ) ) {
            coverage += _( "The <info>hands</info>. " );
        } else if( covers( bodypart_id( "hand_l" ) ) ) {
            coverage += _( "The <info>left hand</info>. " );
        } else if( covers( bodypart_id( "hand_r" ) ) ) {
            coverage += _( "The <info>right hand</info>. " );
        }

        if( is_sided() && ( covers( bodypart_id( "leg_l" ) ) || covers( bodypart_id( "leg_r" ) ) ) ) {
            coverage += _( "Either <info>leg</info>. " );
        } else if( covers( bodypart_id( "leg_l" ) ) && covers( bodypart_id( "leg_r" ) ) ) {
            coverage += _( "The <info>legs</info>. " );
        } else if( covers( bodypart_id( "leg_l" ) ) ) {
            coverage += _( "The <info>left leg</info>. " );
        } else if( covers( bodypart_id( "leg_r" ) ) ) {
            coverage += _( "The <info>right leg</info>. " );
        }

        if( is_sided() && ( covers( bodypart_id( "foot_l" ) ) || covers( bodypart_id( "foot_r" ) ) ) ) {
            coverage += _( "Either <info>foot</info>. " );
        } else if( covers( bodypart_id( "foot_l" ) ) && covers( bodypart_id( "foot_r" ) ) ) {
            coverage += _( "The <info>feet</info>. " );
        } else if( covers( bodypart_id( "foot_l" ) ) ) {
            coverage += _( "The <info>left foot</info>. " );
        } else if( covers( bodypart_id( "foot_r" ) ) ) {
            coverage += _( "The <info>right foot</info>. " );
        }

        if( !covers_anything ) {
            coverage += _( "<info>Nothing</info>." );
        }

        info.emplace_back( iteminfo( "ARMOR", coverage ) );
    }

    if( parts->test( iteminfo_parts::ARMOR_LAYER ) && covers_anything ) {
        info.emplace_back( iteminfo( "ARMOR", _( "Layer: " ) + which_layer( *this ) ) );
    }

    if( parts->test( iteminfo_parts::ARMOR_COVERAGE ) && covers_anything ) {
        info.emplace_back( iteminfo( "ARMOR", _( "Average Coverage: " ), "<num>%",
                                     iteminfo::no_newline, get_avg_coverage() ) );
    }
    if( parts->test( iteminfo_parts::ARMOR_WARMTH ) && covers_anything ) {
        info.emplace_back( iteminfo( "ARMOR", space + _( "Warmth: " ), get_warmth() ) );
    }

    insert_separation_line( info );

    if( parts->test( iteminfo_parts::ARMOR_ENCUMBRANCE ) && covers_anything ) {
        std::string format;
        const bool sizing_matters = get_sizing( you ) != sizing::ignore;
        if( has_flag( flag_FIT ) ) {
            format = _( " <info>(fits)</info>" );
        } else if( has_flag( flag_VARSIZE ) && sizing_matters ) {
            format = _( " <bad>(poor fit)</bad>" );
        }
        if( sizing_matters ) {
            const auto sizing_level = get_sizing( you );
            const auto sizing_info_str = sizing_info( sizing_level );
            if( sizing_info_str ) {
                format = sizing_info_str.value();
            }
        }

        if( const islot_armor *t = find_armor_data() ) {
            if( !t->data.empty() ) {

                std::map<bodypart_str_id, body_part_display_info> to_display_data;
                const auto &avatar = get_avatar();
                for( const armor_portion_data &piece : t->data ) {
                    if( piece.covers.has_value() ) {
                        for( const bodypart_str_id &covering_id : piece.covers.value() ) {
                            if( covering_id != bodypart_str_id( "num_bp" ) ) {
                                const int encumbrance_when_full =
                                    get_encumber_when_containing( you, get_total_capacity(), covering_id.id() );
                                to_display_data[covering_id] = { covering_id.obj().name_as_heading, {
                                        get_encumber( avatar, covering_id ),
                                        encumbrance_when_full,
                                        piece.coverage
                                    }, true
                                };
                            }
                        }
                    }
                }
                // Handle things that use both sides to avoid showing L. Arm R. Arm etc when both are the same
                if( !t->sided ) {
                    for( const body_part &legacy_part : all_body_parts ) {
                        bodypart_str_id bp( convert_bp( legacy_part ) );
                        bodypart_str_id opposite = bp->opposite_part;
                        if( opposite != bp && covers( bp ) && covers( opposite )
                            && to_display_data.at( bp ).portion == to_display_data.at( opposite ).portion
                            && to_display_data.at( opposite ).active ) {
                            to_display_data.at( opposite ).to_display = bp->name_as_heading_multiple;
                            to_display_data.at( bp ).active = false;
                        }
                    }
                }

                const auto &sorted = sorted_lex( to_display_data );
                auto enabled = std::vector<std::pair<bodypart_str_id, body_part_display_info>>();
                std::copy_if( sorted.begin(), sorted.end(), std::back_inserter( enabled ),
                [&t, this]( const auto & piece ) {
                    if( !t->sided ) {
                        return true;
                    }
                    if( !piece.second.active ) {
                        return false;
                    }
                    const bodypart_str_id &covering_id = piece.first;
                    return this->covers( covering_id.id() );
                } );
                const auto item_encumbrances = [&t]( const item & it,
                                                     const std::vector<std::pair<bodypart_str_id, body_part_display_info>> &enabled ) ->
                std::vector<iteminfo> {
                    static const auto space = std::string{"  "};
                    auto info = std::vector<iteminfo>();
                    info.reserve( enabled.size() * 2 );
                    for( auto &piece : enabled )
                    {
                        const bool any_encumb_increase = !it.type->rigid || std::any_of( t->data.begin(), t->data.end(),
                        []( armor_portion_data data ) {
                            return data.encumber != data.max_encumber;
                        } );
                        info.emplace_back( iteminfo( "ARMOR",
                                                     string_format( _( "%-6s:" ), piece.second.to_display.translated() ) + space, "",
                                                     iteminfo::no_newline | iteminfo::lower_is_better,
                                                     piece.second.portion.encumber ) );
                        if( any_encumb_increase ) {
                            info.emplace_back( iteminfo( "ARMOR", space + _( "When Full:" ) + space, "",
                                                         iteminfo::no_newline | iteminfo::lower_is_better,
                                                         piece.second.portion.max_encumber ) );
                        }
                        info.emplace_back( iteminfo( "ARMOR", space + _( "Coverage:" ) + space, "",
                                                     iteminfo::lower_is_better,
                                                     piece.second.portion.coverage ) );
                    }
                    return info;
                };
                info.emplace_back( iteminfo( "ARMOR", _( "<bold>Encumbrance</bold>:" ), format,
                                             iteminfo::lower_is_better ) );
                const auto &tmp_info = item_encumbrances( *this, enabled );
                // efficiently append tmp_info to info
                info.insert( info.end(), tmp_info.begin(), tmp_info.end() );
            }
        }
    }

    int converted_storage_scale = 0;
    const double converted_storage = round_up( convert_volume( get_storage().value(),
                                     &converted_storage_scale ), 2 );
    if( parts->test( iteminfo_parts::ARMOR_STORAGE ) && converted_storage > 0 ) {
        const iteminfo::flags f = converted_storage_scale == 0 ? iteminfo::no_flags : iteminfo::is_decimal;
        info.emplace_back( iteminfo( "ARMOR", space + _( "Storage: " ),
                                     string_format( "<num> %s", volume_units_abbr() ),
                                     f, converted_storage ) );
    }

    // Whatever the last entry was, we want a newline at this point
    info.back().bNewLine = true;

    if( covers_anything ) {
        armor_protection_info( info, parts, batch, debug );
    }

    const units::mass weight_bonus = get_weight_capacity_bonus();
    const float weight_modif = get_weight_capacity_modifier();
    if( weight_modif != 1 ) {
        std::string modifier;
        if( weight_modif < 1 ) {
            modifier = "<num><bad>x</bad>";
        } else {
            modifier = "<num><color_light_green>x</color>";
        }
        info.emplace_back( iteminfo( "ARMOR",
                                     _( "<bold>Weight capacity modifier</bold>: " ), modifier,
                                     iteminfo::no_newline | iteminfo::is_decimal, weight_modif ) );
    }
    if( weight_bonus != 0_gram ) {
        std::string bonus;
        if( weight_bonus < 0_gram ) {
            bonus = string_format( "<num> <bad>%s</bad>", weight_units() );
        } else {
            bonus = string_format( "<num> <color_light_green> %s</color>", weight_units() );
        }
        info.emplace_back( iteminfo( "ARMOR", _( "<bold>Weight capacity bonus</bold>: " ), bonus,
                                     iteminfo::no_newline | iteminfo::is_decimal,
                                     convert_weight( weight_bonus ) ) );
    }
}
