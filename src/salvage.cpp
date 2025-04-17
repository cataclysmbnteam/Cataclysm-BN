#include "salvage.h"
#include "activity_actor_definitions.h"


#include "iuse_actor.cpp"

std::unordered_map<material_id, std::set<quality_id>> salvage_material_dictionary;
std::set<material_id> salvagable_materials;

// It is used to check if an item can be salvaged or not.
bool try_salvage( Character &who, item &it, bool mute = true )
{
    if( it.is_null() ) {
        if( !mute ) {
            add_msg( m_info, _( "You do not have that item." ) );
        }
        return false;
    }
    // There must be some historical significance to these items.
    if( !it.is_salvageable() ) {
        if( !mute ) {
            add_msg( m_info, _( "Can't salvage anything from %s." ), it.tname() );
            if( recipe_dictionary::get_uncraft( it.typeId() ) ) {
                add_msg( m_info, _( "Try disassembling the %s instead." ), it.tname() );
            }
        }
        return false;
    }

    if( !it.only_made_of( salvagable_materials ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is made of material that cannot be cut up." ), it.tname() );
        }
        return false;
    }
    if( !it.contents.empty() ) {
        if( !mute ) {
            add_msg( m_info, _( "Please empty the %s before cutting it up." ), it.tname() );
        }
        return false;
    }
    if( it.weight() < minimal_weight_to_cut( it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "The %s is too small to salvage material from." ), it.tname() );
        }
        return false;
    }
    if( !has_salvage_tools( who.crafting_inventory(), it ) ) {
        if( !mute ) {
            add_msg( m_info, _( "You lack proper tools to salvage material from the %s." ), it.tname() );
        }
        return false;
    }


    // Softer warnings at the end so we don't ask permission and then tell them no.
    if( !mute ) {
        if( who.is_wielding( it ) ) {
            if( !query_yn( _( "You are wielding that, are you sure?" ) ) ) {
                return false;
            }
        } else if( who.is_wearing( it ) ) {
            if( !query_yn( _( "You're wearing that, are you sure?" ) ) ) {
                return false;
            }
        }
        if( it.is_favorite ) {
            if( !query_yn( _( "This item is marked as favorite, are you sure?" ) ) ) {
                return false;
            }
        }
    }
    return true;
}


// function returns charges from it during the cutting process of the *cut.
// it cuts
// cut gets cut
void cut_up( Character &who, item &cut )
{
    const bool filthy = cut.is_filthy();
    // This is the value that tracks progress, as we cut pieces off, we reduce this number.
    units::mass remaining_weight = cut.weight();
    // Chance of us losing a material component to entropy.
    /** @EFFECT_FABRICATION reduces chance of losing components when cutting items up */
    int entropy_threshold = std::max( 5, 10 - who.get_skill_level( skill_fabrication ) );
    // What material components can we get back?
    std::vector<material_id> cut_material_components = cut.made_of();
    // What materials do we salvage (ids and counts).
    std::map<itype_id, int> materials_salvaged;

    // Not much practice, and you won't get very far ripping things up.
    who.practice( skill_fabrication, rng( 0, 5 ), 1 );

    // Higher fabrication, less chance of entropy, but still a chance.
    if( rng( 1, 10 ) <= entropy_threshold ) {
        remaining_weight *= 0.99;
    }
    // Fail dex roll, potentially lose more parts.
    /** @EFFECT_DEX randomly reduces component loss when cutting items up */
    if( dice( 3, 4 ) > who.dex_cur ) {
        remaining_weight *= 0.95;
    }
    // If more than 1 material component can still be salvaged,
    // chance of losing more components if the item is damaged.
    // If the item being cut is not damaged, no additional losses will be incurred.
    if( cut.damage() > 0 ) {
        float component_success_chance = std::min( std::pow( 0.8, cut.damage_level( 4 ) ),
                                         1.0 );
        remaining_weight *= component_success_chance;
    }

    // Essentially we round-robbin through the components subtracting mass as we go.
    std::map<units::mass, itype_id> weight_to_item_map;
    for( const material_id &material : cut_material_components ) {
        if( const std::optional<itype_id> id = material->salvaged_into() ) {
            materials_salvaged[*id] = 0;
            weight_to_item_map[( **id ).weight] = *id;
        }
    }
    while( remaining_weight > 0_gram && !weight_to_item_map.empty() ) {
        units::mass components_weight = std::accumulate( weight_to_item_map.begin(),
                                        weight_to_item_map.end(), 0_gram, []( const units::mass & a,
        const std::pair<units::mass, itype_id> &b ) {
            return a + b.first;
        } );
        if( components_weight > 0_gram && components_weight <= remaining_weight ) {
            int count = remaining_weight / components_weight;
            for( std::pair<units::mass, itype_id> mat_pair : weight_to_item_map ) {
                materials_salvaged[mat_pair.second] += count;
            }
            remaining_weight -= components_weight * count;
        }
        weight_to_item_map.erase( std::prev( weight_to_item_map.end() ) );
    }

    add_msg( m_info, _( "You try to salvage materials from the %s." ),
             cut.tname() );

    item_location_type cut_type = cut.where();
    tripoint pos = cut.position();

    // Clean up before removing the item.
    remove_ammo( cut, who );
    // Original item has been consumed.
    cut.detach();
    // Force an encumbrance update in case they were wearing that item.
    who.reset_encumbrance();

    map &here = get_map();
    for( const auto &salvaged : materials_salvaged ) {
        itype_id mat_name = salvaged.first;
        int amount = salvaged.second;
        item &result = *item::spawn_temporary( mat_name, calendar::turn );
        if( amount > 0 ) {
            // Time based on number of components.
            add_msg( m_good, vgettext( "Salvaged %1$i %2$s.", "Salvaged %1$i %2$s.", amount ),
                     amount, result.display_name( amount ) );
            if( filthy ) {
                result.set_flag( flag_FILTHY );
            }
            if( cut_type == item_location_type::character ) {
                while( amount-- ) {
                    who.i_add_or_drop( item::spawn( result ) );
                }
            } else {
                for( int i = 0; i < amount; i++ ) {
                    here.add_item_or_charges( pos, item::spawn( result ) );
                }
            }
        } else {
            add_msg( m_bad, _( "Could not salvage a %s." ), result.display_name() );
        }
    }
}

int time_to_cut_up( const item &it )
{
    units::mass total_material_weight;
    int num_materials = 0;
    visit_salvage_products( it, [&total_material_weight, &num_materials]( const item & exemplar ) {
        total_material_weight += exemplar.weight();
        num_materials += 1;
    } );
    if( num_materials == 0 ) {
        return 0;
    }
    units::mass average_material_weight = total_material_weight / num_materials;
    int count = it.weight() / average_material_weight;
    //TODO proper use
    return 25 * count;
}

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges = false )
{
    bool has_tools = false;
    for( auto &material : item.made_of_types() ) {
        auto it = salvage_material_dictionary.find( material->id );
        if( it != salvage_material_dictionary.end() ) {
            has_tools = false;
            for( auto &quality : it->second ) {
                //if(check_charges)
                if( inv.has_quality( quality ) ) {
                    has_tools = true;
                    break;
                }
            }
            if( !has_tools ) {
                return false;
            }
        } else {
            // If the material is not in the dictionary, we can't salvage item.
            return false;
        }
    }
    return true;
}

void salvage_activity_actor::start( player_activity &act, Character &who )
{
    map &here = get_map();
    auto inv = who.crafting_inventory();
    for( auto &target : targets ) {
        if( !target.loc ) {
            debugmsg( "Lost target of ACT_DISASSEMBLY" );
            act.set_to_null();
        } else {
            progress.emplace( target.loc->tname( target.loc->count() ), time_to_cut_up( *target.loc ) );
        }
    }
}

void salvage_activity_actor::do_turn( player_activity &act, Character &who )
{
    if( progress.front().complete() ) {
        iuse_location &target = targets.front();
        if( !target.loc ) {
            debugmsg( "Lost target of ACT_DISASSEMBLY" );
            act.set_to_null();
        } else {
            cut_up( who, *target.loc );
        }
        targets.erase( targets.begin() );
        progress.pop();

        if( !progress.empty() ) {
            target = targets.front();
            if( !target.loc ) {
                debugmsg( "Lost target of ACT_DISASSEMBLY" );
                act.set_to_null();
            } else {
                if( try_salvage( who, *target.loc ) ) {
                    recalc_all_moves( act, who );
                } else {
                    act.set_to_null();
                }
            }
        }
    }
}

void salvage_activity_actor::finish( player_activity &act, Character &who )
{
    if( !progress.complete() ) {
        debugmsg( "salvage_activity_actor call finish function while able to start new salvage" );
    }
    add_msg( _( "You finish salvaging." ) );
    act.set_to_null();
}
