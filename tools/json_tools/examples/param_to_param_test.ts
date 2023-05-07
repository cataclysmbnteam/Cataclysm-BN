import { assertEquals } from "https://deno.land/std@0.186.0/testing/asserts.ts"

import { testSchema } from "../utils/test_schema.ts"
import {
  mapMaterialSchema,
  mapNameSchema,
  mapWeightSchema,
  paramSchema,
  volumeMapSchema,
} from "./param_to_param.ts"

Deno.test("weightSchema", async (t) =>
  await testSchema(t.step)(mapWeightSchema)([
    { input: 390, expected: "390 g" },
    { input: 1020, expected: "1020 g" },
    { input: 3000, expected: "3 kg" },
    { input: "390 g", expected: "390 g" },
    { id: "100 kg" },
  ]))

Deno.test("volumeSchema", async (t) =>
  await testSchema(t.step)(volumeMapSchema)([
    { input: 3, expected: "750 ml" },
    { input: 4, expected: "1 L" },
    { id: "1 L" },
  ]))

Deno.test("nameSchema", async (t) =>
  await testSchema(t.step)(mapNameSchema)([
    { input: "insert_name_here", expected: { str_sp: "insert_name_here" } },
    { id: { str: "insert_name_here" } },
    { id: { str_sp: "insert_name_here" } },
    { id: { str: "singular", str_pl: "plural" } },
  ]))

Deno.test("materialSchema", async (t) =>
  await testSchema(t.step)(mapMaterialSchema)([
    { input: "nanite", expected: ["nanite"] },
    { id: ["nanite"] },
  ]))

const before = {
  weight: 390,
  volume: 4,
  name: "insert_name_here",
  material: "nanite",
}

const after = {
  weight: "390 g",
  volume: "1 L",
  name: { str_sp: "insert_name_here" },
  material: ["nanite"],
}

Deno.test("migrate JSON entry parameters by parameters", async (t) => {
  await t.step(
    "can parse partial entry",
    () =>
      assertEquals(
        paramSchema.parse({ weight: 100, foo: 3 }),
        { weight: "100 g", foo: 3 } as unknown,
      ),
  )

  await t.step("can parse full entry", () => assertEquals(paramSchema.parse(before), after))
  await t.step("parsing is idempotent", () => assertEquals(paramSchema.parse(after), after))
  await t.step("parsing preserves order", () =>
    assertEquals(
      paramSchema.parse(JSON.parse(`{
        "name": "insert_name_here",
        "weight": 390,
        "bar": 2,
        "volume": 4,
        "material": "nanite",
        "foo": 3
      }`)),
      {
        name: { str_sp: "insert_name_here" },
        weight: "390 g",
        bar: 2,
        volume: "1 L",
        material: ["nanite"],
        foo: 3,
      } as unknown,
    ))
  //   await t.step("parsing preserves unused fields", () =>
  // assertEquals(
})
