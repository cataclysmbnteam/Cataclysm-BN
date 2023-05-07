import { z } from "https://deno.land/x/zod@v3.20.5/index.ts"

import { baseCli } from "../cli.ts"
import { maybeAdd } from "../utils/maybe_add.ts"

const isRecoverN = (x: string) => x.startsWith("RECOVER_")
const findRecoverN = (xs: string[]) => xs.find(isRecoverN)

export const recoverNMigrationSchema = z.object({
  effects: z.string().array().refine(findRecoverN), // find only entry with effects that contain RECOVER_[N] element
})
  .passthrough() // keep all other fields
  .transform(({ effects, ...rest }) => {
    // rome-ignore lint/style/noNonNullAssertion: already checked by refine
    const recover = findRecoverN(effects)!
    const [_, n] = recover.split("_")
    const dont_recover_one_in = parseInt(n, 10)

    const filteredEffects = effects.filter((x) => !isRecoverN(x))
    return { ...rest, dont_recover_one_in, ...maybeAdd("effects")(filteredEffects) }
  })

const main = baseCli({
  desc: "Migrate all RECOVER_N effects to dont_recover_one_in.",
  schema: recoverNMigrationSchema,
})

if (import.meta.main) {
  await main()
}
