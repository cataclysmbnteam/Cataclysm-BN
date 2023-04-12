import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"
import { Entry } from "./parse.ts"

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

export const lintRecursively = () =>
  Deno.run({ cmd: ["make", "style-all-json-parallel"], stdout: "null", stderr: "null" })
    .status()
