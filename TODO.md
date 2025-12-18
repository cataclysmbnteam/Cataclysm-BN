# NPC Normalization - Deferred Items

This document tracks normalization items that are deferred due to performance concerns or requiring significant AI improvements.

## Messaging and Observability

### NPC Combat Messages
**Status**: Incomplete - Current changes only normalize mechanics, not player visibility

**Current State**:
- NPCs now mechanically suffer from pain/stamina effects (stat penalties, accuracy reduction, etc.)
- However, these effects are NOT communicated to the player observing NPCs
- Miss reasons (e.g., "Your pain distracts you!") only show for player, not for NPCs
- The player can't see when an NPC is affected by pain, exhaustion, hunger, etc.

**Why Incomplete**:
- The miss reason system uses a single string format ("Your X"), not player/NPC pairs
- All miss reason display code is guarded by `if( is_player() )` (melee.cpp:497)
- Proper implementation would require:
  1. Extending miss reason system to support both formats: "Your pain distracts you!" / "The pain distracts <npcname>!"
  2. Removing `is_player()` guards and showing NPC combat messages to observers
  3. Updating all ~20 existing miss reason strings to support both formats
  4. Similar work for other combat/activity messages

**Examples of Missing Messages**:
- NPC misses due to pain → Player sees nothing
- NPC stops pulping corpse due to low stamina → No message
- NPC's clothing restricts movement → Not communicated
- NPC is too weak from hunger → Silent

**Future Implementation**:
- Refactor miss reason system to use `add_msg_player_or_npc()` pattern
- Add visibility checks: only show NPC messages if player can see the NPC
- Update all combat and activity messages to support NPC format
- This is a significant refactor touching melee, ranged, and activity systems

## Performance-Heavy Systems

### Hunger/Thirst Effects
**Status**: Deferred - High computational cost for many NPCs

Currently NPCs track hunger/thirst but these don't affect stats/speed like they do for players.

**Why Deferred**:
- Requires calculating nutrition/hydration effects every turn for every NPC
- Complex metabolic calculations (BMR, activity multipliers, etc.)
- With 20+ NPCs, this could significantly impact frame rate

**Future Implementation**:
- Profile current hunger/thirst system overhead
- Consider simplified hunger/thirst model for NPCs
- Or: Only calculate for NPCs near player (LoS + 1 bubble)

### Fatigue/Sleep System
**Status**: Deferred - Requires AI rework

NPCs don't track fatigue or need to sleep. Players get stat penalties when tired.

**Why Deferred**:
- NPCs need AI to decide when to sleep
- Need bed-seeking behavior
- Sleep scheduling conflicts with guard duty, activities
- Edge cases: sleeping during combat, waking up, etc.

**Future Implementation**:
- Add fatigue tracking to NPC turn processing
- Implement sleep need detection
- Add sleep activity and location finding
- Handle wake-up triggers (noise, danger, etc.)

### Morale System Effects
**Status**: Needs investigation

NPCs have morale but it may not affect stats like it does for players.

**Why Deferred**:
- Need to verify current behavior
- Complex interactions with focus, crafting speed, etc.
- Performance implications unclear

**Future Implementation**:
- Audit morale effect application
- Profile performance impact
- Implement if lightweight enough

## AI Improvements Needed

### Better Stamina Management
**Priority**: High - Needed after stamina normalization

Once NPCs consume stamina, they need AI to avoid exhaustion.

**Needed Features**:
- Detect low stamina (< 33%?)
- Prefer walking over running when stamina is low
- Take breaks during extended activities
- Combat tactics: fall back when exhausted
- Avoid starting stamina-heavy activities when low

### Improved Painkiller Usage
**Priority**: Medium

NPCs use painkillers at pain >= 15, but need smarter logic.

**Improvements**:
- Consider painkiller strength vs pain level
- Avoid wasting strong painkillers on minor pain
- Prioritize painkillers before combat
- Track painkiller cooldowns better

### Rest and Recovery Behavior
**Priority**: Medium - Tied to stamina management

NPCs need to actively recover stamina/HP.

**Needed Features**:
- Detect when rest is needed
- Find safe locations to rest
- Rest activity that recovers stamina faster
- Balance rest vs mission objectives

### Combat Tactics Adjustment
**Priority**: Medium

With pain/stamina affecting NPCs, combat AI needs updates.

**Improvements**:
- Retreat when badly hurt (high pain)
- Avoid melee when low stamina
- Request healing/painkillers from allies
- Group tactics: protect injured members

### Movement and Activity Planning
**Priority**: Low

NPCs should plan routes and activities considering stamina.

**Improvements**:
- Estimate stamina cost of planned route
- Choose walking vs running based on urgency + stamina
- Break long activities into chunks with rest
- Avoid rapid direction changes (stamina drain)

## Missing Features for NPCs

### Fungal Infection
**File**: `src/monattack.cpp` (lines 1707, 1740)

NPCs can't be infected by fungal spores. Comment: "TODO: Infect NPCs?"

**Blocker**: Need infection progression logic, AI response, cure-seeking behavior

### Siphoning
**File**: `src/character_functions.cpp` (line 102)

NPCs can't siphon fuel from vehicles. `debugmsg("Siphoning not implemented for NPCs.")`

**Blocker**: UI interaction vs autonomous action

### NPCs Assisting Other NPCs
**File**: `src/character_functions.cpp` (line 804)

NPCs can assist player in crafting but not each other. Comment: "TODO: NPCs assisting other NPCs"

**Blocker**: NPC-to-NPC interaction framework needed

### Vehicle Boarding/Controls
Multiple TODOs about NPCs using vehicles properly

**Blocker**: Complex vehicle interaction system needs NPC AI integration

## Class Hierarchy Refactoring

**Status**: Deferred - Large architectural change

Currently: `npc` inherits from `player` which inherits from `Character`

**Issues**:
- Creates coupling between NPC and player-specific code
- Message routing methods are main divergence
- Mixing UI concerns with game logic

**Ideal Structure**:
```
Character (base)
├── avatar (player UI + Character)
└── npc (AI + Character)
```

**Why Deferred**:
- Very invasive change (touches hundreds of files)
- High risk of breaking existing functionality
- Should be separate effort after normalization

**Future Work**:
- Move player UI code to avatar class
- Extract message routing strategy pattern
- Make npc inherit directly from Character
- Extensive testing required

## Notes

Performance measurements should be taken after normalization changes to identify actual bottlenecks vs assumed ones. The original "NPCs feel no pain" optimization may no longer be necessary on modern hardware.
