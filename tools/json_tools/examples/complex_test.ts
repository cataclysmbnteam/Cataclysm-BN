import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/index.ts"

const before = {
  parameter1: "value",
  other_parameter1: "value",
  parameter2: "value",
  other_parameter2: "value",
  parameter3: "value",
  other_parameter4: "value",
  parameter4: "value",
}

const after = {
  "parameter": [
    "parameter1_value",
    "parameter2_value",
    "parameter3_value",
    "parameter4_value",
  ],
  other_parameter1: "value",
  other_parameter2: "value",
  other_parameter4: "value",
}

export const mergeComplexParamsSchema = z.object({
  parameter1: z.string(),
  parameter2: z.string(),
  parameter3: z.string(),
  parameter4: z.string(),
})
  .passthrough()
  .transform(({
    parameter1, parameter2, parameter3, parameter4, ...args }) => ({
    parameter: [
      `parameter1_${parameter1}`,
      `parameter2_${parameter2}`,
      `parameter3_${parameter3}`,
      `parameter4_${parameter4}`,
    ],
    ...args,
  }))

Deno.test("complex merging test", async (t) => {
  await t.step(
    "can parse full entry",
    () => assertEquals(mergeComplexParamsSchema.parse(before), after as unknown),
  )
})
