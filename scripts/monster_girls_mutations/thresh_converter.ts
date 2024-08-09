import * as v from "jsr:@valibot/valibot"

/**
 * @module
 *
 * first input is the outputted file from mutation_converter.ts with the `"extend"`s
 * for the monstergirl categories already put into place.
 * Its second input is the same file as was input into mutation_converter.ts
 * (i.e. Cataclysm-BN/data/json/mutations/mutations.json).
 * It then goes through and finds the mutations that have "threshreq"s in the latter file,
 * checks to see if they're in any of the monster girl categories via the first file,
 * and then adds the relevant thresholds to the "extends" of the first file (and writes them to that file).
 * As before, this was written solely to save myself the tedium of adding the relevant ones by hand.
 *
 * original @author @RobbieNeko
 * @see https://github.com/cataclysmbnteam/Cataclysm-BN/pull/5100#issuecomment-2258396678
 */

//  Dictionary for converting vanilla threshreqs to monstergirl ones.
const threshConvDict = new Map([
  "THRESH_NEKO",
  "THRESH_DOGGIRL",
  "THRESH_DRYAD",
  "THRESH_HARPY",
  "THRESH_BEARGIRL",
  "THRESH_SPIDERGIRL",
  "THRESH_SLIMEGIRL",
  "THRESH_MOUSEGIRL",
  "THRESH_COWGIRL",
].map((x) => [x, x]))

const parseMutation = v.safeParser(v.looseObject({
  id: v.string(),
  extend: v.string(),
}))

const parseThreshold = v.safeParser(v.looseObject({
  id: v.string(),
  threshreq: v.pipe(v.array(v.string()), v.transform((xs) => new Set(xs))),
}))

if (Deno.args.length !== 2) {
  console.error(
    "Usage: deno run --allow-read --allow-write thresh_converter.ts <pre_json_file> <user_json_file>",
  )
  Deno.exit(1)
}

// preJSON: This inputted file should be the one with all the "delete"s and "extend"s
// userJson: This one solely exists to get the mutations with threshreqs
const [preJSON, userJson] = await Promise.all(
  Deno.args.map((x) => Deno.readTextFile(x).then<unknown[]>(JSON.parse)),
)

const preParsedJSON = preJSON.map(parseMutation).filter((x) => x.success)
  .map(({ output }) => output)

// Set for holding the entries in the above file, and loop to extract all of them
const preSet = new Set(preParsedJSON.map(({ id }) => id))

const resultDict = new Map(
  userJson.map(parseThreshold).filter((x) => x.success)
    .map(({ output: { id, threshreq } }) => {
      const thresholds = [...threshreq.intersection(preSet)]
        .flatMap((x) => threshConvDict.get(x) ?? [])

      return [id, thresholds]
    }),
)

const outputJson = preParsedJSON.map((x) => {
  const threshreq = resultDict.get(x.id)
  return threshreq ? { ...x, threshreq } : x
})

await Deno.writeTextFile(Deno.args[0], JSON.stringify(outputJson, null, 2))
