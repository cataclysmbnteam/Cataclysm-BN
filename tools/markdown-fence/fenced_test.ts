import { assertEquals } from "https://deno.land/std@0.177.0/testing/asserts.ts"
import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"
import * as yaml from "https://deno.land/std@0.179.0/encoding/yaml.ts"

import { converter } from "./converter.ts"
import { stateFns } from "./state_fns.ts"

const testSchema = z.record(z.object({
  input: z.string(),
  expected: z.string(),
})).transform((entries) => new Map(Object.entries(entries)))

const convert = converter(stateFns("sh"))
const testPath = new URL("", import.meta.url).pathname.replace(".ts", ".yaml")
const tests = testSchema.parse(yaml.parse(await Deno.readTextFile(testPath)))

tests.forEach(({ input, expected }, name) =>
  Deno.test(name, () => assertEquals(convert(input.split("\n")).join("\n"), expected))
)
