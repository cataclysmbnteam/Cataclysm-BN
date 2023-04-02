import { LegacyModTargets, MigrateToGunMod, RequiredCategory } from "./types.ts"
import { match } from "npm:ts-pattern"
import { mapValues } from "https://deno.land/std@0.182.0/collections/map_values.ts"
import { filterValues } from "https://deno.land/std@0.182.0/collections/filter_values.ts"
import "npm:@total-typescript/ts-reset"

/**
 * `"mod_targets"` needs to have "pistol", "smg", "rifle", "shotgun", "crossbow", "archery" and "launcher" removed.
 *  Everything else must stay.
 * `"mod_target_category"` should receive these instead
 *
 *    "pistol" -> [ "PISTOLS" ]
 *    "smg" -> [ "SUBMACHINE_GUNS" ]
 *    "rifle" ->  [ "RIFLES"  ]
 *    "shotgun" -> [ "SHOTGUNS" ]
 *    "archery" -> [ "BOWS" ]
 *    "launcher" -> [[ "GRENADE_LAUNCHERS" ], [ "ROCKET_LAUNCHERS" ]]
 *
 *    "crossbow" needs to be transformed in a very specific way.
 *    ["pistol", "crossbow"] -> [ "H_XBOWS", "XBOWS" ]
 *    ["crossbow"] -> [ "XBOWS" ]
 */
const legacyCategory = [
  "pistol",
  "smg",
  "rifle",
  "shotgun",
  "crossbow",
  "archery",
  "launcher",
] as const
type LegacyCategory = typeof legacyCategory[number]

const extractCategory = (targets: string[]) => targets.filter((t) => !legacyCategory.includes(t))

type MigrateGen = (targets: LegacyModTargets) => RequiredCategory[]
// deno-fmt-ignore
type MigrateCategoryFn = (subarray: LegacyModTargets) => (fn: MigrateGen) => (legacyMods: LegacyModTargets) => RequiredCategory[]
const migrateCategory: MigrateCategoryFn = (subarray) => (fn) => (legacyMods) => {
  const subarrayMatches = legacyMods.filter((t) => subarray.includes(t))

  return subarrayMatches.length > 0 ? fn(subarrayMatches) : []
}

type MigrateCategory = (legacyMods: LegacyModTargets) => RequiredCategory[]
export const migrateCategories = {
  pistol: migrateCategory(["pistol"])(() => [["PISTOLS"]]),
  smg: migrateCategory(["smg"])(() => [["SUBMACHINE_GUNS"]]),
  rifle: migrateCategory(["rifle"])(() => [["RIFLES"]]),
  shotgun: migrateCategory(["shotgun"])(() => [["SHOTGUNS"]]),
  crossbow: migrateCategory(["pistol", "crossbow"])((targets) =>
    match(targets)
      .with(["pistol", "crossbow"], () => [["H_XBOWS"], ["XBOWS"]])
      .with(["crossbow"], () => [["XBOWS"]])
      .otherwise(() => [])
  ),
  archery: migrateCategory(["archery"])(() => [["BOWS"]]),
  launcher: migrateCategory(["launcher"])(() => [["GRENADE_LAUNCHERS"], ["ROCKET_LAUNCHERS"]]),
} satisfies Record<LegacyCategory, MigrateCategory>

export const migrateToGunMod: MigrateToGunMod = (gunmod) => {
  const { mod_targets: legacyModTargets } = gunmod
  const mod_targets = extractCategory(legacyModTargets ?? [])
  const mod_target_category = Object.values(migrateCategories)
    .flatMap((m) => m(legacyModTargets))

  const newGunMods = mapValues(
    { mod_targets, mod_target_category },
    (value) => value.length > 0 ? value : undefined,
  )
  const filtered = filterValues(
    { ...gunmod, ...newGunMods },
    (value) => value !== undefined,
  )
  return filtered
}
