import { assertEquals } from "https://deno.land/std@0.201.0/assert/assert_equals.ts"
import scarfs from "./scarfs.json" with { type: "json" }
import { multiplyVolume, multiplyWeight, Volume, Weight } from "catjazz/units/mod.ts"

type Item = {
  id: string
  type: "TOOL_ARMOR"
  category: "clothing"
  symbol: "["
  name: { str: string; str_pl: string }
  description: string
  covers: readonly string[]
  flags: readonly string[]
}

type Scarf = {
  long: boolean
  material: string
}

const scarf_base = {
  type: "TOOL_ARMOR",
  category: "clothing",
  symbol: "[",
  covers: ["mouth"],
  flags: ["OUTER"],
} as const

const desc = ({ long, material }: Scarf) =>
  ({
    description: `A long knitted ${material} scarf, worn over the mouth for warmth.`,
    material: [material],
  }) as const

const knit_scarf_base = {
  id: "knit_scarf",
  name: { str: "knit scarf", str_pl: "knit scarves" },
  ...scarf_base,
  ...desc({ long: false, material: "cotton" }),
  color: "dark_gray",
  weight: "96 g",
  volume: "750 ml",
  price: 2000,
  price_postapoc: 100,
  material_thickness: 2,
  to_hit: -3,
} as const

const tightened = <const T extends Item>({ id, name, description, ...rest }: T) =>
  ({
    id,
    name,
    description: `${description}  Use it to loosen it if you get too warm.`,
    ...rest,
    use_action: {
      type: "transform",
      msg: "You loosen your %s.",
      target: `${id}_loose`,
      menu_text: "Loosen",
    },
  }) as const

const loosened = <const T extends Item>(
  { id, name: { str, str_pl }, description, flags, ...rest }: T,
) =>
  ({
    id: `${id}_loose`,
    name: { str: `${str} (loose)`, str_pl: `${str_pl} (loose)` },
    description: `${description}  Use it to wear it tighter if you get too cold.`,
    ...rest,
    repairs_like: id,
    revert_to: id,
    use_action: {
      type: "transform",
      msg: "You wrap your scarf tighter.",
      target: id,
      menu_text: "Wrap tighter",
    },
    flags: [...flags, "ALLOWS_NATURAL_ATTACKS"],
  }) as const

const longer = <const T extends Item, const U extends string>(
  { id, name: { str, str_pl }, description, price, flags, ...rest }: T & {
    price: number
    weight: Weight
    volume: Volume
  },
  newDescription: U,
) => ({
  id: `long_${id}`,
  name: { str: `long ${str}`, str_pl: `long ${str_pl}` },
  description: `${newDescription}, ${
    description.split(", ")[1]
  }  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.`,
  price: price * 1.5,
  flags: [...flags, "OVERSIZE", "POCKETS"],
  ...rest,
  weight: multiplyWeight(rest.weight, 2),
  volume: multiplyVolume(rest.volume, 5 / 3),
})

const knit_scarf = {
  ...tightened(knit_scarf_base),
  coverage: 85,
  warmth: 30,
  encumbrance: 3,
  environmental_protection: 2,
}

const knit_scarf_loose = {
  ...loosened(knit_scarf_base),
  warmth: 15,
  coverage: 45,
  encumbrance: 2,
  environmental_protection: 1,
}

const long_knit_scarf_base = longer(knit_scarf_base, "A really long knitted cotton scarf")
const long_knit_scarf = {
  ...tightened(long_knit_scarf_base),
  warmth: 30,
  coverage: 85,
  environmental_protection: 2,
  encumbrance: 3,
}

const long_knit_scarf_loose = {
  ...loosened(long_knit_scarf_base),
  warmth: 15,
  coverage: 45,
  environmental_protection: 1,
  encumbrance: 2,
} as const

const findScarf = ({ id }: Item) => scarfs.find((it) => it.id === id)! as any
const checkScarf = (scarf: Item, looseScarf: Item) => async (t: Deno.TestContext) => {
  await t.step(scarf.id, () => assertEquals(scarf, findScarf(scarf)))
  await t.step(looseScarf.id, () => assertEquals(looseScarf, findScarf(looseScarf)))
}

Deno.test("knit_scarf", checkScarf(knit_scarf, knit_scarf_loose))
Deno.test("long_knit_scarf", checkScarf(long_knit_scarf, long_knit_scarf_loose))

const scarf = {
  "id": "scarf",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "wool scarf", "str_pl": "wool scarves" },
  "description":
    "A long wool scarf, worn over the mouth for warmth.  Use it to loosen it if you get too warm.",
  "price": 3800,
  "price_postapoc": 50,
  "material": ["wool"],
  "weight": "80 g",
  "volume": "750 ml",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "scarf_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OUTER"],
  "warmth": 50,
  "environmental_protection": 2,
  "encumbrance": 3,
  "coverage": 85,
  "material_thickness": 2,
}
const scarf_loose = {
  "id": "scarf_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "scarf",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "wool scarf (loose)", "str_pl": "wool scarves (loose)" },
  "description":
    "A long wool scarf, worn over the mouth for warmth.  Use it to wear it tighter if you get too cold.",
  "price": 3800,
  "price_postapoc": 50,
  "material": ["wool"],
  "weight": "80 g",
  "volume": "750 ml",
  "to_hit": -3,
  "revert_to": "scarf",
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf a bit tighter.",
    "target": "scarf",
    "menu_text": "Wrap tighter",
  },
  "covers": ["mouth"],
  "flags": ["OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 25,
  "environmental_protection": 1,
  "encumbrance": 2,
  "coverage": 45,
  "material_thickness": 2,
}
const scarf_long = {
  "id": "scarf_long",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "long wool scarf", "str_pl": "long wool scarves" },
  "description":
    "A really long wool scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to loosen it if you get too warm.",
  "price": 4500,
  "price_postapoc": 100,
  "material": ["wool"],
  "weight": "160 g",
  "volume": "1250 ml",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "scarf_long_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER"],
  "warmth": 50,
  "environmental_protection": 2,
  "encumbrance": 3,
  "coverage": 85,
  "material_thickness": 2,
}
const scarf_long_loose = {
  "id": "scarf_long_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "scarf_long",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "long wool scarf (loose)", "str_pl": "long wool scarves (loose)" },
  "description":
    "A really long wool scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to wear it tighter if you get too cold.",
  "price": 4500,
  "price_postapoc": 100,
  "material": ["wool"],
  "weight": "160 g",
  "volume": "1250 ml",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf tighter.",
    "target": "scarf_long",
    "menu_text": "Wrap tighter",
  },
  "revert_to": "scarf_long",
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 25,
  "environmental_protection": 1,
  "encumbrance": 2,
  "coverage": 45,
  "material_thickness": 2,
}
const scarf_fur = {
  "id": "scarf_fur",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "fur scarf", "str_pl": "fur scarves" },
  "description":
    "A long fur scarf, worn over the mouth for warmth.  Use it to loosen it if you get too warm.",
  "price": 9000,
  "price_postapoc": 250,
  "material": ["fur"],
  "weight": "140 g",
  "volume": "1 L",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "scarf_fur_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OUTER"],
  "warmth": 70,
  "environmental_protection": 3,
  "encumbrance": 10,
  "coverage": 85,
  "material_thickness": 3,
}
const scarf_fur_loose = {
  "id": "scarf_fur_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "scarf_fur",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "fur scarf (loose)", "str_pl": "fur scarves (loose)" },
  "description":
    "A long fur scarf, worn over the mouth for warmth.  Use it to wear it tighter if you get too cold.",
  "price": 9000,
  "price_postapoc": 250,
  "material": ["fur"],
  "weight": "140 g",
  "volume": "1 L",
  "to_hit": -3,
  "revert_to": "scarf_fur",
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf a bit tighter.",
    "target": "scarf_fur",
    "menu_text": "Wrap tighter",
  },
  "covers": ["mouth"],
  "flags": ["OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 35,
  "environmental_protection": 2,
  "encumbrance": 10,
  "coverage": 45,
  "material_thickness": 3,
}
const scarf_fur_long = {
  "id": "scarf_fur_long",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "long fur scarf", "str_pl": "long fur scarves" },
  "description":
    "A really long fur scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to loosen it if you get too warm.",
  "price": 17700,
  "price_postapoc": 300,
  "material": ["fur"],
  "weight": "280 g",
  "volume": "2 L",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "scarf_fur_long_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER"],
  "warmth": 70,
  "environmental_protection": 3,
  "encumbrance": 10,
  "coverage": 85,
  "material_thickness": 3,
}
const scarf_fur_long_loose = {
  "id": "scarf_fur_long_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "scarf_fur_long",
  "symbol": "[",
  "color": "brown",
  "name": { "str": "long fur scarf (loose)", "str_pl": "long fur scarves (loose)" },
  "description":
    "A really long fur scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to wear it tighter if you get too cold.",
  "price": 17700,
  "price_postapoc": 300,
  "material": ["fur"],
  "weight": "280 g",
  "volume": "2 L",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf tighter.",
    "target": "scarf_fur_long",
    "menu_text": "Wrap tighter",
  },
  "revert_to": "scarf_fur_long",
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 35,
  "environmental_protection": 2,
  "coverage": 45,
  "material_thickness": 3,
}
const patchwork_scarf = {
  "id": "patchwork_scarf",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "light_gray",
  "name": { "str": "simple patchwork scarf", "str_pl": "simple patchwork scarves" },
  "description":
    "A simple and light cloth scarf, worn over the mouth for warmth.  Use it to loosen it if you get too warm.",
  "price": 1000,
  "price_postapoc": 50,
  "material": ["cotton"],
  "weight": "60 g",
  "volume": "500 ml",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "patchwork_scarf_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OUTER"],
  "warmth": 20,
  "environmental_protection": 1,
  "encumbrance": 3,
  "coverage": 85,
  "material_thickness": 1,
}
const patchwork_scarf_loose = {
  "id": "patchwork_scarf_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "patchwork_scarf",
  "symbol": "[",
  "color": "light_gray",
  "name": { "str": "simple patchwork scarf (loose)", "str_pl": "simple patchwork scarves (loose)" },
  "description":
    "A simple and light cloth scarf, worn over the mouth for warmth.  Use it to wear it tighter if you get too cold.",
  "price": 1000,
  "price_postapoc": 50,
  "material": ["cotton"],
  "weight": "60 g",
  "volume": "500 ml",
  "to_hit": -3,
  "revert_to": "patchwork_scarf",
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf tighter.",
    "target": "patchwork_scarf",
    "menu_text": "Wrap tighter",
  },
  "covers": ["mouth"],
  "flags": ["OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 10,
  "environmental_protection": 1,
  "encumbrance": 2,
  "coverage": 45,
  "material_thickness": 1,
}
const long_patchwork_scarf = {
  "id": "long_patchwork_scarf",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "symbol": "[",
  "color": "light_gray",
  "name": { "str": "long patchwork scarf", "str_pl": "long patchwork scarves" },
  "description":
    "A very long light cloth scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to loosen it if you get too warm.",
  "price": 1500,
  "price_postapoc": 100,
  "material": ["cotton"],
  "weight": "120 g",
  "volume": "1 L",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You loosen your %s.",
    "target": "long_patchwork_scarf_loose",
    "menu_text": "Loosen",
  },
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER"],
  "warmth": 20,
  "environmental_protection": 1,
  "encumbrance": 3,
  "coverage": 85,
  "material_thickness": 1,
}
const long_patchwork_scarf_loose = {
  "id": "long_patchwork_scarf_loose",
  "type": "TOOL_ARMOR",
  "category": "clothing",
  "repairs_like": "long_patchwork_scarf",
  "symbol": "[",
  "color": "light_gray",
  "name": { "str": "long patchwork scarf (loose)", "str_pl": "long patchwork scarves (loose)" },
  "description":
    "A very long light cloth scarf, worn over the mouth for warmth.  With the extra length, it's enough to handle nonstandard facial features and accommodate your hands too.  Use it to wear it tighter if you get too cold.",
  "price": 1500,
  "price_postapoc": 100,
  "material": ["cotton"],
  "weight": "120 g",
  "volume": "1 L",
  "to_hit": -3,
  "use_action": {
    "type": "transform",
    "msg": "You wrap your scarf tighter.",
    "target": "long_patchwork_scarf",
    "menu_text": "Wrap tighter",
  },
  "revert_to": "long_patchwork_scarf",
  "covers": ["mouth"],
  "flags": ["OVERSIZE", "POCKETS", "OUTER", "ALLOWS_NATURAL_ATTACKS"],
  "warmth": 10,
  "environmental_protection": 1,
  "encumbrance": 2,
  "coverage": 45,
  "material_thickness": 1,
}
