import { join } from "https://deno.land/std@0.182.0/path/mod.ts"
import { asynciter } from "https://deno.land/x/asynciter@0.0.15/asynciter.ts"

const getExecPath = async () => {
  const isMsys2 = Deno.build.os === "windows" && Deno.env.get("MSYSTEM")
  const isMingw32 = Deno.build.os === "windows" && Deno.env.get("CROSS")?.includes("mingw32")
  const JSON_FORMATTER_BIN = `tools/format/json_formatter.${(isMsys2 || isMingw32) ? "exe" : "cgi"}`

  const cwd = (await import("./cata_paths.ts")).ROOT

  return join(cwd, JSON_FORMATTER_BIN)
}

export const fmtJsonRecursively = async (paths: string[]): Promise<void> => {
  const execPath = await getExecPath()

  await asynciter(paths)
    .concurrentUnorderedMap((path) => new Deno.Command(execPath, { args: [path] }).output())
    .collect()
}
