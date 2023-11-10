import { desc, knit_scarf_base, longer, loosened, tightened } from "./scarfs_transform.ts"
export const knit_scarf = {
  ...tightened(knit_scarf_base),
  coverage: 85,
  warmth: 30,
  encumbrance: 3,
  environmental_protection: 2,
}

export const knit_scarf_loose = {
  ...loosened(knit_scarf_base),
  warmth: 15,
  coverage: 45,
  encumbrance: 2,
  environmental_protection: 1,
}

const long_knit_scarf_base = {
  ...longer(knit_scarf_base, "A really long knitted cotton scarf"),
  price: 3000,
} as const

export const long_knit_scarf = {
  ...tightened(long_knit_scarf_base),
  warmth: 30,
  coverage: 85,
  environmental_protection: 2,
  encumbrance: 3,
}

export const long_knit_scarf_loose = {
  ...loosened(long_knit_scarf_base),
  warmth: 15,
  coverage: 45,
  environmental_protection: 1,
  encumbrance: 2,
} as const

const wool_scarf_base = {
  ...knit_scarf_base,
  id: "scarf",
  name: { str: "wool scarf", str_pl: "wool scarves" },
  description: "A long wool scarf, worn over the mouth for warmth.",
  color: "brown",
  material: ["wool"],
  weight: "80 g",
  price: 3800,
  price_postapoc: 50,
} as const

const wool_tight_stat = {
  warmth: 50,
  coverage: 85,
  encumbrance: 3,
  environmental_protection: 2,
} as const

const wool_loose_stat = {
  warmth: 25,
  environmental_protection: 1,
  encumbrance: 2,
  coverage: 45,
} as const

export const wool_scarf = { ...tightened(wool_scarf_base), ...wool_tight_stat } as const
export const wool_scarf_loose = {
  ...loosened(wool_scarf_base, { tightenDesc: "a bit tighter" }),
  ...wool_loose_stat,
} as const

const wool_scarf_long_base = {
  ...longer(wool_scarf_base, "A really long wool scarf"),
  id: "scarf_long",
  price_postapoc: 100,
  price: 4500,
} as const

export const wool_scarf_long = {
  ...tightened(wool_scarf_long_base),
  ...wool_tight_stat,
}
export const wool_scarf_long_loose = {
  ...loosened(wool_scarf_long_base),
  ...wool_loose_stat,
}

const scarf_fur_base = {
  ...wool_scarf_base,
  id: "scarf_fur",
  name: { str: "fur scarf", str_pl: "fur scarves" },
  ...desc({ desc: "long", material: "fur" }),
  price: 9000,
  price_postapoc: 250,
  weight: "140 g",
  volume: "1 L",
  material_thickness: 3,
} as const

export const scarf_fur = {
  ...tightened(scarf_fur_base),
  warmth: 70,
  environmental_protection: 3,
  encumbrance: 10,
  coverage: 85,
} as const
export const scarf_fur_loose = {
  ...loosened(scarf_fur_base, { tightenDesc: "a bit tighter" }),
  warmth: 35,
  environmental_protection: 2,
  encumbrance: 10,
  coverage: 45,
} as const
const scarf_fur_long_base = {
  ...longer(scarf_fur_base, "A really long fur scarf"),
  id: "scarf_fur_long",
  price: 17700,
  price_postapoc: 300,
  volume: "2 L",
} as const
export const scarf_fur_long = {
  ...tightened(scarf_fur_long_base),
  warmth: 70,
  environmental_protection: 3,
  encumbrance: 10,
  coverage: 85,
} as const
export const scarf_fur_long_loose = {
  ...loosened(scarf_fur_long_base),
  warmth: 35,
  environmental_protection: 2,
  coverage: 45,
  material_thickness: 3,
} as const

const patchwork_scarf_base = {
  ...knit_scarf_base,
  id: "patchwork_scarf",
  name: { str: "simple patchwork scarf", str_pl: "simple patchwork scarves" },
  ...desc({ desc: "simple and light", material: "cloth" }),
  material: ["cotton"],
  color: "light_gray",
  weight: "60 g",
  volume: "500 ml",
  price: 1000,
  price_postapoc: 50,
  material_thickness: 1,
  environmental_protection: 1,
} as const

export const patchwork_scarf = {
  ...tightened(patchwork_scarf_base),
  warmth: 20,
  encumbrance: 3,
  coverage: 85,
} as const

export const patchwork_scarf_loose = {
  ...loosened(patchwork_scarf_base),
  warmth: 10,
  encumbrance: 2,
  coverage: 45,
} as const
const long_patchwork_scarf_base = {
  ...longer(patchwork_scarf_base, "A very long light cloth scarf"),
  name: { str: "long patchwork scarf", str_pl: "long patchwork scarves" },
  id: "long_patchwork_scarf",
  price_postapoc: 100,
  volume: "1 L",
} as const
export const long_patchwork_scarf = {
  ...tightened(long_patchwork_scarf_base),
  warmth: 20,
  environmental_protection: 1,
  encumbrance: 3,
  coverage: 85,
} as const
export const long_patchwork_scarf_loose = {
  ...loosened(long_patchwork_scarf_base),
  warmth: 10,
  environmental_protection: 1,
  encumbrance: 2,
  coverage: 45,
} as const
