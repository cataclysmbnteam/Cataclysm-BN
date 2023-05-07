import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { match } from "npm:ts-pattern"

export const weight = z.custom<`${number} ${WeightUnits}`>((x) => /^\d+ (m|k)?g$/.test(x as string))
export const volume = z.custom<`${number} ${VolumeUnits}`>((x) => /^\d+ (ml|L)$/.test(x as string))

export type WeightUnits = "mg" | "g" | "kg"
export type Weight = z.infer<typeof weight>
export type VolumeUnits = "ml" | "L"
export type Volume = z.infer<typeof volume>

/**
 * converts legacy weight to new weight format.
 * @param g legacy weight (`1 unit` = `1g`)
 * @return g or kg
 */
export const fromLegacyWeight = (g: number): Weight =>
  match(g % 1000)
    .with(0, () => `${g / 1000} kg` as const)
    .otherwise(() => `${g} g` as const)

/**
 * converts legacy volume to new volume format.
 * @param x legacy volume (`1 unit` = `250 mL`)
 * @return ml or L
 */
export const fromLegacyVolume = (x: number): Volume => {
  const ml = 250 * x
  return match(ml % 1000)
    .with(0, () => `${ml / 1000} L` as const)
    .otherwise(() => `${ml} ml` as const)
}
