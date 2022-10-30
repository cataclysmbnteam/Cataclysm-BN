#include "construction_sequence.h"

#include "construction.h"
#include "generic_factory.h"
#include "consistency_report.h"
#include "mapdata.h"
#include "json.h"
#include "type_id_implement.h"

namespace
{

generic_factory<construction_sequence> all_sequences( "construction sequence" );

std::map<ter_str_id, construction_sequence_int_id> sequences_for_ter;
std::map<furn_str_id, construction_sequence_int_id> sequences_for_furn;

} // namespace

IMPLEMENT_STRING_AND_INT_IDS( construction_sequence, all_sequences )

void construction_sequence::load( const JsonObject &jo, const std::string & )
{
    mandatory( jo, was_loaded, "elems", elems );
    optional( jo, was_loaded, "post_terrain", post_terrain );
    optional( jo, was_loaded, "post_furniture", post_furniture );
    optional( jo, was_loaded, "blacklisted", blacklisted );
}

void construction_sequence::check() const
{
    consistency_report report;
    for( const construction_str_id &elem : elems ) {
        if( !elem.is_valid() ) {
            report.warn( "invalid construction id \"%s\"", elem );
        }
    }
    if( !report.is_empty() ) {
        std::string s = report.format( "construction sequence", id.str() );
        debugmsg( s );
    }
}

namespace construction_sequences
{

void load( const JsonObject &jo, const std::string &src )
{
    all_sequences.load( jo, src );
}

void reset()
{
    all_sequences.reset();
    sequences_for_ter.clear();
    sequences_for_furn.clear();
}

static bool is_valid_first_step( const construction &c )
{
    static const ter_str_id t_dirt( "t_dirt" );

    return ( c.pre_terrain.is_empty() || c.pre_terrain == t_dirt ) &&
           c.pre_furniture.is_empty() &&
           c.pre_special_is_valid_for_dirt &&
    std::all_of( c.pre_flags.begin(), c.pre_flags.end(), []( const std::string & flag ) {
        return t_dirt->has_flag( flag );
    } );
}

static bool has_valid_result( const construction &c )
{
    return !c.post_furniture.is_empty() || !c.post_terrain.is_empty();
}

static bool has_valid_category( const construction &c )
{
    static const construction_category_id construction_category_REPAIR( "REPAIR" );
    return c.category != construction_category_REPAIR;
}

void finalize()
{
    all_sequences.finalize();

    // Validate explicit sequences
    for( const construction_sequence &seq : all_sequences.get_all() ) {
        if( seq.blacklisted ) {
            continue;
        }
        if( !seq.post_furniture.is_empty() && !seq.post_furniture.is_valid() ) {
            debugmsg( R"(Construction sequence "%s" defines invalid post_furniture "%s".)",
                      seq.id, seq.post_furniture );
            continue;
        }
        if( !seq.post_terrain.is_empty() && !seq.post_terrain.is_valid() ) {
            debugmsg( R"(Construction sequence "%s" defines invalid post_terrain "%s".)",
                      seq.id, seq.post_terrain );
            continue;
        }
        if( seq.post_furniture.is_empty() == seq.post_terrain.is_empty() ) {
            debugmsg( R"(Construction sequence "%s" must define post_furniture OR post_terrain.)",
                      seq.id );
            continue;
        }
        if( !seq.elems.empty() ) {
            bool constr_ok = true;
            for( const construction_str_id &c_id : seq.elems ) {
                if( !c_id.is_valid() ) {
                    debugmsg( R"(Construction sequence "%s" contains invalid construction id "%s".)",
                              seq.id, c_id );
                    constr_ok = false;
                    break;
                }
                if( c_id->is_blacklisted() ) {
                    debugmsg( R"(Construction sequence "%s" contains blacklisted construction id "%s".)",
                              seq.id, c_id );
                    constr_ok = false;
                    break;
                }
                if( !has_valid_category( *c_id ) ) {
                    debugmsg( R"(Construction sequence "%s" should no contain constructions from REPAIR category ("%s").)",
                              seq.id, c_id );
                    constr_ok = false;
                    break;
                }
                if( !has_valid_result( *c_id ) ) {
                    debugmsg( R"(In construction sequence "%s" step "%s" produces no furniture or terrain.)",
                              seq.id, c_id
                            );
                    constr_ok = false;
                    break;
                }
            }
            if( !constr_ok ) {
                continue;
            }
            const construction &first = *seq.elems.front();
            if( !is_valid_first_step( first ) ) {
                debugmsg( R"(First entry "%s" in construction sequence "%s" )"
                          R"(must have t_dirt as valid prerequisite.)",
                          first.id, seq.id );
                continue;
            }
            const construction &last = *seq.elems.back();
            if( last.post_terrain != seq.post_terrain || last.post_furniture != seq.post_furniture ) {
                debugmsg( R"(Last entry "%s" in construction sequence "%s" )"
                          R"(must produce terrain or furniture specified by the sequence.)",
                          last.id, seq.id );
            }
            for( size_t i = 1; i < seq.elems.size(); i++ ) {
                const construction &c1 = *seq.elems[i - 1];
                const construction &c2 = *seq.elems[i];

                if( c1.post_furniture != c2.pre_furniture ) {
                    debugmsg( R"(In construction sequence "%s" post_furniture and pre_furniture )"
                              R"(don't match when going from "%s" to "%s".)",
                              seq.id, c1.id, c2.id
                            );
                    constr_ok = false;
                    break;
                }
                if( c1.post_terrain != c2.pre_terrain ) {
                    debugmsg( R"(In construction sequence "%s" post_terrain and pre_terrain )"
                              R"(don't match when going from "%s" to "%s".)",
                              seq.id, c1.id, c2.id
                            );
                    constr_ok = false;
                    break;
                }
            }
            if( !constr_ok ) {
                continue;
            }
        }
        cata::optional<construction_sequence_id> conflict_id;
        if( !seq.post_terrain.is_empty() ) {
            auto res = sequences_for_ter.emplace( seq.post_terrain, seq.id.id() );
            if( !res.second ) {
                conflict_id = res.first->second.id();
            }
        } else {
            auto res = sequences_for_furn.emplace( seq.post_furniture, seq.id.id() );
            if( !res.second ) {
                conflict_id = res.first->second.id();
            }
        }
        if( conflict_id ) {
            debugmsg( R"(Construction sequence "%s" defines same result as another existing sequence "%s".)",
                      seq.id, *conflict_id
                    );
        }
    }

    // Generate auto 1-element sequences
    std::map<ter_str_id, std::vector<construction_sequence>> ter_map;
    std::map<furn_str_id, std::vector<construction_sequence>> furn_map;
    for( const construction_id &c_id : constructions::get_all_sorted() ) {
        const construction &c = *c_id;
        if( !is_valid_first_step( c ) || !has_valid_result( c ) || !has_valid_category( c ) ) {
            // Recipe must be a valid first construction step to be turned into sequence
            continue;
        }
        std::vector<construction_sequence> *seq_vec = nullptr;
        if( !c.post_terrain.is_empty() ) {
            auto it = ter_map.emplace( c.post_terrain, std::vector<construction_sequence>() );
            seq_vec = &it.first->second;
        } else {
            auto it = furn_map.emplace( c.post_furniture, std::vector<construction_sequence>() );
            seq_vec = &it.first->second;
        }

        construction_sequence seq;
        seq.was_loaded = true;
        seq.id = construction_sequence_id( "#auto#_" + c.id.str() );
        seq.elems.push_back( c.id );
        seq_vec->push_back( seq );
    }
    for( const auto &it : ter_map ) {
        if( it.second.size() != 1 ) {
            // Don't autogenerate sequence if multiple constructions
            // can be turned into a sequence, let user solve conflict.
            continue;
        }
        // Don't autogenerate sequence if there is already an explicitly defined sequence
        if( lookup_sequence( it.first ) ) {
            continue;
        }
        const construction_sequence &ref = all_sequences.insert( it.second.front() );
        sequences_for_ter.emplace( it.first, ref.id.id() );
    }
    for( const auto &it : furn_map ) {
        if( it.second.size() != 1 ) {
            // Don't autogenerate sequence if multiple constructions
            // can be turned into a sequence, let user solve conflict.
            continue;
        }
        // Don't autogenerate sequence if there is already an explicitly defined sequence
        if( lookup_sequence( it.first ) ) {
            continue;
        }
        const construction_sequence &ref = all_sequences.insert( it.second.front() );
        sequences_for_furn.emplace( it.first, ref.id.id() );
    }
}

void check_consistency()
{
    for( const construction_sequence &seq : all_sequences.get_all() ) {
        seq.check();
    }
}

const construction_sequence *lookup_sequence( const ter_str_id &id )
{
    auto it = sequences_for_ter.find( id );
    return it == sequences_for_ter.end() ? nullptr : &*it->second;
}

const construction_sequence *lookup_sequence( const furn_str_id &id )
{
    auto it = sequences_for_furn.find( id );
    return it == sequences_for_furn.end() ? nullptr : &*it->second;
}

} // namespace construction_sequences
