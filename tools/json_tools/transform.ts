import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"

import { match, P } from "npm:ts-pattern"

import { Entry, MigrationSchema, ObjectSchema, parseCataJson } from "./parse.ts"

import { id } from "./utils/id.ts"

export type Transformer = (text: string) => unknown[]

/** Apply same transformation to all JSON files recursively in given directory. */
export const applyRecursively = (transformer: Transformer) => async (entries: Entry[]) => {
  await asynciter(entries)
    .concurrentUnorderedMap(({ path, text }) =>
      Deno.writeTextFile(path, JSON.stringify(transformer(text)))
    )
    .collect()
}

/**
 * creates a transformer that searches (and migrates) entries satisfying given schema.
 * mapping schemas are zod schema with .transform() method either on object level or property,
 * which will transform an entry satisfying schema.
 *
 * entries that do not satisfy schema are left unchanged.
 */
export const schemaMigrationTransformer =
  (schema: ObjectSchema | MigrationSchema): Transformer => (text) =>
    parseCataJson(text)
      .map((x) =>
        match(schema.safeParse(x))
          .with({ success: true, data: P.select() }, id)
          .otherwise(() => x)
      )
