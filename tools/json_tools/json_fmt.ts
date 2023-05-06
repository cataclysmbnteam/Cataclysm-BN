export const fmtJsonRecursively = async () => {
  const cwd = (await import("./cata_paths.ts")).ROOT
  const command = new Deno.Command("make", { cwd, args: ["style-all-json-parallel"] })

  return command.output()
}
