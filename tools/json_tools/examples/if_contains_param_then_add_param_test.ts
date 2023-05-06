import { z } from "https://deno.land/x/zod@v3.20.5/index.ts"

import { match, P } from "npm:ts-pattern"

import { testSchema } from "../utils/test_schema.ts"

const overmapSchema = z.object({
  type: z.literal("overmap_special"),
  flags: z.string().array().optional(),
})

// typescript can extract types from values using typeof keyword.
// z.infer is a very, very complex typescript type which
// extracts the type from a zod schema for you.
type OvermapSchema = z.infer<typeof overmapSchema>

type Flags = OvermapSchema["flags"]

// it takes an array of strings, or undefined
const mergeUniquely = (newArr: string[]) => (prevArr?: string[]) => {
  if (prevArr === undefined) return newArr

  const added = [...prevArr, ...newArr]
  return [...new Set(added)] // we must use a set because it's unknown how many duplicates there are
}

const addElectricGridSchema = overmapSchema
  .passthrough()
  .transform(({ flags, ...args }) => {
    // we fill newFlags first
    // so we can use this lazily generated function to add flags
    const addElectricGrid = mergeUniquely(["ELECTRIC_GRID"])

    // now we pattern match to check if flags are undefined
    return match(flags)
      // if so, we create a new flags field with ELECTRIC_GRID
      .with(P.nullish, () => ({ ...args, flags: addElectricGrid() }))
      // if the flags are defined, we add ELECTRIC_GRID to it but make sure
      // to not add duplicates by using mergeUniquely
      .otherwise(() => ({ ...args, flags: addElectricGrid(flags) }))
  })

const gridTestcases = [
    {
      input: { type: "overmap_special", flags: ["flag1", "flag2", "flag3", "flag4"] },
      expected: {
        type: "overmap_special",
        flags: ["flag1", "flag2", "flag3", "flag4", "ELECTRIC_GRID"],
      },
    },
    {
      input: { type: "overmap_special" },
      expected: { type: "overmap_special", flags: ["ELECTRIC_GRID"] },
    },
    {
      "id": {
        type: "overmap_special",
        flags: ["flag1", "flag2", "flag3", "flag4", "ELECTRIC_GRID"],
      },
    },
  ]

Deno.test("overmap electric grid", async (t) =>
  await testSchema(t.step)(addElectricGridSchema)(gridTestcases))

const endsWithSword = z.string().refine((x) => x.endsWith("sword"))
const containsSword = z.string().refine((x) => x.includes("sword"))
const maybeWeaponCategory = z.string().array().optional().default([])

const swordSchema = z
  .record(z.string(), z.unknown())
  .transform((obj) => {
    const nameOk = endsWithSword.safeParse(obj?.name).success
    const descOk = containsSword.safeParse(obj?.description).success
    const prevWeaponCategory = maybeWeaponCategory.parse(obj?.weapon_category)

    const addSwordFlag = mergeUniquely(["1H_SWORDS", "DUELING_SWORDS"])

    if (nameOk || descOk) return { ...obj, weapon_category: addSwordFlag(prevWeaponCategory) }
    return obj
  })

Deno.test("sword schema", async (t) =>
  await testSchema(t.step)(swordSchema)([
    {
      input: { name: "test sword" },
      expected: { name: "test sword", weapon_category: ["1H_SWORDS", "DUELING_SWORDS"] },
    },
    {
      input: { name: "funni staff", description: "it actually contains a hidden sword" },
      expected: {
        name: "funni staff",
        description: "it actually contains a hidden sword",
        weapon_category: ["1H_SWORDS", "DUELING_SWORDS"],
      },
    },
    { id: { name: "this does not ends with sword so it should not add the flag" } },
    { id: { name: "test sword", weapon_category: ["1H_SWORDS", "DUELING_SWORDS"] } },
  ]))
