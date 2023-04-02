import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { LegacyGunMod, NewGunMod } from "../types.ts"
import { migrateToGunMod } from "../gunmod.ts"

const scopeBefore = {
  "id": "rifle_scope",
  "type": "GUNMOD",
  "name": { "str": "rifle scope" },
  "//": "Leupold Mark 6",
  "description":
    "A 3-18x44 rifle scope.  It is adjustable for windage and elevation in 1/10th mrad increments and is remarkably small and light for its magnification.",
  "weight": "669 g",
  "volume": "435 ml",
  "price": 68000,
  "price_postapoc": 750,
  "material": ["aluminum", "glass"],
  "symbol": ":",
  "color": "dark_gray",
  "location": "sights",
  "mod_targets": ["rifle", "crossbow", "launcher"],
  "install_time": "30 m",
  "sight_dispersion": 0,
  "aim_speed": 0,
  "flags": ["DISABLE_SIGHTS", "ZOOM"],
} as LegacyGunMod

const scopeAfter = {
  "id": "rifle_scope",
  "type": "GUNMOD",
  "name": { "str": "rifle scope" },
  "//": "Leupold Mark 6",
  "description":
    "A 3-18x44 rifle scope.  It is adjustable for windage and elevation in 1/10th mrad increments and is remarkably small and light for its magnification.",
  "weight": "669 g",
  "volume": "435 ml",
  "price": 68000,
  "price_postapoc": 750,
  "material": ["aluminum", "glass"],
  "symbol": ":",
  "color": "dark_gray",
  "location": "sights",
  "mod_target_category": [["RIFLES"], ["XBOWS"], ["GRENADE_LAUNCHERS"], [
    "ROCKET_LAUNCHERS",
  ]],
  "install_time": "30 m",
  "sight_dispersion": 0,
  "aim_speed": 0,
  "flags": ["DISABLE_SIGHTS", "ZOOM"],
} as NewGunMod

Deno.test("{rifle_scope} replaces all {mod_target} to {mod_target_category}", () => {
  assertEquals(migrateToGunMod(scopeBefore), scopeAfter)
})
