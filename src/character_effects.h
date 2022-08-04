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
stat_mod get_pain_penalty( const Character &ch );

/** Returns the penalty to speed from starvation */
int get_kcal_speed_penalty( float kcal_percent );

/** Returns the penalty to speed from thirst */
int get_thirst_speed_penalty( int thirst );

} // namespace character_effects

#endif // CATA_SRC_CHARACTER_EFFECTS_H
