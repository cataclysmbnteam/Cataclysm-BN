import { baseCli, cliOptions } from "$catjazz/utils/cli.ts"
import { fromLegacyCurrency, fromLegacyVolume, fromLegacyWeight } from "$catjazz/units/mod.ts"
import { z } from "$catjazz/deps/zod.ts"
import { Command } from "$catjazz/deps/cliffy.ts"
import { timeit } from "$catjazz/utils/timeit.ts"
import { applyRecursively, schemaTransformer } from "$catjazz/utils/transform.ts"
import { fmtJsonRecursively } from "$catjazz/utils/json_fmt.ts"
import { CataEntry, Entry, parseCataJson, readRecursively } from "$catjazz/utils/parse.ts"
import { match, P } from "$catjazz/deps/ts_pattern.ts"
import { id } from "$catjazz/utils/id.ts"

const desc = "Migrates Legacy units into new literal format."

const int = z.number().int()

export type Currency = `${number} ${"cent" | "USD" | "kUSD"}`

export const migrate = <const T>(fromLegacy: (x: number) => T) =>
  z.union([int.transform(fromLegacy), z.string()]).optional()

const unpack = (xs: string[] | Entry[]) =>
  match(xs)
    .with(P.array(P.string), id)
    .otherwise((xs) => xs.map(({ path }) => path))

// const migrateWeight = migrate(fromLegacyWeight)
const migrateVolume = migrate(fromLegacyVolume)
// const migrateCurrency = migrate(fromLegacyCurrency)

const base = z
  .object({
    // GUN
    barrel_length: migrateVolume,

    armor_data: z.object({ storage: migrateVolume }).passthrough().optional(),

    storage: migrateVolume,

    max_pet_vol: migrateVolume,
    min_pet_vol: migrateVolume,

    contains: migrateVolume,

    volume: migrateVolume,
    integral_volume: migrateVolume,
    magazine_well: migrateVolume,

    filthy_volume_threshold: migrateVolume,

    max_volume: migrateVolume,
    min_volume: migrateVolume,
    use_action: z.object({
      max_volume: migrateVolume,
      min_volume: migrateVolume,
      filthy_volume_threshold: migrateVolume,
    }).passthrough().optional(),

    size: migrateVolume,
    // TODO: burn_data[].volume_per_turn

    // weight: migrateWeight,
    // integral_weight: migrateWeight,
    // weight_capacity_bonus: migrateWeight,
    // price: migrateCurrency,
    // price_postapoc: migrateCurrency,
  })
  .passthrough()

const vehiclePart = z.object({
  type: z.literal("vehicle_part"),
  folded_volume: migrateVolume,
  size: migrateVolume,
})
  .passthrough()

const schema = z.union([base, vehiclePart])

// const schema = z.object({}).passthrough()

const main = () =>
  new Command()
    .option(...cliOptions.path)
    .option(...cliOptions.quiet)
    .option(...cliOptions.format)
    .description(desc)
    .action(async ({ path, format, quiet = false }) => {
      const timeIt = timeit(quiet)

      const transformer = schemaTransformer(schema)
      const ignore = (entries: CataEntry[]) =>
        entries.find(({ type }) =>
          ["speech", "sound_effect", "mapgen", "palette", "faction", "mod_tileset"].includes(type)
        )

      const mapgenIgnoringTransformer = (text: string) => {
        const entries = parseCataJson(text)
        return ignore(entries) ? text : JSON.stringify(transformer(entries), null, 2)
      }

      const recursiveTransformer = applyRecursively(mapgenIgnoringTransformer)

      const entries = await timeIt({ name: "reading JSON", val: readRecursively(path) })

      await timeIt({ name: "transform", val: recursiveTransformer(entries) })

      if (!format) return
      await timeIt({
        name: "formatting",
        val: fmtJsonRecursively({ formatterPath: format, quiet: true })(unpack(entries)),
      })
    })

if (import.meta.main) {
  await main().parse(Deno.args)
}
