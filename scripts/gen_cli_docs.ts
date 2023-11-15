#!/usr/bin/env -S deno run --allow-read --allow-write --allow-run

/**
 * Generates markdown documentation for the game executable.
 */
import { chunk } from "$std/collections/chunk.ts"

export type Flag = { option: string; desc: string }
export type Section = { title: string; flags: Flag[] }

/**
 * Use simple heuristics to parse the command line help text.
 *
 * ### Heuristics
 * - first line is the section title
 * - each flags and description is on a separate line
 */
export const parseSection = (section: string): Section => {
  // first line is the title
  const [title, ...lines] = section.split("\n")
  const flags = chunk(lines, 2).map(([option, desc]) => ({ option, desc: desc.trim() }))

  return { title: title.replace(":", ""), flags }
}

export const flagToMarkdown = ({ option, desc }: Flag): string => /*md*/ `
### \`${option}\`

${desc}.`

export const sectionToMarkdown = ({ title, flags }: Section): string => /*md*/ `
## ${title}
${flags.map(flagToMarkdown).join("\n")}`

const toMarkdown = (text: string): string => {
  const sections = text.trim().split("\n\n").map(parseSection).map(sectionToMarkdown).join("\n")

  return /*md*/ `\
---
title: CLI Options
editUrl: false
sidebar:
  badge:
    text: Generated
    status: note
---

:::note

This page is auto-generated from \`tools/gen_cli_docs.ts\` and should not be edited directly.

:::

The game executable can not only run your favorite roguelike,
but also provides a number of command line options to help modders and developers.

---
${sections}
`
}

if (import.meta.main) {
  const command = new Deno.Command("./cataclysm-tiles", { args: ["--help"] })
  const { stdout } = await command.output()

  const text = new TextDecoder().decode(stdout)

  const result = toMarkdown(text)
  const docsUrl = new URL(
    "../doc/src/content/docs/en/dev/reference/cli_options.md",
    import.meta.url,
  )
  await Deno.writeTextFile(docsUrl, result)
}
