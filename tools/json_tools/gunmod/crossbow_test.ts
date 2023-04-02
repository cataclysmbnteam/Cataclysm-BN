import { assertEquals } from "https://deno.land/std@0.182.0/testing/asserts.ts"
import { migrations } from "./gunmod.ts"

Deno.test("crossbow category conversion", async (t) => {
  const { crossbow } = migrations

  await t.step("[pistol, crossbow] -> [[ H_XBOWS ], [ XBOWS ]]", () => {
    assertEquals(crossbow(["pistol", "crossbow"]), [["H_XBOWS"], ["XBOWS"]])
  })
  await t.step("[crossbow] -> [[ XBOWS ]]", () => {
    assertEquals(crossbow(["crossbow"]), [["XBOWS"]])
  })
  await t.step("[pistol, crossbow, rifle] -> [[ H_XBOWS ], [ XBOWS ]]", () => {
    assertEquals(crossbow(["pistol", "crossbow", "rifle"]), [["H_XBOWS"], ["XBOWS"]])
  })
  await t.step("[pistol, rifle] -> []", () => {
    assertEquals(crossbow(["pistol", "rifle"]), [])
  })
})
