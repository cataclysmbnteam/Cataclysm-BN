import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"

import { Entry, id, ObjectSchema, parseCataJson } from "./parse.ts"

import { match, P } from "npm:ts-pattern"
import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

/** clone an object with single key replaced.
 *  not very efficient, but at least it's purely functional (when looked at from the outside)
 */
export const structuredReplace = <T extends Record<string, unknown>, K extends keyof T, V>(
  obj: T,
  key: K,
  value: V,
) => {
  const copy = structuredClone(obj)
  copy[key] = value
  return copy
}

/** clone an object with single key removed. */
export const removeObjectKey = <T extends Record<string, unknown>, K extends keyof T>(
  obj: T,
  key: K,
): Omit<T, K> => {
  const { [key]: _, ...rest } = obj
  return rest
}

export type Transformer = (text: string) => unknown[]

/** Apply same transformation to all JSON files recursively in given directory. */
export const applyRecursively = (transformer: Transformer) => async (entries: Entry[]) => {
  await asynciter(entries)
    .concurrentUnorderedMap(({ path, text }) =>
      Deno.writeTextFile(path, JSON.stringify(transformer(text), null, 2))
    )
    .collect()
}

export type MigrationSchema = z.ZodEffects<ObjectSchema>

/**
 * applies mapping schema to all entries.
 * mapping schemas are zod schema with .transform() method, which will transform an entry satisfying schema.
 */
export const schemaMigrationTransformer =
  <Schema extends MigrationSchema>(mappingSchema: Schema): Transformer => (text) =>
    parseCataJson(text)
      .map((x) =>
        match(mappingSchema.safeParse(x))
          .with({ success: true, data: P.select() }, id)
          .otherwise(() => x)
      )
