/**
 * @file
 *
 * Parses cataclysm JSON files.
 */

import { walk } from "https://deno.land/std@0.182.0/fs/walk.ts"
import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"
import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { match, P } from "npm:ts-pattern"

/** identity function that preserves type. */
export const id = <T>(x: T): T => x

/** most common form of cataEntry. */
const jsonEntries = z.array(z.unknown())

/** parses cataclysm JSON. wraps single object into array. */
export const parseCataJson = (text: string): unknown[] => {
  const rawJson = JSON.parse(text)

  return match(jsonEntries.safeParse(rawJson))
    .with({ success: true, data: P.select() }, id)
    .otherwise(() => [rawJson])
}

type ObjectSchema = z.ZodObject<z.ZodRawShape>

/**
 * wraps a schema to return data in same order as passed.
 *
 * @link https://github.com/colinhacks/zod/discussions/1852#discussioncomment-4658084
 */
export const orderPreservingSchema = <Schema extends ObjectSchema>(
  schema: Schema,
) => z.custom<z.infer<Schema>>((value) => schema.safeParse(value).success)

/** function that will transform given entry into output.
 * its output should not be used as input to other functions.
 */
export type CataJsonEntryFn<Schema extends ObjectSchema> = (obj: z.infer<Schema>) => unknown

/**
 * applies a function to all json entries that match a given schema.
 * order of unmatched entries is preserved.
 *
 * probably handles 90% of use cases.
 *
 * @param fn function that will transform entry satisfying schema.
 * @param text raw json string to parse.
 */
export const genericCataTransformer =
  <Schema extends ObjectSchema>(schema: Schema) =>
  (fn: CataJsonEntryFn<Schema>) =>
  (text: string): unknown[] => {
    const preservingSchema = orderPreservingSchema(schema)
    return parseCataJson(text)
      .map((x) =>
        match(preservingSchema.safeParse(x))
          .with({ success: true, data: P.select() }, fn)
          .otherwise(() => x)
      )
  }

type ToEntry = (path: string) => Promise<Entry>
const toEntry: ToEntry = async (path) => ({ path, text: await Deno.readTextFile(path) })

/** lightweight file system entry with file path and text content. */
export type Entry = { path: string; text: string }

/** recursively reads all JSON files from given directory. */
type ReadDirRecursively = (root: string) => () => Promise<Entry[]>
const readDirRecursively: ReadDirRecursively = (root) => () =>
  asynciter(walk(root, { exts: [".json"] }))
    .concurrentUnorderedMap(({ path }) => toEntry(path))
    .collect()

/**
 * recursively reads all JSON files from given path.
 *
 * @param root path to file or directory.
 */
export type ReadRecursively = (root: string) => Promise<Entry[]>
export const readRecursively: ReadRecursively = async (root) =>
  match({ root, ...(await Deno.stat(root)) })
    .with(
      { isFile: true, root: P.when((x) => x.endsWith(".json")) },
      async () => [await toEntry(root)],
    )
    .with({ isDirectory: true }, readDirRecursively(root))
    .otherwise(() => {
      throw new Error(`path ${root} is neither JSON file nor directory`)
    })
