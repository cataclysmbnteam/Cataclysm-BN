#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run --unstable

import { walk } from "https://deno.land/std@0.178.0/fs/walk.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
export { brightGreen, brightYellow } from "https://deno.land/std@0.177.0/fmt/colors.ts"
import { Command } from "https://deno.land/x/cliffy@v0.25.7/command/mod.ts"
import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"

import { legacyGunMod } from "./types.ts"
import { migrateToGunMod } from "./gunmod.ts"
import { match } from "npm:ts-pattern"
import { brightGreen, brightYellow } from "https://deno.land/std@0.182.0/fmt/colors.ts"

const jsonEntries = z.array(z.unknown())

const applyToFile = (text: string) => {
  const entries = jsonEntries.parse(JSON.parse(text))

  const migrated = entries.map((x) =>
    match(legacyGunMod.safeParse(x))
      .with({ success: true }, (x) => migrateToGunMod(x.data))
      .otherwise(() => x)
  )

  return JSON.stringify(migrated, null, 2)
}

const applyRecursively = (path: string) =>
  asynciter(walk(path, { exts: [".json"] }))
    .concurrentUnorderedMap(async ({ path }) => {
      const text = await Deno.readTextFile(path)
      const migrated = applyToFile(text)

      await Deno.writeTextFile(path, migrated)
    })
    .collect()

await new Command()
  .name("./tools/json_tools/gunmod/run_migration.ts")
  .description(`
    migrate ${brightYellow("mod_target")} to new ${brightGreen("mod_target_category")} format.
    format the json files after migration with ${brightGreen("make style-json")}.
  `)
  .option("-p, --path <type:string>", "path to recursively search for json files", {
    required: true,
  })
  .action(async ({ path }) => applyRecursively(path))
  .parse(Deno.args)
