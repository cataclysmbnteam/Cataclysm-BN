import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { partition } from "https://deno.land/std@0.182.0/collections/partition.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/index.ts"
const before = {
  "armor_bullet": 30,
  "id": "item_id1",
  "name": "item1",
  "armor_bash": 46,
  "material": "nanite",
  "armor_cut": 30,
  "weight": "460 g",
  "volume": "430 ml",
  "armor_acid": 80,
  "armor_foo": 123,
  "armor_bar": 456,
  "armor_baz": 789,
  "flags": ["foo", "bar", "baz"],
}

const after = {
  "id": "item_id1",
  "name": "item1",
  "material": ["nanite", "broken_nanite"],
  "flags": ["foo", "bar", "baz"],
  "weight": "460 g",
  "volume": "430 ml",
  "armor": { "bash": 46, "cut": 30, "bullet": 30, "acid": 80, "foo": 123, "bar": 456, "baz": 789 },
}

const mergeComplexArmorParamsSchema = z.object({
  material: z.string(),
  flags: z.string().array(),
})
  .passthrough()
  .transform(({ material, flags, ...args }) => {
    // we first partition the object into two arrays:
    // one is array of fields that starts with "armor_", and the other which does not.
    const [armors, notArmors] = partition(Object.entries(args), ([key]) => key.startsWith("armor_"))

    // strip "armor_" prefix from the keys.
    const armorskeys = armors.map(([key, value]) => [key.replace("armor_", ""), value])

    return {
      armor: Object.fromEntries(armorskeys),
      material: [material, `broken_${material}`],
      flags,
      ...Object.fromEntries(notArmors), // all the other fields are preserved
    }
  })

Deno.test(
  "merge complex armor params",
  async (t) => {
    await t.step(
      "can parse full entry",
      () =>
        assertEquals(
          mergeComplexArmorParamsSchema.parse(before),
          after as unknown,
        ),
    )
  },
)
