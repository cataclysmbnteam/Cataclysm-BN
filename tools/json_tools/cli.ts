/** Boilerplate to  */
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { Entry, MigrationSchema, ObjectSchema, readRecursively } from "./parse.ts"
import { applyRecursively, schemaTransformer } from "./transform.ts"
import { timeit } from "./timeit.ts"
import { fmtJsonRecursively } from "./json_fmt.ts"
type BaseCliOption = {
  desc: string
  task?: string
  schema: ObjectSchema | MigrationSchema
}
import { match, P } from "npm:ts-pattern"
import { id } from "./utils/id.ts"

const unpack = (xs: string[] | Entry[]) =>
  match(xs)
    .with(P.array(P.string), id)
    .otherwise((xs) => xs.map(({ path }) => path))

export const baseCliFlags = () =>
  new Command()
    .option("-p, --path <type:string>", "path to recursively apply to jsons.", { required: true })
    .option("-q --quiet", "silence all output.", { required: false })
    .option("-l, --lint", "lint all json files after migration.", { required: false })

/**
 * Cli boilerplate to recursively apply a transformation
 * to all json files concurrently in a given path.
 *
 * Limitations:
 * - does not resolve inheritance.
 * - cannot reference other entries.
 */
type BaseCli = (d: BaseCliOption) => () => Promise<unknown>
export const baseCli: BaseCli = ({ desc, task = "migration", schema }) => () =>
  baseCliFlags()
    .description(desc)
    .action(async ({ path, lint, quiet = false }) => {
      const timeIt = quiet ? <T>(_: string) => (x: T) => x : timeit
      const transformer = schemaTransformer(schema)
      const recursiveTransformer = applyRecursively(transformer)

      const entries = await readRecursively(path)

      await timeIt(task)(recursiveTransformer(entries))

      if (!lint) return
      await timeIt("linting")(fmtJsonRecursively(false)(unpack(entries)))
    })
    .parse(Deno.args)

if (import.meta.main) {
  const { z } = await import("https://deno.land/x/zod@v3.20.5/index.ts")
  const { id } = await import("./utils/id.ts")

  const noop = z.object({}).passthrough().transform(id)
  const main = baseCli({ desc: "Do nothing", schema: noop })
  await main()
}
