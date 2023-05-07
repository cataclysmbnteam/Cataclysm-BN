#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run --unstable

import {
  brightGreen as g,
  brightRed as r,
  brightYellow as y,
} from "https://deno.land/std@0.186.0/fmt/colors.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { match, P } from "npm:ts-pattern"
import { promiseAllProperties } from "npm:promise-all-properties"

import { Entry, readRecursively } from "./parse.ts"
import { timeit } from "./timeit.ts"
import { fmtJsonRecursively } from "./json_fmt.ts"
import { schemaTransformer } from "./transform.ts"

type CataWithId = z.infer<typeof cataWithId>
const cataWithId = z.object({ id: z.string() }).passthrough()

type ParsedEntry = { path: string; parsed: CataWithId[] }

export const parseIds = (entries: Entry[]): ParsedEntry[] => {
  const idExtractor = schemaTransformer(cataWithId)

  return entries.map(({ path, text }) => ({ path, parsed: idExtractor(text) as CataWithId[] }))
}

export const findId = (ids: ParsedEntry[]) => (idToFind: string): ParsedEntry | undefined => {
  return ids.find(({ parsed }) => parsed.some(({ id }) => id === idToFind))
}

export const findFromEntry = (parsed: CataWithId[]) => (idToFind: string): CataWithId => {
  const result = parsed.find(({ id }) => id === idToFind)
  if (!result) {
    throw new Error(`could not find id ${idToFind}`)
  }
  return result
}

type ReplaceWith = (replace: CataWithId[]) => (using: CataWithId[]) => (id: string) => CataWithId[]
export const replaceWith: ReplaceWith = (replace) => (using) => (idToReplace) => {
  const actualReplaceEntry = findFromEntry(using)(idToReplace)

  return replace.map((x) =>
    match(x.id)
      .with(idToReplace, () => actualReplaceEntry)
      .otherwise(() => x)
  )
}

const nonNull = () => P.not(P.nullish)

const writeReplaceWith =
  (replaceEntries: ParsedEntry[]) =>
  (usingEntries: ParsedEntry[]) =>
  (idToReplace: string): Promise<string | void> => {
    const withId = (msg: string) => `${r(idToReplace.padEnd(20))} : ${msg}`
    const warn = (msg: string) => () => Promise.resolve(console.log(withId(r(msg))))

    const replaceEntry = findId(replaceEntries)(idToReplace)
    const usingEntry = findId(usingEntries)(idToReplace)

    return match({ replaceEntry, usingEntry })
      .with(
        { replaceEntry: nonNull(), usingEntry: nonNull() },
        async (
          { replaceEntry: { path, parsed: replace }, usingEntry: { path: from, parsed: using } },
        ) => {
          const newlyReplaced = replaceWith(replace)(using)(idToReplace)
          const write = Deno.writeTextFile(path, JSON.stringify(newlyReplaced, null, 2))

          await timeit(withId(`${y(path)} <- ${g(from)}`))(write)
          return path
        },
      )
      .otherwise(warn(`${replaceEntry?.path ?? "no entry"} <- ${usingEntry?.path ?? "no entry"}`))
  }

const ex = {
  id: r("foo"),
  replace: y('{ "id": "foo", "desc": "to be replaced" }'),
  replacePath: y("replace/(any path)/path.json"),
  using: g('{ "id": "foo", "desc": "using" }'),
  usingPath: g("using/(any path)/.json"),
}

const main = () =>
  new Command()
    .description(`
      ${y("replace")} contents of a JSON entry ${g("using")} another JSON entry with the same id.
      this is useful for replacing mod content with vanilla content and vice versa.

      for example, imagine you have two entries with same id ${ex.id}:
      ${ex.replace} at ${ex.replacePath}
      ${ex.using} at ${ex.usingPath}

      if you want to replace ${ex.replace} with ${ex.using}, you can run:
      tools/json_tools/replace_with.ts ${y("--replace replace/")} ${g("--using using/")} ${ex.id}
    `)
    .option("-q --quiet", "silence all output.", { required: false })
    .option("-l, --lint", "lint all json files after migration.", { required: false })
    .option("-r, --replace <type:string>", "path to recursively search jsons.", { required: true })
    .option("-u, --using <type:string>", "path to recursively search jsons.", { required: true })
    .arguments("<id...>")
    .action(async ({ replace, using, lint, quiet = false }, ...idsToReplace) => {
      const readResult = promiseAllProperties({
        replaceEntries: readRecursively(replace).then(parseIds),
        usingEntries: readRecursively(using).then(parseIds),
      })
      const { replaceEntries, usingEntries } = await timeit("reading entries")(readResult)
      const replaceFn = writeReplaceWith(replaceEntries)(usingEntries)

      const result = await Promise.all(idsToReplace.map(replaceFn))

      if (!lint) return
      const paths = result.filter((x): x is string => x !== undefined)
      await timeit("linting")(fmtJsonRecursively(true)(paths))
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
