#!/usr/bin/env -S deno run --allow-read --allow-write

import { asynciter, brightGreen, outdent, partition } from "./deps.ts"
import { Converter, converter } from "./converter.ts"
import { Command, walk } from "./deps.ts"
import { description } from "./description.ts"
import { stateFns } from "./state_fns.ts"

const resolvedPaths = (exts: string[]) => async (paths: string[]) => {
  const entries = await Promise.all(
    paths.map(async (path) => ({ path, stat: await Deno.stat(path) })),
  )
  const [files, directories] = partition(entries, ({ stat }) => stat.isFile).map((stats) =>
    stats.map(({ path }) => path)
  )

  const walkedFiles = await asynciter(directories).concurrentUnorderedMap((dir) =>
    asynciter(walk(dir, { exts })).map(({ path }) => path).collect()
  ).collect().then((paths) => paths.flat())

  return new Set([...files, ...walkedFiles])
}

const gather = (paths: string[]) =>
  asynciter(paths)
    .concurrentUnorderedMap(async (path) => ({ path, text: await Deno.readTextFile(path) }))

const convertAndShow = (converter: Converter) => async (paths: string[]) =>
  gather(paths).map(async ({ path, text }) =>
    console.log(outdent`${brightGreen(`<<<${path}>>>`)}

    ${converter(text.split("\n")).join("\n")}
  `)
  ).collect()

const convertAndWrite = (converter: Converter) => async (paths: string[]) =>
  gather(paths)
    .concurrentUnorderedMap(({ path, text }) =>
      Deno.writeTextFile(path, converter(text.split("\n")).join("\n"))
    )
    .collect()

const main = () =>
  new Command()
    .name("fenced.ts")
    .version("0.1.0")
    .description(description)
    .option("-l, --lang <lang>", "Language to use for fenced code blocks", {
      default: "sh",
    })
    .option("-w --write", "Write to file")
    .arguments("<paths...>")
    .action(async ({ lang, write }, ...paths) => {
      const convert = converter(stateFns(lang))
      const resolved = await resolvedPaths([".md"])(paths)
      const action = write ? convertAndWrite : convertAndShow

      await action(convert)([...resolved])
      console.log(`${write ? "Wrote" : "Converted"} ${resolved.size} files`)
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
