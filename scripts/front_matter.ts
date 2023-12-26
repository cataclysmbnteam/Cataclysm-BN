/**
 * Converts first heading into frontmatter.
 */

import * as frontmatter from "$std/front_matter/yaml.ts"
import { walk } from "$std/fs/walk.ts"
import { asynciter } from "$asynciter/mod.ts"

export const consumeFirstHeading = (lines: string[]): { heading: string; lines: string[] } => {
  // consume all empty lines with regex
  const spaces = lines.findIndex((line) => !line.match(/^\s*$/)) ?? 0
  // split into frontmatter and content

  const [frontmatter, ...content] = lines.slice(spaces)
  return { heading: frontmatter.replace(/^#+\s*/, ""), lines: content }
}

export const toFrontmatter = (text: string): string => {
  if (frontmatter.test(text)) {
    return text
  }

  const lines = text.split("\n")

  const { heading, lines: content } = consumeFirstHeading(lines)
  return /*md*/ `\
---
title: ${heading}
---
${content.join("\n")}
`
}

if (import.meta.main) {
  await asynciter(walk("doc/src/content", { exts: ["md"], includeDirs: false }))
    .concurrentUnorderedMap(async ({ path, name }) => ({
      path,
      name,
      text: await Deno.readTextFile(path),
    }))
    .filter(({ text }) => !frontmatter.test(text))
    .concurrentUnorderedMap(async ({ path, name, text }) => {
      console.log(name)
      await Deno.writeTextFile(path, toFrontmatter(text))
    })
    .collect()
}
