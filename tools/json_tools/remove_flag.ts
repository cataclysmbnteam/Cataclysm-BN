#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run --unstable

import { walk } from "https://deno.land/std@0.178.0/fs/walk.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"
import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"

import { match } from "npm:ts-pattern"
import { genericCataTransformer } from "./parse.ts"

export type Flaggable = z.infer<typeof flaggable>
export const flaggable = z.object({ flags: z.string().array() }).passthrough()

/**
 * Removes all occurances of xs from ys.
 *
 * @example
 * diff([1, 2])([1, 2, 3, 4]) // [3, 4]
 */
export type Diff<T> = (xs: T[]) => (ys: T[]) => T[]
export const diff = <T>(xs: T[]) => (ys: T[]) => ys.filter((y) => !xs.includes(y))

export const structuredReplace = <T, V>(obj: T, key: keyof T, value: V) => {
  const copy = structuredClone(obj)
  copy[key] = value
  return copy
}

export type RemoveFlags = (diff: ReturnType<Diff<string>>) => (x: Flaggable) => unknown
export const removeFlags: RemoveFlags = (diff) => (obj) => {
  const { flags, ...args } = obj
  return match(diff(flags))
    .with([], () => args)
    .otherwise((x) => structuredReplace(obj, "flags", x))
}

type Transformer = (text: string) => unknown[]

const applyRecursively = (transformer: Transformer) => (path: string) =>
  asynciter(walk(path, { exts: [".json"] }))
    .concurrentUnorderedMap(async ({ path }) => {
      const text = await Deno.readTextFile(path)
      const migrated = JSON.stringify(transformer(text), null, 2)

      await Deno.writeTextFile(path, migrated)
    })
    .collect()

const main = () =>
  new Command()
    .name("./tools/json_tools/gunmod/run_migration.ts")
    .description(`
      completely removes given list of flags from all json files in given path
    `)
    .option(
      "-p, --path <type:string>",
      "path to recursively search for json files",
      { required: true },
    )
    .arguments("<flags...>")
    .action(async ({ path }, ...flags) => {
      const transformer = genericCataTransformer(flaggable)(removeFlags(diff(flags)))
      await applyRecursively(transformer)(path)
      await Deno.run({ cmd: ["make", "style-all-json-parallel", "RELEASE=1"] }).status()
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
