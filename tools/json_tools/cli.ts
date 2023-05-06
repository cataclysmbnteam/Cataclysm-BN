/** Boilerplate to  */
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { readRecursively } from "./parse.ts"
import { applyRecursively, MigrationSchema, schemaMigrationTransformer } from "./transform.ts"
import { timeit } from "./timeit.ts"
import { fmtJsonRecursively } from "./json_fmt.ts"

type BaseCliOption = {
  desc: string
  task?: string
  schema: MigrationSchema
}

export const baseCliFlags = () =>
  new Command()
    .option("-p, --path <type:string>", "path to recursively apply to jsons.", { required: true })
    .option("-q --quiet", "silence all output.", { required: false })
    .option("-l, --lint", "lint all json files after migration.", { required: false })

/**
 * Cli boilerplate to recursively apply a transformation to all json files in a given path.
 * DOES NOT resolves inheritance.
 */
type BaseCli = (d: BaseCliOption) => () => Promise<unknown>
export const baseCli: BaseCli = ({ desc, task = "migration", schema }) => () =>
  baseCliFlags()
    .description(desc)
    .action(async ({ path, lint, quiet = false }) => {
      const timeIt = quiet ? <T>(_: string) => (x: T) => x : timeit
      const transformer = schemaMigrationTransformer(schema)
      const recursiveTransformer = applyRecursively(transformer)
      const entries = await readRecursively(path)

      await timeIt(task)(recursiveTransformer(entries))

      if (!lint) return
      await timeIt("linting")(fmtJsonRecursively())
    })
    .parse(Deno.args)

if (import.meta.main) {
  const { z } = await import("https://deno.land/x/zod@v3.20.5/index.ts")
  const { id } = await import("./parse.ts")

  const noop = z.object({}).transform(id)
  const main = baseCli({ desc: "Do nothing", schema: noop })
  await main()
}
