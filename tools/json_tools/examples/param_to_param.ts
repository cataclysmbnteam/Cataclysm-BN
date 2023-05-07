import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { match, P } from "npm:ts-pattern"
import { fromLegacyVolume, fromLegacyWeight } from "../units.ts"
import { id } from "../utils/id.ts"

/** converts 123 -> "123 g"  */
export const mapWeightSchema = z.unknown().transform((x) =>
  match(x)
    .with(P.number, fromLegacyWeight)
    .otherwise(id)
)

/** converts "volume": 4 -> "1 L"  */
export const volumeMapSchema = z.unknown().transform((x) =>
  match(x)
    .with(P.number, fromLegacyVolume)
    .otherwise(id)
)

/** converts "name" -> { str_sp: "name" }  */
export const mapNameSchema = z.unknown().transform((x) =>
  match(x)
    .with(P.string, (str_sp) => ({ str_sp }))
    .otherwise(id)
)

/** converts "nanite" -> [ "nanite" ]  */
export const mapMaterialSchema = z.unknown().transform((x) =>
  match(x)
    .with(P.string, (str) => [str])
    .otherwise(id)
)

export const paramSchema = z.object({
  weight: mapWeightSchema,
  volume: volumeMapSchema,
  name: mapNameSchema,
  material: mapMaterialSchema,
}).partial().passthrough()

export type ParamSchema = z.infer<typeof paramSchema>
