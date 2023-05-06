import {
  brightCyan as bc,
  brightYellow as by,
  gray,
} from "https://deno.land/std@0.182.0/fmt/colors.ts"
import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

import { match, P } from "npm:ts-pattern"
import { concurrently, Step } from "./test_concurrently.ts"

type Schema = z.ZodEffects<z.ZodTypeAny>

type TestCase =
  | { input: unknown; expected: unknown }
  | { id: unknown }
/** get test metadata */
const unpack = (testCase: TestCase) =>
  match(testCase)
    .with(
      { id: P._ },
      ({ id }) => ({ name: (`id  : ${gray(Deno.inspect(id))}`), input: id, expected: id }),
    )
    .with(
      { input: P._, expected: P._ },
      ({ input, expected }) => {
        const inp = Deno.inspect(input)
        const exp = Deno.inspect(expected)
        return { name: `maps: ${bc(inp)} -> ${by(exp)}`, input, expected }
      },
    )
    .exhaustive()

/** test a zod schema with test values. */
type TestSchema = (step: Step) => (schema: Schema) => (cases: readonly TestCase[]) => Promise<void>

export const testSchema: TestSchema = (step) => (schema) => async (cases) => {
  const testTransform = (testCase: TestCase) => {
    const { name, input, expected } = unpack(testCase)

    concurrently(step)({ name, fn: () => assertEquals(schema.parse(input), expected) })
  }

  await Promise.all(cases.map((x) => testTransform(x)))
}
