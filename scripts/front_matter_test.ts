import { assertEquals } from "$std/assert/assert_equals.ts"
import { consumeFirstHeading, toFrontmatter } from "./front_matter.ts"

const input = /*md*/ `

## Foo asdf

bar

### baz

qux
`
const output = /*md*/ `\
---
title: Foo asdf
---

bar

### baz

qux

`
Deno.test("first heading is found and consumed", () => {
  const result = consumeFirstHeading(input.split("\n"))
  assertEquals(result, {
    heading: "Foo asdf",
    lines: ["", "bar", "", "### baz", "", "qux", ""],
  })
})
Deno.test("first heading is converted to frontmatter", () => {
  const result = toFrontmatter(input)
  assertEquals(result, output)
})
Deno.test("function is idempotent", () => {
  const result = toFrontmatter(output)
  assertEquals(result, output)
})
