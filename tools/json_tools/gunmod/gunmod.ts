import { MigrateToGunMod } from "./types.ts"
import { mapValues } from "https://deno.land/std@0.182.0/collections/map_values.ts"
import { filterValues } from "https://deno.land/std@0.182.0/collections/filter_values.ts"
import { extractCategory } from "./migrate_category.ts"
import { migrateCategories } from "./migrate_categories.ts"

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
