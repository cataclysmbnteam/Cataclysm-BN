#!/usr/bin/env -S deno run --unstable --allow-read --allow-write

import { relative } from "https://deno.land/std@0.182.0/path/mod.ts"
import { join } from "https://deno.land/std@0.182.0/path/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { PROJECT } from "./cata_paths.ts"
import { timeit } from "./timeit.ts"
import { Entry, readRecursively } from "./parse.ts"

/** @link https://ctags.sourceforge.net/FORMAT */
const makeTagsLine = (idKey: string, tagname: string, tagfile: string): string => {
  const tagaddress = `/"${idKey}": "${tagname}"/`
  return `${tagname}\t${tagfile}\t${tagaddress}`
}

const idKeys = ["id", "abstract", "ident", "nested_mapgen_id"]

const isValidPair = (pair: [string, unknown]): pair is [string, string] => {
  const [key, value] = pair
  return idKeys.includes(key) && typeof value === "string" && !!value
}

const intoCtag = (relativePath: string) => (obj: Record<string, unknown>): string[] =>
  Object.entries(obj)
    .filter(isValidPair)
    .map(([key, value]) => makeTagsLine(key, value, relativePath))

const intoCtags = ({ path, text }: Entry) => {
  const relativePath = relative(PROJECT.ROOT, path)
  const jsonData = JSON.parse(text)
  const jsonList = Array.isArray(jsonData) ? jsonData : [jsonData]
  const into = intoCtag(relativePath)

  return jsonList.flatMap(into)
}

export const catatags = async (): Promise<string[]> => {
  const entries = await readRecursively(PROJECT.DATA)

  return entries.flatMap(intoCtags).sort()
}

const main = () =>
  new Command()
    .description(`
        generates ctags for cataclysm json files.
        see doc/DEVELOPER_TOOLING.md for more info.
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
