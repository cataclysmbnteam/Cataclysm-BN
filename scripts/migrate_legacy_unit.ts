import { baseCli } from "https://deno.land/x/catjazz@v0.0.1/utils/cli.ts"
import {
  fromLegacyVolume,
  fromLegacyWeight,
} from "https://deno.land/x/catjazz@v0.0.1/utils/units.ts"
import { z } from "https://deno.land/x/catjazz@v0.0.1/deps/zod.ts"
import { match } from "https://deno.land/x/catjazz@v0.0.1/deps/ts_pattern.ts"

const desc = "Migrates Legacy units into new literal format."

const int = z.number().int()

export type Currency = `${number} ${"cent" | "USD" | "kUSD"}`

const usd = 100
const kusd = 1_000 * usd

/**
 * converts legacy cent to new currency format.
 * @param c legacy cent (`1 unit` = `1 cent`)
 * @return cent or USD or kUSD
 */
export const fromLegacyCurrency = (c: number): Currency =>
  match(c)
    .when((c) => c % kusd === 0, () => `${c / kusd} kUSD` as const)
    .when((c) => c % usd === 0, () => `${c / usd} USD` as const)
    .otherwise(() => `${c} cent` as const)

export const migrate = <const T>(fromLegacy: (x: number) => T) =>
  int.transform(fromLegacy).optional()

const migrateWeight = migrate(fromLegacyWeight)
const migrateVolume = migrate(fromLegacyVolume)
const migrateCurrency = migrate(fromLegacyCurrency)

const schema = z
  .object({
    volume: migrateVolume,
    integral_volume: migrateVolume,
    min_pet_vol: migrateVolume,
    max_pet_vol: migrateVolume,
    barrel_length: migrateVolume,

    weight: migrateWeight,
    integral_weight: migrateWeight,
    weight_capacity_bonus: migrateWeight,

    price: migrateCurrency,
    price_postapoc: migrateCurrency,
  })
  .passthrough()

const main = baseCli({ desc, schema })

if (import.meta.main) {
  await main().parse(Deno.args)
}
