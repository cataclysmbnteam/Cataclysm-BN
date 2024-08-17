import * as v from "jsr:@valibot/valibot"

import { assertEquals } from "jsr:@std/assert/equals"
import { parseMany, recursivelyReadJSON } from "./utils.ts"
import { assertExists } from "jsr:@std/assert"

type Item = v.InferOutput<typeof Item>
const Item = v.looseObject({
  id: v.string(),
  name: v.optional(v.any()),
  type: v.optional(v.string()),
  category: v.optional(v.string()),
})

type RenamedItem = v.InferOutput<typeof RenamedItem>
const RenamedItem = v.looseObject({
  id: v.string(),
  name: v.optional(v.any()),
  type: v.optional(v.string()),
  "copy-from": v.optional(v.string()),
  category: v.string(),
})

const parseItems = parseMany(Item)
const parseRenamedItems = parseMany(RenamedItem)

const showId = (item: Item) => `${item.id} (${item.type})`

const jsons = await recursivelyReadJSON("data/json/items")
  .then(parseItems).then((xs) => Map.groupBy(xs, (x) => x.id))

const overWritten = await recursivelyReadJSON(
  "data/mods/Item_Category_Overhaul",
).then(parseRenamedItems)

overWritten.forEach((item) =>
  Deno.test({
    name: showId(item),
    sanitizeOps: false,
    sanitizeResources: false,
    sanitizeExit: false,
    fn: () => {
      const showName = showId(item)
      const originals = jsons.get(item.id)
      assertExists(originals, `${showName} exists in original items`)
      const original = originals.find((x) => x.type === item.type)
      assertExists(
        original,
        `${showName} has matching 'type' as original, found: ${originals.map(showId)}`,
      )
      assertEquals(item.id, item["copy-from"], `${showName}'s 'id' and 'copy-from' are different`)
      assertEquals(original?.name, item.name, `${showName}'s 'name' is different from original`)
    },
  })
)
