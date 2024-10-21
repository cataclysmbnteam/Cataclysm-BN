import { Command } from "@cliffy/command"
import { cliOptions } from "$catjazz/utils/cli.ts"
import { timeit } from "$catjazz/utils/timeit.ts"
import { applyRecursively, schemaTransformer } from "$catjazz/utils/transform.ts"
import { type CataEntry, Entry, parseCataJson, readJSONsRec } from "$catjazz/utils/parse.ts"
import { fmtJsonRecursively } from "$catjazz/utils/json_fmt.ts"
import { match, P } from "$catjazz/deps/ts_pattern.ts"
import { id } from "$catjazz/utils/id.ts"
import { z } from "$catjazz/deps/zod.ts"

// FIXME: include in library
const unpack = (xs: string[] | Entry[]) =>
  match(xs)
    .with(P.array(P.string), id)
    .otherwise((xs) => xs.map(({ path }) => path))

const main = new Command()
  // TODO: allow multiple paths
  .option(...cliOptions.paths)
  .option(...cliOptions.format)
  .option(...cliOptions.quiet)
  .arguments("<...ids>")
  .description("Converts given id to stackable.")
  .action(async ({ paths, quiet = false, format }, ...ids) => {
    const timeIt = timeit(quiet)

    const schema = z.object({
      // @ts-expect-error: zod hates string literal mapping
      id: z.union(ids.map((id) => z.literal(id))) as z.ZodString,
      type: z.string().transform((x) => x === "TOOL" ? "GENERIC" : x),
    })
      .passthrough()
      .transform(({ id, ...rest }) => {
        console.log(id)
        return { id, ...rest, stackable: true }
      })

    const transformer = schemaTransformer(schema)
    const ignore = (entries: CataEntry[]) =>
      entries.find(({ type }) => ["mapgen", "palette", "mod_tileset"].includes(type))

    // FIXME: include in library
    const mapgenIgnoringTransformer = (text: string) => {
      const entries = parseCataJson(text)
      return ignore(entries) ? text : JSON.stringify(transformer(entries), null, 2)
    }

    const recursiveTransformer = applyRecursively(mapgenIgnoringTransformer)

    const entries = await timeIt({ name: "reading JSON", val: readJSONsRec(paths) })

    await timeIt({ name: "Transforming", val: recursiveTransformer(entries) })

    if (!format) return
    await timeIt({
      name: "formatting",
      val: fmtJsonRecursively({ formatterPath: format, quiet: true })(unpack(entries)),
    })
  })

if (import.meta.main) {
  await main.parse(Deno.args)
}
