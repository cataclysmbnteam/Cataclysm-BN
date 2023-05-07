import { replaceWith } from "./replace_with.ts"
import { assertEquals } from "https://deno.land/std@0.186.0/testing/asserts.ts"

const toReplace = {
  "id": "stockings_tent_legs",
  "description": "this entry needs to be replaced with the vanilla one",
}
const expected = {
  "id": "stockings_tent_legs",
  "type": "ARMOR",
  "name": { "str": "pair of tentacle stockings", "str_pl": "pairs of tentacle stockings" },
  "description":
    "Six long cotton tubes sized to fit over tentacles and help protect them from the cold.",
  "weight": "270 g",
  "volume": "1500 ml",
  "price": 1200,
  "price_postapoc": 250,
  "material": ["cotton"],
  "symbol": "[",
  "looks_like": "leg_warmers",
  "color": "dark_gray",
  "covers": ["feet", "legs"],
  "coverage": 75,
  "encumbrance": 10,
  "warmth": 10,
  "material_thickness": 1,
  "flags": ["VARSIZE", "SKINTIGHT", "OVERSIZE"],
}

Deno.test("replaceWith", () => {
  const idToReplace = "stockings_tent_legs"
  const replaced = replaceWith([toReplace])([expected])(idToReplace)

  assertEquals(replaced, [expected])
})
