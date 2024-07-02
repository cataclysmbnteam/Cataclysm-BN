/**
 * Fixes compilation database used by run-clang-tidy.py on Windows.
 */
import { join } from "$std/path/join.ts"

const compileDb = "build/compile_commands.json"
const startsWithDriveLetter = /^\/(\w)\/(.*)$/

const modifyCommand = (commandStr: string, directory: string): string =>
  commandStr.split(" ").map((part) => {
    if (part.startsWith("@")) {
      const rspPath = join(directory, part.substring(1))
      return Deno.readTextFileSync(rspPath).trim()
    }

    const matchResult = startsWithDriveLetter.exec(part)
    return matchResult ? `${matchResult[1]}:/${matchResult[2]}` : part
  }).join(" ")

const processCompileDb = (data: any[]): any[] =>
  data.map((entry) => {
    entry["command"] = modifyCommand(entry["command"], entry["directory"])
    return entry
  })

const compileDbData = JSON.parse(Deno.readTextFileSync(compileDb))
const processedData = processCompileDb(compileDbData)
Deno.writeTextFileSync(compileDb, JSON.stringify(processedData, null, 2))
