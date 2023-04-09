import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import { match, P } from "npm:ts-pattern"

/** identity function that preserves type. */
export const id = <T>(x: T): T => x

/** most common form of cataEntry. */
const jsonEntries = z.array(z.unknown())

/**
 * parse a json string into an array of unknown.
 * supports both array and single object.
 */
export const parseCataJsonFile = async (path: string): Promise<unknown[]> =>
  parseCataJson(await Deno.readTextFile(path))

/** parses cataclysm JSON. wraps single object into array. */
export const parseCataJson = (text: string): unknown[] => {
  const rawJson = JSON.parse(text)

  return match(jsonEntries.safeParse(rawJson))
    .with({ success: true, data: P.select() }, id)
    .otherwise(() => [rawJson])
}

/** extract all elements from a json array that match a given schema. */
export const filterSchema = <T extends z.ZodTypeAny>(schema: T): z.ZodArray<T> =>
  z.unknown().array()
    .transform((xs) => xs.filter((x) => schema.safeParse(x).success)) as unknown as z.ZodArray<T>
