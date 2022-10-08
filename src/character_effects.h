#pragma once
#ifndef CATA_SRC_CHARACTER_EFFECTS_H
#define CATA_SRC_CHARACTER_EFFECTS_H

class Character;

struct stat_mod {
    int strength = 0;
    int dexterity = 0;
    int intelligence = 0;
    int perception = 0;

    int speed = 0;
};

namespace character_effects
{

/** Returns the effect of pain on stats */
auto get_pain_penalty( const Character &ch ) -> stat_mod;

/** Returns the penalty to speed from starvation */
auto get_kcal_speed_penalty( float kcal_percent ) -> int;

/** Returns the penalty to speed from thirst */
auto get_thirst_speed_penalty( int thirst ) -> int;

} // namespace character_effects

#endif // CATA_SRC_CHARACTER_EFFECTS_H
