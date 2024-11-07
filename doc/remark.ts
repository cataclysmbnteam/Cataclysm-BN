import { join } from "node:path"

import type { Root } from "mdast"
import { visit } from "unist-util-visit"

/** Check if a string starts with one of `http://` or `https://`. */
export const isAbsolute = (link: string) => /^https?:\/\//.test(link)

// starts with ./ or ../
const relativePath = new RegExp(`^\\.\\.?/`)

const isRelative = (url: string) => !isAbsolute(url) && relativePath.test(url)

const relativeUrl = (url: string) => join("..", url)

const suffix = new RegExp(`\\.(md|mdx)$`)
const suffixAnchor = new RegExp(`\\.(md|mdx)#`)

const removeSuffix = (url: string) =>
  url
    .replace(suffix, "")
    .replace(suffixAnchor, "#")

export const fixRelativeLinks = () => (tree: Root) =>
  visit(tree, "link", (node) => {
    if (!isRelative(node.url)) return

    node.url = relativeUrl(removeSuffix(node.url)).toLowerCase()
  })
