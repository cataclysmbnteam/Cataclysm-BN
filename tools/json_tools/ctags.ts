#!/usr/bin/env -S deno run --unstable --allow-read --allow-write

import { relative } from "https://deno.land/std@0.182.0/path/mod.ts"
import { join } from "https://deno.land/std@0.182.0/path/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { PROJECT } from "./cata_paths.ts"
import { timeit } from "./timeit.ts"
import { Entry, readRecursively } from "./parse.ts"

/** @link https://ctags.sourceforge.net/FORMAT */
const makeTagsLine = (idKey: string, tagname: string, tagfile: string, line: number): string => {
  const tagaddress = `/"${idKey}": "${tagname}"/`
  return `${tagname}\t${tagfile}\t${tagaddress};"\tline:${line}`
}

const idKeys = ["id", "abstract", "ident", "nested_mapgen_id"]

const isValidPair = (pair: [string, unknown]): pair is [string, string] => {
  const [key, value] = pair
  return idKeys.includes(key) && typeof value === "string" && !!value
}

/**
 * finds first line of appearance of a string in a text.
 * it's dumb, it slow but it works.
 * @link https://stackoverflow.com/a/32017337/13503626
 */
const findFirstAppearance = (text: string) => (search: string) => {
  const index = text.indexOf(search)
  if (index === -1) return 0
  return text.slice(0, index).split("\n").length
}

const intoCtag =
  (finder: (x: string) => number) =>
  (relativePath: string) =>
  (obj: Record<string, unknown>): string[] => {
    const result = Object.entries(obj)
      .filter(isValidPair)

    return result.map(([key, value]) => makeTagsLine(key, value, relativePath, finder(value)))
  }

const intoCtags = ({ path, text }: Entry) => {
  const relativePath = relative(PROJECT.ROOT, path)
  const jsonData = JSON.parse(text)
  const jsonList = Array.isArray(jsonData) ? jsonData : [jsonData]
  const finder = findFirstAppearance(text)
  const into = intoCtag(finder)(relativePath)

  return jsonList.flatMap(into)
}

export const catatags = async (): Promise<string[]> => {
  const entries = await readRecursively(PROJECT.DATA)

  return entries.flatMap(intoCtags)
}

const oldTags = async (path: string) => {
  try {
    const oldTags = await Deno.readTextFile(path)
    return oldTags.split("\n").filter((x) => !x.includes(".json\t"))
  } catch {
    return []
  }
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
      const previousTags = await timeit("reading old tags")(oldTags(path))
      const allTagsLines = await timeit("generating ctags")(catatags())

      const text = [...allTagsLines, ...previousTags].sort().join("\n")

      await Deno.writeTextFile(path, text)
      console.log(`wrote ${allTagsLines.length} tags to ${path}`)
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
