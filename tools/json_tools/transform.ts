import { walk } from "https://deno.land/std@0.178.0/fs/walk.ts"
import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"
import { match, P } from "npm:ts-pattern"

/** clone an object with single key replaced.
 *  not very efficient, but at least it's purely functional (when looked at from the outside)
 */
export const structuredReplace = <T, V>(obj: T, key: keyof T, value: V) => {
  const copy = structuredClone(obj)
  copy[key] = value
  return copy
}

export type Transformer = (text: string) => unknown[]

export const applyRecursively = (transformer: Transformer) => async (path: string) => {
  const apply = async (path: string) => {
    const text = await Deno.readTextFile(path)
    const migrated = JSON.stringify(transformer(text), null, 2)

    await Deno.writeTextFile(path, migrated)
  }

  return match({ path, ...(await Deno.stat(path)) })
    .with({ isFile: true, path: P.when((x) => x.endsWith(".json")) }, () => apply(path))
    .with(
      { isDirectory: true },
      () => {
        asynciter(walk(path, { exts: [".json"] }))
          .concurrentUnorderedMap(async ({ path }) => apply(path))
          .collect()
      },
    )
    .otherwise(() => {
      throw new Error(`path ${path} is neither JSON file nor directory`)
    })
}
