import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

/** schema to parse items containing legacy gunmod. */
export const legacyGunMod = z.object({
  /** itype_id and category mixed. */
  mod_targets: z.array(z.string()),
}).passthrough()
export type LegacyGunMod = Readonly<z.infer<typeof legacyGunMod>>

/** item should satisfy all the following categories to be able to accept this gunmod. */
type RequiredCategory = Uppercase<string>[]

/** gunmod with categories extracted from `mod_targets`. */
export type NewGunMod = Readonly<{
  /** list of itype_id that this gunmod can be applied to. */
  mod_targets?: string[]

  /** gunmod can be applied to items satisfying one of the following {@link RequiredCategory}. */
  mod_target_category?: RequiredCategory[]
}>

export type MigrateToGunMod = (gunmod: LegacyGunMod) => NewGunMod
