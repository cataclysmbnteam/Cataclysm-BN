import { assertEquals } from "https://deno.land/std@0.186.0/testing/asserts.ts"
import { z } from "https://deno.land/x/zod@v3.20.5/index.ts"

/**
 * this demonstrates how to merge armor params, the brute way
 */

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
}

const after = {
  "id": "item_id1",
  "name": "item1",
  "material": ["nanite"],
  "weight": "460 g",
  "volume": "430 ml",
  "armor": { "bash": 46, "cut": 30, "bullet": 30, "acid": 80 },
}

const mergeComplexArmorParamsSchema = z.object({
  armor_bullet: z.number(),
  armor_bash: z.number(),
  armor_cut: z.number(),
  armor_acid: z.number(),
  material: z.string(),
}) // we're looking for a object which has all these four fields
  .passthrough() // passthrough keeps all the other fields untouched
  // now we create a NEW object using by feeding a function to transform method,
  // which takes all these four fields as parameter,
  // and return a new object with armor field added
  .transform(({
    armor_bullet,
    armor_acid,
    armor_bash,
    armor_cut,
    material,
    ...args // all the other fields
  }) => (
    // creates and returns JSON object with armor field added
    {
      armor: {
        bash: armor_bash, // armor object has bash property, which is same value to armor_bash variable
        cut: armor_cut,
        bullet: armor_bullet,
        acid: armor_acid,
      },
      material: [material], // put it in array
      ...args, // all the other fields are preserved
    }
  ))

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
