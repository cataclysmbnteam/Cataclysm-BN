#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run --unstable

import { brightGreen, brightRed } from "https://deno.land/std@0.182.0/fmt/colors.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { match, P } from "npm:ts-pattern"
import { promiseAllProperties } from "npm:promise-all-properties"

import {
  Entry,
  genericCataTransformer,
  id as identity,
  orderPreservingSchema,
  readRecursively,
} from "./parse.ts"
import { applyRecursively, lintRecursively, structuredReplace } from "./transform.ts"
import { timeit } from "./timeit.ts"
type CataWithId = z.infer<typeof cataWithId>
const cataWithId = z.object({ id: z.string() }).passthrough()

type ParsedEntry = { path: string; parsed: CataWithId[] }

const parseIds = (entries: Entry[]): ParsedEntry[] => {
  const idExtractor = genericCataTransformer(cataWithId)(identity)

  return entries.map(({ path, text }) => ({ path, parsed: idExtractor(text) as CataWithId[] }))
}

const findId = (ids: ParsedEntry[]) => (id_: string): ParsedEntry | undefined => {
  return ids.find(({ parsed }) => parsed.some(({ id }) => id === id_))
}

type MapIf = <T>(x: T[]) => (predicate: (x: T) => boolean, fn: (x: T) => T) => T[]
const mapIf: MapIf = (xs) => (predicate, fn) => xs.map((x) => (predicate(x) ? fn(x) : x))

const warn = (msg: string) => () => console.log(brightRed(msg))

const main = () =>
  new Command()
    .description(`
      replace contents of a JSON entry with another JSON entry with the same id.
      this is useful for replacing mod content with vanilla content and vice versa.
    `)
    .option("-l, --lint", "lint all json files after migration.", { required: false })
    .option("-r, --replace <type:string>", "path to recursively search jsons.", { required: true })
    .option("-u, --using <type:string>", "path to recursively search jsons.", { required: true })
    .arguments("<id>")
    .action(async ({ replace, using, lint }, idToReplace) => {
      console.log({ replace, using, idToReplace })
      const { replaceEntries, usingEntries } = await timeit("reading entries")(
        promiseAllProperties({
          replaceEntries: readRecursively(replace).then(parseIds),
          usingEntries: readRecursively(using).then(parseIds),
        }),
      )
      const searchResult = {
        replaceEntry: findId(replaceEntries)(idToReplace),
        usingEntry: findId(usingEntries)(idToReplace),
      }

      match(searchResult)
        .with(
          { replaceEntry: P.not(P.nullish), usingEntry: P.not(P.nullish) },
          async ({ replaceEntry, usingEntry }) => {
            const actualReplaceEntry = usingEntry.parsed.find(({ id }) => id === idToReplace)!

            const newlyReplaced = mapIf(replaceEntry.parsed)(
              ({ id }) => id === idToReplace,
              () => actualReplaceEntry,
            )

            console.log({ replaceEntry, newlyReplaced })
            await Deno.writeTextFile(replaceEntry.path, JSON.stringify(newlyReplaced, null, 2))
          },
        )
        .with({ replaceEntry: P.nullish }, warn("could not find replace entry"))
        .with({ usingEntry: P.nullish }, warn("could not find using entry"))
        .otherwise(warn("could not find both replace and using entry"))

      if (!lint) return
      await timeit("linting")(lintRecursively())
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
