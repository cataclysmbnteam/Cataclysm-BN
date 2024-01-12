/**
 * Fixes compilation database used by run-clang-tidy.py on Windows.
 */

import { readTextFileSync, writeTextFileSync } from "https://deno.land/std/fs/mod.ts";
import { join } from "https://deno.land/std/path/mod.ts";

const compileDb = "build/compile_commands.json";
const startsWithDriveLetter = /^\/(\w)\/(.*)$/;

function modifyCommand(commandStr: string, directory: string): string {
  return commandStr.split(" ").map(part => {
    if (part.startsWith("@")) {
      const rspPath = join(directory, part.substring(1));
      return readTextFileSync(rspPath).trim();
    } 

    const matchResult = startsWithDriveLetter.exec(part);
    return matchResult ? `${matchResult[1]}:/${matchResult[2]}` : part;
  }).join(" ");
}

function processCompileDb(data: any[]): any[] {
  return data.map(entry => {
    entry["command"] = modifyCommand(entry["command"], entry["directory"]);
    return entry;
  });
}

const compileDbData = JSON.parse(readTextFileSync(compileDb));
const processedData = processCompileDb(compileDbData);
writeTextFileSync(compileDb, JSON.stringify(processedData, null, 2));
