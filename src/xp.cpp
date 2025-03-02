#include "xp.h"

#include <algorithm>
#include "avatar.h"
#include "character.h"
#include "skill.h"

namespace xp
{
int total( const Character &c )
{
    if( !c.is_avatar() ) {
        // @todo Let NPCs gain xp somehow
        return 0;
    }
    const avatar &u = *c.as_avatar();
    return u.kill_xp() + u.get_bonus_xp();
}

int available( const Character &c )
{
    return total( c ) - for_skill_set( c.get_all_skills() );
}

int spent( const Character &c )
{
    return for_skill_set( c.get_all_skills() );
}

int for_skill_set( const std::map<skill_id, SkillLevel> skills )
{
    int total_skills = std::accumulate( skills.begin(), skills.end(), 0,
    []( int acc, const std::pair<const skill_id, SkillLevel> &pr ) {
        return acc + pr.second.level();
    } );
    int total_pow4_skills = std::accumulate( skills.begin(), skills.end(), 0,
    []( int acc, const std::pair<const skill_id, SkillLevel> &pr ) {
        int lvl = pr.second.level();
        return acc + lvl * lvl * lvl * lvl;
    } );

    return 10 * total_skills + total_skills * total_skills + total_pow4_skills;
}

int to_raise_skill( const Character &c, const skill_id &skill )
{
    const auto &skills = c.get_all_skills();

    auto skillset_copy = skills;
    skillset_copy[skill].level( skillset_copy[skill].level() + 1 );

    return for_skill_set( skillset_copy ) - for_skill_set( skills );
}
} // namespace xp
