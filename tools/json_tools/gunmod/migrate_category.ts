import "npm:@total-typescript/ts-reset"
import { LegacyModTargets, RequiredCategory } from "./types.ts"

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
export type LegacyCategory = typeof legacyCategory[number]

export const extractCategory = (targets: string[]) =>
  targets.filter((t) => !legacyCategory.includes(t))

type MigrateGen = (targets: LegacyModTargets) => RequiredCategory[]

// deno-fmt-ignore
type MigrateCategoryFn = (subarray: LegacyModTargets) => (fn: MigrateGen) => (legacyMods: LegacyModTargets) => RequiredCategory[];
export const migrateCategory: MigrateCategoryFn = (subarray) => (fn) => (legacyMods) => {
  const subarrayMatches = legacyMods.filter((t) => subarray.includes(t))

  return subarrayMatches.length > 0 ? fn(subarrayMatches) : []
}
