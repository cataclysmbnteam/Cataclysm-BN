import * as v from "jsr:@valibot/valibot"

/**
 * @module
 *
 * takes a file with mutation definitions (i.e. Cataclysm-BN/data/json/mutations/mutations.json)
 * and outputs a new file that has "delete"s in place for all of the mutations with categories defined for them in that inputted file.
 * It creates the base that I (after renaming the resulting file to a more pleasant title)
 * then added all my choices of which mutations a given monster girl category gets,
 * and the script was mostly made to save an enormous amount of time as opposed to doing all those "delete"s by hand.
 *
 * original @author @RobbieNeko
 * @see https://github.com/cataclysmbnteam/Cataclysm-BN/pull/5100#issuecomment-2258396678
 */

// JSON schema parser
const mutationParser = v.safeParser(v.looseObject({
  id: v.string(),
  type: v.string(),
  // Type and ID are assured, but not all mutations have categories
  category: v.array(v.string()),
}))

if (Deno.args.length !== 1) {
  console.error(
    "Usage: deno run --allow-read --allow-write mutation_conversion.ts <input_file_with_mutations>",
  )
  Deno.exit(1)
}

// Creates the converted name
const userInput = Deno.args[0]
const [first, ...rest] = userInput.split(".")
const convertName = [first + "converted-ts", ...rest].join(".")

// Opens user's file as userJson
const userJson: unknown[] = await Deno.readTextFile(userInput).then(JSON.parse)

// List to hold the dictionaries that are the converted entries
const tempList = userJson
  .map(mutationParser)
  .filter((x) => x.success)
  .map(({ output: { type, id, category } }) => ({
    type,
    id,
    "copy-from": id,
    delete: { category },
  }))

// Output the finished converted JSON
await Deno.writeTextFile(convertName, JSON.stringify(tempList, null, 2))
