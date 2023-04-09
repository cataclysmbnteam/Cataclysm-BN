#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run --unstable

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"

import { match } from "npm:ts-pattern"

import { genericCataTransformer } from "./parse.ts"
import { applyRecursively, structuredReplace } from "./transform.ts"
import { timeit } from "./timeit.ts"

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

export type RemoveFlags = (diff: ReturnType<Diff<string>>) => (x: Flaggable) => unknown
export const removeFlags: RemoveFlags = (diff) => (obj) => {
  const { flags, ...args } = obj
  return match(diff(flags))
    .with([], () => args)
    .otherwise((x) => structuredReplace(obj, "flags", x))
}

const main = () =>
  new Command()
    .description(`
      completely removes given list of flags from all json files in given path
    `)
    .option("-l, --lint", "lint all json files after migration.", { required: false })
    .option("-p, --path <type:string>", "path to recursively search jsons.", { required: true })
    .arguments("<flags...>")
    .action(async ({ path, lint }, ...flags) => {
      const transformer = genericCataTransformer(flaggable)(removeFlags(diff(flags)))
      await timeit("stripping flags")(applyRecursively(transformer)(path))

      if (!lint) return
      await timeit("linting")(
        Deno.run({ cmd: ["make", "style-all-json-parallel"], stdout: "null", stderr: "null" })
          .status(),
      )
    })
    .parse(Deno.args)

if (import.meta.main) {
  await main()
}
