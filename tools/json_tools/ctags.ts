#!/usr/bin/env -S deno run --unstable --allow-read --allow-write

import { relative } from "https://deno.land/std@0.182.0/path/mod.ts"
import { join } from "https://deno.land/std@0.182.0/path/mod.ts"
import { walk, WalkEntry } from "https://deno.land/std@0.182.0/fs/walk.ts"

import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { PROJECT } from "./cata_paths.ts"
import { timeit } from "./timeit.ts"

/** @link https://ctags.sourceforge.net/FORMAT */
const makeTagsLine = (idKey: string, tagname: string, tagfile: string): string => {
  const tagaddress = `/^ *"${idKey}": "${tagname}"/`
  return `${tagname}\t${tagfile}\t${tagaddress}`
}

const idKeys = ["id", "abstract", "ident", "nested_mapgen_id"]

const intoCtags = async ({ path }: WalkEntry) => {
  const relativePath = relative(PROJECT.ROOT, path)
  const jsonData = JSON.parse(await Deno.readTextFile(path))
  const jsonList = Array.isArray(jsonData) ? jsonData : [jsonData]

  return jsonList.flatMap((obj) =>
    Object.entries(obj)
      .filter(([key, value]) => idKeys.includes(key) && typeof value === "string" && value)
      .map(([key, value]) => makeTagsLine(key, value as any, relativePath))
  )
}

const isValidJson = ({ name }: WalkEntry) => !["default.json", "replacements.json"].includes(name)

export const catatags = async (): Promise<string[]> => {
  const jsonTagsLines = await asynciter(walk(PROJECT.DATA, { includeDirs: false, exts: [".json"] }))
    .filter(isValidJson)
    .concurrentUnorderedMap(intoCtags)
    .collect()

  return jsonTagsLines.flat().sort()
}

const main = () =>
  new Command()
    .description(`
        generates ctags for cataclysm json files.
        for vscode, install https://marketplace.visualstudio.com/items?itemName=gediminaszlatkus.ctags-companion
    `)
    .option("-p, --path <type:string>", "path to save ctags file", {
      default: join(PROJECT.ROOT, ".vscode", ".tags"),
    })
    .action(async ({ path }) => {
      const allTagsLines = await timeit("generating ctags")(catatags())
      const text = allTagsLines.join("\n")

      await Deno.writeTextFile(path, text)
      console.log(`wrote ${allTagsLines.length} tags to ${path}`)
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
