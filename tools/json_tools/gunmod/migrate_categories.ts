import { LegacyModTargets, RequiredCategory } from "./types.ts"
import { match } from "npm:ts-pattern"
import { LegacyCategory, migrateCategory } from "./migrate_category.ts"

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
