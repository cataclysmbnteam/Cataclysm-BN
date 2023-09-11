import { isAbsolute, join } from "node:path"

import type { Root } from "mdast"
import { visit } from "unist-util-visit"

// starts with ./ or ../
const relativePath = new RegExp(`^\\.\\.?/`)

const relativeUrl = (url: string) =>
  (!isAbsolute(url) && relativePath.test(url)) ? join("..", url) : url

const suffix = new RegExp(`\\.(md|mdx)$`)

const removeSuffix = (url: string) => url.replace(suffix, "")

export const fixRelativeLinks = () => (tree: Root) =>
  visit(tree, "link", (node) => {
    node.url = relativeUrl(removeSuffix(node.url)).toLowerCase()
  })

/**
 * Uses :::tip "Text" instead of :::tip[Text] to avoid link parsing warnings.
 */
export const alternativeTipSyntax = () => (tree: Root) =>
  visit(tree, "text", (node) => {
    if (!node.value.includes(":::tip")) return
    const prev = node.value
    console.log("match")
    node.value = node.value.replace(/:::tip\s*"(.+?)"/g, ":::tip [$1]")
    console.log(`${prev} -> ${node.value}`)
  })
