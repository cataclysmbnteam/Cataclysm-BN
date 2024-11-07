#include "regen.h"
#include "character.h"
#include "rng.h"

const flag_id flag_SPLINT( "SPLINT" );

namespace
{

/// Limb is broken without splint
auto has_broken_limb_penalty( const Character &c, const bodypart_id &bp ) -> bool
{
    return c.is_limb_broken( bp )
           && !c.worn_with_flag( flag_SPLINT, bp );
}

/// Broken limbs without splint heal slower up to 25%
auto mending_modifier( const Character &c ) -> float
{
    return clamp( c.mutation_value( "mending_modifier" ), 0.25f, 1.0f );
}

} // namespace

auto heal_adjusted( Character &c, const bodypart_id &bp, const int heal ) -> int
{
    const float broken_regen_mod = mending_modifier( c );
    const int broken_heal = roll_remainder( heal * broken_regen_mod );
    const bool is_broken = has_broken_limb_penalty( c, bp );
    const int actual_heal = is_broken ? broken_heal : heal;

    c.heal( bp, actual_heal );

    return actual_heal;
}
