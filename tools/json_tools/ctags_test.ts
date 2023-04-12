import { join } from "https://deno.land/std@0.182.0/path/mod.ts"
import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { PROJECT } from "./cata_paths.ts"
import { catatags } from "./ctags.ts"

/** generate your own .tags.bak file when running this test first. */
Deno.test("regression test with previous tags.bak file", async () => {
  const tagsBak = await Deno.readTextFile(join(PROJECT.VSCODE, ".tags.bak"))
  const tags = await catatags()

  assertEquals(tagsBak, tags.join("\n"))
})
