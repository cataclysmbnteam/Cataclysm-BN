#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "activity_type.h"
#include "character_stat.h"
#include "crafting.h"
#include "type_id.h"

class Character;
class inventory;
class item;

using metric = std::pair<units::mass, units::volume>;

struct activity_reqs_adapter {
    std::vector<activity_req<quality_id>> qualities;
    std::vector<activity_req<skill_id>> skills;
    metric metrics = std::make_pair( 0_milligram, 0_ml );

    activity_reqs_adapter() = default;
    activity_reqs_adapter( const construction &con );
    activity_reqs_adapter( const recipe &rec, units::mass mass,
                           units::volume volume );
    activity_reqs_adapter( const std::vector<activity_req<quality_id>> &qualities,
                           const std::vector<activity_req<skill_id>> &skills, const metric &metrics )
        : qualities( qualities ), skills( skills ), metrics( metrics ) {
    }
};

using q_reqs = std::vector<activity_req<quality_id>>;
using stat_reqs = std::vector<activity_req<character_stat>>;
using stat_factors = std::vector<std::pair<character_stat, float>>;
using skill_reqs = std::vector<activity_req<skill_id>>;

using morale_factor_fn = std::function<float( const Character & )>;
using tools_factor_fn =
    std::function<float( const q_reqs &, const inventory & )>;
using stats_factor_fn = std::function<stat_factors( const Character &, const stat_reqs & )>;
using skills_factor_fn = std::function<float( const Character &, const skill_reqs & )>;
using bench_factor_fn = std::function<void( bench_loc &, const metric & )>;

/*
 * Struct to track activity speed by factors
*/
class activity_speed
{
    public:
        activity_id type = activity_id::NULL_ID();
        std::optional<bench_loc> bench;
        int assistant_count = 0;
        bench_factor_fn bench_factor_custom_formula = []( bench_loc &bench, const metric & ) {
            bench.wb_info.multiplier_adjusted = bench.wb_info.multiplier;
        };
        morale_factor_fn morale_factor_custom_formula = []( const Character & ) {
            return -1.f;
        };
        tools_factor_fn tools_factor_custom_formula = []( const q_reqs &, const inventory & ) {
            return -1.f;
        };
        stats_factor_fn stats_factor_custom_formula = []( const Character &, const stat_reqs & ) {
            return stat_factors{};
        };
        skills_factor_fn skills_factor_custom_formula = []( const Character &, const skill_reqs & ) {
            return -1.f;
        };



        float assist = 1.0f;
        float bench_factor = 1.0f;
        float player_speed = 1.0f;
        float skills = 1.0f;
        float tools = 1.0f;
        float morale = 1.0f;
        float light = 1.0f;
        std::vector<std::pair<character_stat, float>> stats;

        //Returns total product of all stats
        inline float stats_total() const {
            float acc = 1.0f;
            for( auto &stat : stats ) {
                acc *= stat.second;
            }
            return acc;
        }

        //Returns total product of all factors
        inline float total() const {
            return 1.0f * assist * bench_factor * player_speed * stats_total() * skills * tools * morale *
                   light;
        }

        //Returns total amonut of moves based on factors
        inline int total_moves() const {
            return std::roundf( total() * 100.0f );
        }

        //Calculates all factors
        void calc_all_moves( Character &who );
        void calc_all_moves( Character &who, activity_reqs_adapter &reqs );


        void calc_moves( const Character &who );

        void calc_assistants_factor( const Character &who );
        void calc_bench_factor( const Character &who );
        void find_best_bench( const tripoint &pos, metric metrics = std::make_pair( 0_milligram, 0_ml ) );
        void calc_light_factor( const Character &who );
        void calc_morale_factor( const Character &who );
        void calc_skill_factor( const Character &who,
                                const std::vector<activity_req<skill_id>> &skill_req );

        void calc_stats_factors( const Character &who );
        static std::pair<character_stat, float> calc_single_stat( const Character &who,
                const activity_req<character_stat> &stat );

        void calc_tools_factor( Character &who,
                                const std::vector<activity_req<quality_id>> &quality_reqs );
        static float get_best_qual_mod( const activity_req<quality_id> &q,
                                        const inventory &inv );
        static float calc_quality_factor( const activity_req<quality_id> &q, int q_level );
        static float calc_quality_factor( std::pair<quality_id, int> &q ) {
            return calc_quality_factor( activity_req<quality_id>( q.first ), q.second );
        }
        static float calc_quality_factor( std::pair<const quality_id, int> &q ) {
            return calc_quality_factor( activity_req<quality_id>( q.first ), q.second );
        }
};
