import { multiplyVolume, multiplyWeight, Volume, Weight } from "catjazz/units/mod.ts"

export type Item = {
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
  desc?: string
  material: string
}
const scarf_base = {
  type: "TOOL_ARMOR",
  category: "clothing",
  symbol: "[",
  covers: ["mouth"],
  flags: ["OUTER"],
} as const
export const desc = ({ desc = "long knitted", material }: Scarf) =>
  ({
    description: `A ${desc} ${material} scarf, worn over the mouth for warmth.`,
    material: [material],
  }) as const
export const knit_scarf_base = {
  id: "knit_scarf",
  name: { str: "knit scarf", str_pl: "knit scarves" },
  ...scarf_base,
  ...desc({ material: "cotton" }),
  color: "dark_gray",
  weight: "96 g",
  volume: "750 ml",
  price: 2000,
  price_postapoc: 100,
  material_thickness: 2,
  to_hit: -3,
} as const
export const tightened = <const T extends Item>({ id, description, ...rest }: T) =>
  ({
    id,
    description: `${description}  Use it to loosen it if you get too warm.`,
    use_action: {
      type: "transform",
      msg: "You loosen your %s.",
      target: `${id}_loose`,
      menu_text: "Loosen",
    },
    ...rest,
  }) as const
type LoosenOption = { tightenDesc?: string }
const defaultLoosenOption = { tightenDesc: "tighter" } satisfies LoosenOption
export const loosened = <const T extends Item, const U extends LoosenOption>(
  { id, name: { str, str_pl }, description, flags, ...rest }: T,
  option?: U,
) => {
  const { tightenDesc } = { ...defaultLoosenOption, ...option }
  return ({
    id: `${id}_loose`,
    name: { str: `${str} (loose)`, str_pl: `${str_pl} (loose)` },
    description: `${description}  Use it to wear it tighter if you get too cold.`,
    repairs_like: id,
    revert_to: id,
    use_action: {
      type: "transform",
      msg: `You wrap your scarf ${tightenDesc}.`,
      target: id,
      menu_text: "Wrap tighter",
    },
    flags: [...flags, "ALLOWS_NATURAL_ATTACKS"],
    ...rest,
  }) as const
}
export const longer = <const T extends Item, const U extends string>(
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
  flags: [...flags, "OVERSIZE", "POCKETS"],
  ...rest,
  price: price * 1.5,
  weight: multiplyWeight(rest.weight, 2),
  volume: multiplyVolume(rest.volume, 5 / 3),
})
