import { assertEquals, assertThrows } from "https://deno.land/std@0.186.0/testing/asserts.ts"
import { recoverNMigrationSchema } from "./recover_n.ts"

Deno.test("with other effects", () => {
  const input = {
    effects: ["RECOVER_1", "RECOVER_2", "FOO"],
  }
  const expected = {
    dont_recover_one_in: 1,
    effects: ["FOO"],
  }
  const actual = recoverNMigrationSchema.parse(input)
  assertEquals(actual, expected)
})
Deno.test("with only recover effects", () => {
  const input = {
    foo: 123,
    bar: "bar",
    effects: ["RECOVER_100"],
  }
  const expected = {
    foo: 123,
    bar: "bar",
    dont_recover_one_in: 100,
  }
  const actual = recoverNMigrationSchema.parse(input)
  assertEquals(actual, expected)
})
Deno.test("with no effects", () => {
  assertThrows(() =>
    recoverNMigrationSchema.parse({
      foo: 123,
      bar: "bar",
    })
  )
})

const input = {
  "type": "AMMO",
  "id": "arrow_metal",
  "price": 1200,
  "name": { "str": "aluminum broadhead arrow" },
  "symbol": "=",
  "color": "green",
  "looks_like": "arrow_fire_hardened_fletched",
  "description":
    "A fletched aluminum arrow shaft with a bladed tip.  Useful for maximizing damage to the target.  Stands a good chance of remaining intact once fired.",
  "material": ["aluminum", "steel"],
  "volume": "250 ml",
  "price_postapoc": 500,
  "weight": "30 g",
  "bashing": 3,
  "cutting": 2,
  "ammo_type": "arrow",
  "range": 2,
  "dispersion": 75,
  "loudness": 0,
  "count": 10,
  "effects": ["RECOVER_35"],
}
const ouput = {
  "type": "AMMO",
  "id": "arrow_metal",
  "price": 1200,
  "name": { "str": "aluminum broadhead arrow" },
  "symbol": "=",
  "color": "green",
  "looks_like": "arrow_fire_hardened_fletched",
  "description":
    "A fletched aluminum arrow shaft with a bladed tip.  Useful for maximizing damage to the target.  Stands a good chance of remaining intact once fired.",
  "material": ["aluminum", "steel"],
  "volume": "250 ml",
  "price_postapoc": 500,
  "weight": "30 g",
  "bashing": 3,
  "cutting": 2,
  "ammo_type": "arrow",
  "range": 2,
  "dispersion": 75,
  "loudness": 0,
  "count": 10,
  dont_recover_one_in: 35,
}

Deno.test("test with actual data", () => {
  const actual = recoverNMigrationSchema.parse(input)
  assertEquals(actual, ouput as unknown)
})
