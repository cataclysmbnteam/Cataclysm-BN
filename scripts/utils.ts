import * as v from "jsr:@valibot/valibot"

import { walk } from "jsr:@std/fs"
import { asynciter } from "$asynciter/mod.ts"
import type { BaseIssue, BaseSchema, InferOutput } from "jsr:@valibot/valibot"

/**
 * create a parser from given schema that parses an array of objects
 */
export const parseMany = <const TSchema extends BaseSchema<unknown, unknown, BaseIssue<unknown>>>(
  schema: TSchema,
) => {
  const parser = v.safeParser(schema)
  return (xs: unknown[]): InferOutput<TSchema>[] =>
    xs
      .map(parser)
      .filter((x) => x.success)
      .map((x) => x.output)
}

/**
 * @param rootPath a path to recursively read JSON files from
 * @returns an array of parsed unverified JSON objects
 */
export const recursivelyReadJSON = (rootPath: string): Promise<unknown[]> =>
  asynciter(walk(rootPath, { exts: [".json"], skip: [/modinfo\.json/] }))
    .concurrentUnorderedMap((x) =>
      Deno.readTextFile(x.path).then((x) => JSON.parse(x) as unknown[])
    )
    .collect()
    .then((xs) => xs.flat())
