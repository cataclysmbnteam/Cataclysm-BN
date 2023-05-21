import { join } from "https://deno.land/std@0.186.0/path/mod.ts"
import { assertEquals } from "https://deno.land/std@0.186.0/testing/asserts.ts"

import { PROJECT } from "./cata_paths.ts"
import { catatags } from "./ctags.ts"

const getBackupTags = async () => {
  const path = join(PROJECT.VSCODE, ".tags.bak")
  try {
    return await Deno.readTextFile(path)
  } catch {
    return undefined
  }
}

/** generate your own .tags.bak file when running this test first. */
Deno.test({
  name: "regression test with previous .tags.bak file",
  ignore: await getBackupTags() === undefined,
  fn: async () => {
    const tagsBak = await getBackupTags()
    const tags = await catatags()

    assertEquals(tagsBak?.length, tags.join("\n").length)
  },
})
