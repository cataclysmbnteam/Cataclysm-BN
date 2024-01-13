import * as flags from "$std/flags/mod.ts"

const IGNORE_MISMATCH = ["id", "abstract"]

const parseArguments = () => {
  const args = flags.parse(Deno.args)
  const itemSource = args["item_source"]
  const outputName = args["output"] || "output.json"

  if (!itemSource) {
    throw new Error("You must specify a json file to rewrite using copy-from.")
  }

  return { itemSource, outputName }
}

const loadJsonFile = async (fileName: string) => {
  try {
    const content = await Deno.readTextFile(fileName)
    return JSON.parse(content)
  } catch (error) {
    throw new Error(`Error loading file ${fileName}: ${error.message}`)
  }
}

const refineItems = (items: any[]) => {
  const baseItem = items[0]
  const baseId = baseItem["id"] || baseItem["abstract"]
  if (!baseId) {
    throw new Error("First item is malformed, missing 'id' or 'abstract'")
  }

  return items.map((item) => {
    if (item === baseItem || item["copy-from"]) return item

    for (const key of Object.keys(item)) {
      if ((key !== "type" && item[key] === baseItem[key]) || IGNORE_MISMATCH.includes(key)) {
        delete item[key]
      }
    }

    item["copy-from"] = baseId
    return item
  })
}

const main = async () => {
  const { itemSource, outputName } = parseArguments()
  const items = await loadJsonFile(itemSource)
  const processedItems = refineItems(items)
  await Deno.writeTextFile(outputName, JSON.stringify(processedItems, null, 2))
  console.log(`Processed data saved to ${outputName}`)
}

if (import.meta.main) {
  await main()
}
