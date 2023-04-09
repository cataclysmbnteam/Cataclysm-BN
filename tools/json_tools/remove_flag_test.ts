import { diff, Flaggable, flaggable, removeFlags } from "./remove_flag.ts"
import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { genericCataTransformer } from "./parse.ts"
import { removeObjectKey } from "./transform.ts"

const cases = [
  {
    "type": "COMESTIBLE",
    "id": "meat_aspic",
    "name": { "str": "aspic" },
    "conditional_names": [
      { "type": "COMPONENT_ID", "condition": "mutant", "name": { "str_sp": "abomination %s" } },
      { "type": "FLAG", "condition": "CANNIBALISM", "name": { "str_sp": "amoral %s" } },
    ],
    "weight": "154 g",
    "color": "red",
    "spoils_in": "3 days 8 hours",
    "comestible_type": "FOOD",
    "symbol": "%",
    "calories": 213,
    "//":
      "Same as the meat it's been made from.  It's additional ingredient compared to smoking, jerking, and dehydrating, so there's no incentive otherwise.",
    "description":
      "A dish in which meat or fish is set into a gelatin made from a meat or vegetable stock.",
    "price": 2500,
    "price_postapoc": 150,
    "material": "flesh",
    "volume": "250 ml",
    "vitamins": [["vitA", 10], ["vitC", 15], ["calcium", 2], ["iron", 8]],
    "flags": ["EATEN_COLD"],
  },
  {
    "type": "COMESTIBLE",
    "id": "dry_fish",
    "name": { "str_sp": "dehydrated fish" },
    "copy-from": "fish_cooked",
    "primary_material": "cured_meat",
    "weight": "85 g",
    "color": "light_gray",
    "spoils_in": "360 days",
    "quench": -3,
    "description":
      "Dehydrated fish flakes.  With proper storage, this dried food will remain edible for an incredibly long time.",
    "price": 500,
    "price_postapoc": 250,
    "volume": "125 ml",
    "flags": ["EDIBLE_FROZEN"],
  },
  {
    "id": "ar10",
    "copy-from": "rifle_semi",
    "type": "GUN",
    "name": { "str": "AR-10" },
    "description":
      "Somewhat similar to the later AR-15, the AR-10 is a gas operated, rotating bolt rifle chambered for 7.62x51mm rounds.",
    "weight": "3290 g",
    "volume": "2 L",
    "barrel_length": "250 ml",
    "price": 120000,
    "price_postapoc": 5500,
    "material": ["steel", "plastic"],
    "color": "dark_gray",
    "ammo": "308",
    "ranged_damage": { "damage_type": "bullet", "amount": -1 },
    "dispersion": 150,
    "durability": 7,
    "min_cycle_recoil": 2700,
    "magazines": [["308", ["ar10mag_20rd", "ar10_makeshiftmag"]]],
  },
] as const

Deno.test("flaggable.safeParse", () => {
  const result = cases.reduce((acc, x) => flaggable.safeParse(x).success ? acc + 1 : acc, 0)

  assertEquals(result, 2)
})

Deno.test("removing [a, b, foo] from { flags: [a, b, c, d] } -> { flags: [b, d] }", () => {
  const flags = { flags: ["a", "b", "c", "d"] }
  const toRemove = ["a", "c", "foo"]
  const flagRemoveFn = removeFlags(diff(toRemove))

  assertEquals(flagRemoveFn(flags), { flags: ["b", "d"] })
})

Deno.test("removing [a, b, foo] from { flags: [a, b] } -> {}", () => {
  const flags = { flags: ["a", "b"] }
  const toRemove = ["a", "b", "foo", "bar"]
  const flagRemoveFn = removeFlags(diff(toRemove))

  assertEquals(flagRemoveFn(flags), {})
})

Deno.test("removing EDIBLE_FROZEN from dry_fish", async (t) => {
  const dry_fish = cases[1]
  if (!dry_fish) throw new Error("dry_fish not found")
  const expectedFish = removeObjectKey(dry_fish, "flags")

  const flagFn = removeFlags(diff(["EDIBLE_FROZEN"]))

  await t.step("removeFlags", () => {
    assertEquals(
      flagFn(dry_fish as unknown as Flaggable),
      expectedFish,
    )
  })

  await t.step("flagsStripTransformer", () => {
    const transformer = genericCataTransformer(flaggable)(flagFn)
    const applied = transformer(JSON.stringify(dry_fish))
    const expected = [expectedFish]
    assertEquals(applied, expected)
  })
})
