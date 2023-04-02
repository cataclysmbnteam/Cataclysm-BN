import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { LegacyGunMod, NewGunMod } from "../types.ts"
import { migrateToGunMod } from "../gunmod.ts"

const gripBefore = {
  "id": "light_grip",
  "type": "GUNMOD",
  "name": { "str": "lightweight replacement furniture" },
  "description":
    "A set of lightweight composite grips and furniture that reduces a firearm's weight, and as a consequence, its handling and melee damage.",
  "weight": "140 g",
  "volume": "250 ml",
  "integral_volume": "0 ml",
  "price": 48000,
  "price_postapoc": 750,
  "material": ["plastic"],
  "symbol": ":",
  "color": "dark_gray",
  "location": "grip",
  "mod_targets": ["pistol", "smg", "rifle", "shotgun", "crossbow", "launcher"],
  "handling_modifier": -5,
  "weight_multiplier": 0.75,
  "flags": ["REDUCED_BASHING"],
} as LegacyGunMod

const gripAfter = {
  "id": "light_grip",
  "type": "GUNMOD",
  "name": { "str": "lightweight replacement furniture" },
  "description":
    "A set of lightweight composite grips and furniture that reduces a firearm's weight, and as a consequence, its handling and melee damage.",
  "weight": "140 g",
  "volume": "250 ml",
  "integral_volume": "0 ml",
  "price": 48000,
  "price_postapoc": 750,
  "material": ["plastic"],
  "symbol": ":",
  "color": "dark_gray",
  "location": "grip",
  "mod_target_category": [
    ["PISTOLS"],
    ["SUBMACHINE_GUNS"],
    ["RIFLES"],
    ["SHOTGUNS"],
    ["H_XBOWS"],
    ["XBOWS"],
    ["GRENADE_LAUNCHERS"],
    ["ROCKET_LAUNCHERS"],
  ],
  "handling_modifier": -5,
  "weight_multiplier": 0.75,
  "flags": ["REDUCED_BASHING"],
} as NewGunMod

Deno.test("{light_grip} replaces all {mod_target} to {mod_target_category}", () => {
  assertEquals(migrateToGunMod(gripBefore), gripAfter)
})
