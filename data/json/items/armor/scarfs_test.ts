import { assertEquals } from "https://deno.land/std@0.201.0/assert/assert_equals.ts"
import scarfs from "./scarfs.json" with { type: "json" }
import {
  knit_scarf,
  knit_scarf_loose,
  long_knit_scarf,
  long_knit_scarf_loose,
  long_patchwork_scarf,
  long_patchwork_scarf_loose,
  patchwork_scarf,
  patchwork_scarf_loose,
  scarf_fur,
  scarf_fur_long,
  scarf_fur_long_loose,
  scarf_fur_loose,
  wool_scarf,
  wool_scarf_long,
  wool_scarf_long_loose,
  wool_scarf_loose,
} from "./scarfs.ts"
import { Item } from "./scarfs_transform.ts"

const findScarf = ({ id }: Item) => scarfs.find((it) => it.id === id)! as any
const checkScarf = (scarf: Item, looseScarf: Item) => async (t: Deno.TestContext) => {
  await t.step(scarf.id, () => assertEquals(scarf, findScarf(scarf)))
  await t.step(looseScarf.id, () => assertEquals(looseScarf, findScarf(looseScarf)))
}
Deno.test("knit_scarf", checkScarf(knit_scarf, knit_scarf_loose))
Deno.test("long_knit_scarf", checkScarf(long_knit_scarf, long_knit_scarf_loose))
Deno.test("scarf", checkScarf(wool_scarf, wool_scarf_loose))
Deno.test("scarf_long", checkScarf(wool_scarf_long, wool_scarf_long_loose))
Deno.test("scarf_fur", checkScarf(scarf_fur, scarf_fur_loose))
Deno.test("scarf_fur_long", checkScarf(scarf_fur_long, scarf_fur_long_loose))
Deno.test("patchwork_scarf", checkScarf(patchwork_scarf, patchwork_scarf_loose))
Deno.test("long_patchwork_scarf", checkScarf(long_patchwork_scarf, long_patchwork_scarf_loose))
