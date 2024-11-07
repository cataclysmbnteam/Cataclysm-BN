/**
 * @module
 *
 * Extracts all item prices from given path recursively.
 */

import { z } from "$catjazz/deps/zod.ts"
import { queryCli } from "$catjazz/mod.ts"
import { type Currency, toCents } from "$catjazz/units/mod.ts"
import { type Column, stringify } from "https://deno.land/std@0.217.0/csv/stringify.ts"

const parseCurrency = z.string().transform((c) => toCents(c as Currency))

const schema = z.object({
  id: z.string(),
  price_postapoc: parseCurrency,
})

const columns: Column[] = ["id", "price_postapoc"]

if (import.meta.main) {
  const main = queryCli({
    desc: "item prices",
    schema,
    map: (xs) => {
      console.table(xs)
      const csv = stringify(xs, { columns })
      return csv
    },
  })

  await main().parse(Deno.args)
}
