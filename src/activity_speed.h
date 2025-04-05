#pragma once
#ifndef _ACTIVITY_SPEED_H
#define _ACTIVITY_SPEED_H

#include <vector>

#include "character_stat.h"
#include "activity_type.h"


static const activity_id ACT_NULL = activity_id::NULL_ID();

struct bench_loc;
class Character;
class inventory;



struct activity_reqs_adapter {
    std::vector<activity_req<quality_id>> qualities;
    std::vector<activity_req<skill_id>> skills;

    activity_reqs_adapter( const recipe &rec );

    activity_reqs_adapter( const construction &con );
};

/*
 * Struct to track activity speed by factors
*/
class activity_speed
{
    public:
        const activity_id &type = ACT_NULL;
        std::optional<bench_loc> bench = std::nullopt;
        int assistant_count = 0;
        const std::function<float( const Character & )> morale_factor_custom_formula = [](
        const Character & ) {
            return -1.f;
        };

        const std::function<float( const std::vector<activity_req<quality_id>>&,
                                   const inventory & )> tools_factor_custom_formula = []( const std::vector<activity_req<quality_id>>
        &, const inventory & ) {
            return -1.f;
        };


        const std::function<std::vector<std::pair<character_stat, float>>( const Character &,
                const std::vector<activity_req<character_stat>>& )> stats_factor_custom_formula = [](
                            const Character &,
        const std::vector<activity_req<character_stat>> & ) {
            return std::vector<std::pair<character_stat, float>> {};
        };

        const std::function<float( const Character &,
                                   const std::vector<activity_req<skill_id>>& )> skills_factor_custom_formula = [](
                                               const Character &,
        const std::vector<activity_req<skill_id>> & ) {
            return -1.f;
        };



        float assist = 1.0f;
        float bench_factor = 1.0f;
        float player_speed = 1.0f;
        float skills = 1.0f;
        float tools = 1.0f;
        float morale = 1.0f;
        float light = 1.0f;
        std::vector<std::pair<character_stat, float>> stats = {};

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
};

#endif // CATA_SRC_PLAYER_ACTIVITY_H
