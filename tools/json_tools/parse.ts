/**
 * @file
 *
 * Parses cataclysm JSON files.
 */

import { walk } from "https://deno.land/std@0.182.0/fs/walk.ts"
import { basename } from "https://deno.land/std@0.182.0/path/mod.ts"

import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"
import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

import { match, P } from "npm:ts-pattern"
import { id } from "./utils/id.ts"

/** most common form of cataEntry. */
const jsonEntries = z.array(z.unknown())

/** parses cataclysm JSON. wraps single object into array. */
export const parseCataJson = (text: string): unknown[] => {
  const rawJson = JSON.parse(text)

  return match(jsonEntries.safeParse(rawJson))
    .with({ success: true, data: P.select() }, id)
    .otherwise(() => [rawJson])
}

export type ObjectSchema = z.ZodObject<z.ZodRawShape>
export type MigrationSchema = z.ZodEffects<ObjectSchema>

/** function that will transform given entry into output.
 * its output should not be used as input to other functions.
 */
export type CataJsonEntryFn<Schema extends ObjectSchema> = (obj: z.infer<Schema>) => unknown

const isValidJson = (name: string) => !["default.json", "replacements.json"].includes(name)

type ToEntry = (path: string) => Promise<Entry>
const toEntry: ToEntry = async (path) => ({ path, text: await Deno.readTextFile(path) })

/** lightweight file system entry with file path and text content. */
export type Entry = { path: string; text: string }

/**
 * @internal
 * recursively reads all JSON files from given directory.
 */
type ReadDirRecursively = (root: string) => () => Promise<Entry[]>
const readDirRecursively: ReadDirRecursively = (root) => () =>
  asynciter(walk(root, { exts: [".json"] }))
    .filter(({ name }) => isValidJson(name))
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
    .with({ root: P.when((x) => !isValidJson(basename(x))) }, () => {
      throw new Error(`path ${root} is not a valid JSON file`)
    })
    .with(
      { isFile: true, root: P.when((x) => x.endsWith(".json")) },
      async () => [await toEntry(root)],
    )
    .with({ isDirectory: true }, readDirRecursively(root))
    .otherwise(() => {
      throw new Error(`path ${root} is neither JSON file nor directory`)
    })
