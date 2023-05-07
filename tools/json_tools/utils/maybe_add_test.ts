import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { maybeAdd } from "./maybe_add.ts"
import { concurrently } from "./test_concurrently.ts"

const cases = [
  { input: [], expected: {} },
  { input: {}, expected: {} },
  { input: [3], expected: { c: [3] } },
]

Deno.test("maybeAdd", async (t) => {
  await Promise.all(cases.map(({ input, expected }) =>
    concurrently(t.step)({
      name: `maybeAdd("c")(${Deno.inspect(input)}) -> ${Deno.inspect(expected)}`,
      fn: () => assertEquals(maybeAdd("c")(input), expected as unknown),
    })
  ))
})
