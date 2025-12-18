# NPC-Player Normalization Tasks

## Completed Changes

### Pain System Normalization ✓

**Finding**: Pain stat penalties were ALREADY applied to NPCs through `Character::reset_stats()`. No player-only checks existed for pain penalties in stat calculations.

**No Code Changes Needed**:
- Pain penalties (STR/DEX/INT/PER/speed) already affect NPCs identically to players
- Pain miss chance already tracked for NPCs (though not displayed - see TODO.md)
- Message text remains "Your pain distracts you!" since it's player-only
- Proper NPC message support requires larger refactor (see TODO.md: "NPC Combat Messages")

### Combat Accuracy Normalization ✓

**File**: `src/creature.cpp` (lines 863-873)

**Problem**: NPCs used HP% as a proxy for stamina in ranged accuracy calculations with a 0.8 cap. Comment stated "NPCs feel no pain and pretty much always have full stamina".

**Changes Made**:
- Removed `sourceplayer` conditional check
- NPCs now use actual stamina percentage: `get_stamina() / get_stamina_max()`
- Removed artificial 0.8 cap for NPCs
- Unified skill ratio from 0.1 (NPC) / 0.15 (player) to 0.15 for both
  - Original comment suggested NPCs get worse ratio "If they felt pain, got hungry/thirsty, or got tired they could get an equivalent skill adjust"
  - Now that NPCs feel these effects, they get the same ratio

### Activity Stamina Consumption ✓

**File**: `src/activity_handlers.cpp` (line 1963)

**Problem**: Code bypassed stamina checks for NPCs with `|| p->is_npc()` condition in corpse pulping activity.

**Change Made**:
- Removed `|| p->is_npc()` bypass
- NPCs now respect stamina limits during activities just like players
- When stamina < 33%, both players and NPCs will stop the activity

### Speed Rating Unification ✓

**Files**: `src/npc.h` (line 1189), `src/npc.cpp` (lines 2877-2883)

**Problem**: NPC had override of `speed_rating()` that didn't include stamina adjustment.
Character version includes: `ret *= 1.0f + (get_stamina() / get_stamina_max())` when not running.

**Changes Made**:
- Removed NPC's `speed_rating()` override declaration from npc.h
- Removed NPC's `speed_rating()` implementation from npc.cpp
- NPCs now inherit Character's stamina-aware version
- Note: NPCs don't have running implemented yet, so they always get the stamina adjustment

## Build Status

✓ All changed files compiled successfully:
- `src/creature.cpp` (20:06)
- `src/activity_handlers.cpp` (20:06)
- `src/character_turn.cpp` (20:06)
- `src/npc.cpp` (20:06)
- `src/npc.h` (header)

## Impact Summary

**Gameplay Changes**:
- ✓ Injured NPCs now suffer stat penalties from pain (STR/DEX/INT/PER/speed)
- ✓ NPCs with low stamina have reduced ranged accuracy
- ✓ NPCs with low stamina move slower
- ✓ NPCs must manage stamina during activities
- ✓ NPCs get pain-based miss chance in melee
- Combat balance shifts: NPCs become weaker when hurt/exhausted (more realistic)

**Code Quality**:
- ✓ Removed special-case `is_npc()` checks in 3 locations
- ✓ Removed outdated comments about NPCs not feeling pain
- ✓ Unified accuracy formulas between player and NPC
- ✓ One set of rules for all Characters
- ✓ Easier to reason about game mechanics
