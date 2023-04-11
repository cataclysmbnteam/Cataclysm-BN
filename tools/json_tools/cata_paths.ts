import { fromFileUrl, join } from "https://deno.land/std@0.182.0/path/mod.ts"

export const FILE_PATH = fromFileUrl(import.meta.url)
export const ROOT = fromFileUrl(new URL("../../", import.meta.url))

const fromRoot = (path: string) => join(ROOT, path)

export const PROJECT = {
  ROOT,
  SRC: fromRoot("src"),
  DATA: fromRoot("data"),
  JSON: fromRoot("data/json"),
  TOOLS: fromRoot("tools"),
} as const

if (import.meta.main) {
  for (const [name, path] of Object.entries(PROJECT)) {
    console.log(`${name}: ${path}`)
  }
}
