#include <map>
#include <sstream>
#include <vector>

#include "calendar.h"
#include "cata_utility.h"
#include "creature.h"
#include "debug.h"
#include "effect.h"
#include "input.h"
#include "json.h"
#include "output.h"
#include "string_input_popup.h"
#include "units_serde.h"
#include "ui.h"
#include "uistate.h"

namespace debug_menu
{


namespace
{

struct entry_data {
    entry_data( const bodypart_str_id &body_part )
        : body_part( body_part )
    {}
    entry_data( const bodypart_str_id &body_part, const effect &eff )
        : body_part( body_part )
        , eff( &eff )
    {}
    bodypart_str_id body_part;
    // If not set, we're a body part
    cata::optional<const effect *> eff;
};

} // namespace

class effect_select_callback : public uilist_callback
{
    private:

        // This is "almost static", might be cached on finalize
        const std::vector<efftype_id> all_effects = find_all_effect_types();

    public:
        effect_select_callback() {

        }

        void refresh( uilist *menu ) override {
            size_t selected = clamp<size_t>( menu->selected, 0, all_effects.size() - 1 );
            const int startx = menu->w_width - menu->pad_right;
            mvwprintw( menu->window, point( startx, 3 ), all_effects[selected].str() );
        }
};

class bodypart_select_callback : public uilist_callback
{

};

class effect_edit_callback : public uilist_callback
{
    private:
        typedef void ( effect_edit_callback::*effect_edit_fun )(
            const input_context &,
            const input_event &,
            int,
            uilist & );
        static const std::map<std::string, effect_edit_fun> handled_actions;

        Creature &c;

        // This is "almost static", might be cached on finalize
        const std::vector<efftype_id> all_effects = find_all_effect_types();
        // Should be aligned with entries
        const std::vector<entry_data> meta;
        std::function<void()> on_creature_changed;

    public:
        effect_edit_callback( Creature &c,
                              const std::vector<entry_data> &meta,
                              const std::function<void()> &on_creature_changed )
            : c( c )
            , meta( meta )
            , on_creature_changed( on_creature_changed )
        {}

        static std::vector<std::string> get_handled_actions() {
            std::vector<std::string> actions;
            std::transform( handled_actions.begin(), handled_actions.end(), std::back_inserter( actions ),
            []( const auto & pr ) {
                return pr.first;
            } );
            return actions;
        }


        bool key( const input_context &ctx, const input_event &key, int entnum,
                  uilist *parent_menu ) override {
            const std::string &action = ctx.input_to_action( key );
            auto iter = handled_actions.find( action );
            if( iter != handled_actions.end() ) {
                // This ugly thing is a call to method using a pointer-to-member-method
                ( ( *this ).*( iter->second ) )( ctx, key, entnum, *parent_menu );
                return true;
            }
            return false;

        }

    private:
        void add_effect_submenu( const input_context &, const input_event &, int,
                                 uilist & ) {
            wisheffect_state &last_val = uistate.debug_menu.effect;
            uilist submenu;
            /*
            submenu.w_x_setup = 0;
            submenu.w_width_setup = []() -> int {
                return TERMX;
            };
            submenu.pad_right_setup = []() -> int {
                return TERMX - 40;
            };
            */
            //effect_select_callback callback;
            //submenu.callback = &callback;
            int i = 0;
            for( const efftype_id &eff : all_effects ) {
                submenu.addentry( i++, true, -1, eff.str() );
            }
            submenu.set_selected( last_val.selected );

            submenu.query();
            if( submenu.ret >= 0 && submenu.ret < static_cast<int>( all_effects.size() ) ) {
                const auto eff_type = all_effects[static_cast<size_t>( submenu.ret )];
                last_val.selected = submenu.ret;
                time_duration duration = last_val.duration <= 0_seconds ? eff_type->get_max_duration() :
                                         last_val.duration;
                c.add_effect( eff_type, duration, last_val.bodypart, last_val.intensity, last_val.force );
                on_creature_changed();
            }
        }

        void clear_effects( const input_context &, const input_event &, int,
                            uilist & ) {
            on_creature_changed();
            c.clear_effects();
            if( !c.get_all_effects().empty() ) {
                debugmsg( "Couldn't clear effects" );
            }
        }
        void remove( const input_context &, const input_event &, int entnum,
                     uilist & ) {
            on_creature_changed();
            const entry_data &entry = meta[entnum];
            if( entry.eff ) {
                bool removed = c.remove_effect( ( *entry.eff )->get_id(), entry.body_part );
                if( !removed ) {
                    debugmsg( "Couldn't remove %s from %s",
                              ( *entry.eff )->get_id().str(),
                              entry.body_part.str() );
                }
            } else {
                // Body part, so remove all effects from it
                for( const auto &pr : c.get_all_effects() ) {
                    for( const auto &inner : pr.second ) {
                        bool removed = c.remove_effect( pr.first, inner.first );
                        if( !removed ) {
                            debugmsg( "Couldn't remove %s from %s",
                                      ( *entry.eff )->get_id().str(),
                                      entry.body_part.str() );
                        }
                    }
                }
            }
        }
        void change_body_part( const input_context &, const input_event &, int,
                               uilist &parent_menu ) {
            wisheffect_state &last_val = uistate.debug_menu.effect;
            const std::vector<bodypart_id> &all_bps = c.get_all_body_parts();
            uilist menu;
            size_t i = 0;
            for( const bodypart_id &bp : all_bps ) {
                menu.addentry( i++, true, -1, bp.id().str() );
            }
            menu.query();
            if( menu.ret >= 0 && menu.ret < static_cast<int>( all_bps.size() ) ) {
                last_val.bodypart = all_bps[static_cast<size_t>( menu.ret )].id();
                int bp_index = static_cast<int>( meta.size() );
                // Find the entry for our body part
                for( ; bp_index > 1; bp_index-- ) {
                    if( meta[bp_index].body_part == last_val.bodypart ) {
                        break;
                    }
                }
                parent_menu.set_selected( bp_index );
            }
        }
        void change_intensity( const input_context &, const input_event &, int,
                               uilist & ) {
            wisheffect_state &last_val = uistate.debug_menu.effect;
            last_val.intensity = string_input_popup()
                                 .title( _( "Input new intensity" ) )
                                 .only_digits( true )
                                 .query_int();
        }
        void change_duration( const input_context &, const input_event &, int,
                              uilist & ) {
            wisheffect_state &last_val = uistate.debug_menu.effect;
            // Ugly hack, but does the job
            string_input_popup popup;
            popup.title( _( "Input new duration followed by unit (s, m, h, d)" ) );
            std::string dur_string = popup.query_string();
            if( !popup.confirmed() ) {
                return;
            }
            time_duration duration;
            // Have to wrap it in quotes because we're reading a raw string
            std::istringstream iss( '"' + dur_string + '"' );
            try {
                JsonIn jsin( iss );
                last_val.duration = read_from_json_string<time_duration>( jsin, time_duration::units );
            } catch( const JsonError &e ) {
                debugmsg( e.c_str() );
            }
        }
        void toggle_force( const input_context &, const input_event &, int,
                           uilist & ) {
            wisheffect_state &last_val = uistate.debug_menu.effect;
            last_val.force = !last_val.force;
        }
};

const std::map<std::string, effect_edit_callback::effect_edit_fun>
effect_edit_callback::handled_actions = {{
        { "ADD_NEW_EFFECT", &effect_edit_callback::add_effect_submenu },
        { "CLEAR_EFFECTS", &effect_edit_callback::clear_effects },
        { "REMOVE", &effect_edit_callback::remove },
        { "CHANGE_BODY_PART", &effect_edit_callback::change_body_part },
        { "CHANGE_INTENSITY", &effect_edit_callback::change_intensity },
        { "CHANGE_DURATION", &effect_edit_callback::change_duration },
        { "TOGGLE_FORCE", &effect_edit_callback::toggle_force },
    }
};

void effect_edit_menu( Creature &c )
{
    bool not_dirty = false;
    bool stay_in_menu = true;
    int last_selected = 0;
    while( stay_in_menu ) {
        const effects_map &em = c.get_all_effects();
        uilist menu;
        menu.input_category = "edit_creature_effects";
        std::map<bodypart_str_id, std::vector<const effect *>> bp_effects;
        const std::vector<bodypart_id> &all_bps = c.get_all_body_parts();
        for( const auto &pr : em ) {
            for( const auto &inner : pr.second ) {
                bp_effects[inner.first].push_back( &inner.second );
                if( !inner.first.is_null() &&
                    std::count( all_bps.begin(), all_bps.end(), inner.first ) == 0 ) {
                    debugmsg( "Creature %s has effect on body part %s, not in anatomy",
                              c.get_name(), inner.first.str() );
                }
            }
        }

        std::vector<entry_data> meta;
        size_t i = 0;
        const auto add_effect_entry = [&menu, &i]( const effect & eff ) {
            // TODO: Columns
            std::string effect_description = string_format(
                                                 "- %s %d, %d s",
                                                 eff.get_id().str(),
                                                 eff.get_intensity(),
                                                 to_seconds<int>( eff.get_duration() ) );
            menu.addentry( i++, true, -1, effect_description );
        };
        menu.addentry( i++, true, -1, _( "Global" ) );
        auto global_iter = bp_effects.find( bodypart_str_id::NULL_ID() );
        meta.emplace_back( bodypart_str_id::NULL_ID() );
        if( global_iter != bp_effects.end() ) {
            for( const effect *eff : global_iter->second ) {
                add_effect_entry( *eff );
                meta.emplace_back( bodypart_str_id::NULL_ID(), *eff );
            }

        }
        for( const bodypart_id &bp : all_bps ) {
            menu.addentry( i++, true, -1, bp.id().str() );
            meta.emplace_back( bp.id() );
            auto iter = bp_effects.find( bp.id() );
            if( iter != bp_effects.end() ) {
                for( const effect *eff : iter->second ) {
                    add_effect_entry( *eff );
                    meta.emplace_back( bp.id(), *eff );
                }
            }
        }

        const auto on_creature_changed = [&not_dirty]() {
            not_dirty = false;
        };
        effect_edit_callback callback( c, meta, on_creature_changed );
        menu.callback = &callback;
        for( const std::string &action : callback.get_handled_actions() ) {
            menu.additional_actions.emplace_back( action, translation() );
        }
        not_dirty = true;
        while( not_dirty ) {
            // Have to do this because menus don't support entry removal
            menu.set_selected( last_selected );
            menu.query( false );
            if( menu.ret == UILIST_CANCEL ) {
                stay_in_menu = false;
                break;
            }
            if( not_dirty ) {
                last_selected = menu.selected;
            }
        }
    }
}

}
