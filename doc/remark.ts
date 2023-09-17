import { isAbsolute, join } from "node:path"

import type { Root } from "mdast"
import { visit } from "unist-util-visit"

// starts with ./ or ../
const relativePath = new RegExp(`^\\.\\.?/`)

const relativeUrl = (url: string) =>
  (!isAbsolute(url) && relativePath.test(url)) ? join("..", url) : url

const suffix = new RegExp(`\\.(md|mdx)$`)
const suffixAnchor = new RegExp(`\\.(md|mdx)#`)

const removeSuffix = (url: string) =>
  url
    .replace(suffix, "")
    .replace(suffixAnchor, "#")

export const fixRelativeLinks = () => (tree: Root) =>
  visit(tree, "link", (node) => {
    node.url = relativeUrl(removeSuffix(node.url)).toLowerCase()
  })
