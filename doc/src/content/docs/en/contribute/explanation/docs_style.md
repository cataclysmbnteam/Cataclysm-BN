---
title: Documentation Best Practices
---

![](https://github.com/cataclysmbnteam/Cataclysm-BN/assets/54838975/52de02a8-036e-4940-8881-9d8e1f0f4bc7)

## File structure

For details, check starlight's [authoring content guide][authoring-content].

[authoring-content]: https://starlight.astro.build/guides/authoring-content

### File name

Prefer `snake_case`, like other files in the repo.

### Frontmatter

```md
---
title: OK
---
```

Frontmatter is a block of metadata, in [yaml format][yaml] at the top of a markdown file. At least
`title` attribute is required to render the page correctly. It does not have to match the file name.
A file without frontmatter will cause errors.

[yaml]: https://yaml.org

:::caution

Leading `---` must be at the first line of the file.

<!-- deno-fmt-ignore -->
```md

---
title: Will cause errors because there's a newline before the frontmatter
---
```

:::

## Syntax guide

BN docs uses [markdown](https://en.wikipedia.org/wiki/Markdown) and [MDX](https://mdxjs.com) for
documentation. Here's some syntax guide to help keep pages consistent even when read in raw format.

### Headings

Avoid using h1 (`#`) and h4 (`####`) as it will not appear on table of contents. Use h2 (`##`) and
h3 (`###`) instead.

:::caution

It is possible to
[override displayed heading levels in TOC](https://starlight.astro.build/reference/frontmatter/#tableofcontents),
but it might cause obscure issues, for example using crashes caused by using multiple h1 headings.

:::

### Unordered Lists

- Prefer `-`
- over `*`
- or `+`

```md
- Prefer `-`
- over `*`
- or `+`
```

### Code blocks

```cpp
constexpr auto good = "cpp";
auto avoid = "c++";
```

````md
```
constexpr auto good = "cpp";
auto avoid = "c++";
```

```cpp
constexpr auto good = "cpp";
auto avoid = "c++";
```
````

Prefer using code fences with language identifiers. This allows [shiki][shiki], the syntax
highlighter used by the docs site to understand the language correctly. To see the full list of
supported languages, see [shiki's documentation][shiki-docs].

[shiki]: https://docs.astro.build/en/guides/markdown-content/#syntax-highlighting
[shiki-docs]: https://github.com/shikijs/shiki/blob/main/docs/languages.md#all-languages

### Asides

:::note

Prefer [asides](https://starlight.astro.build/guides/authoring-content/#asides) when displaying
notes.

:::

:::tip{title="They are more expressive"}

You can use following variants: `note`, `caution`, `danger`, and `tip`. You can also give them
custom `title`.

:::

```md
:::note

Prefer [asides](https://starlight.astro.build/guides/authoring-content/#asides) when displaying
notes.

:::

:::tip{title="They are more expressive"}

You can use following variants: `note`, `caution`, `danger`, and `tip`. You can also give them
custom `title`.

:::
```

## Formatting

<video controls>
  <source src="https://github.com/cataclysmbnteam/Cataclysm-BN/assets/54838975/c88319ad-175d-4148-a5e0-38b69569e7d5"
  type="video/mp4" />
</video>

Formatting stuff manually is hard. You can auto-format your markdown files on save by installing
[deno] and its [VSCode extension][deno-ext].

[deno]: https://docs.deno.com/runtime/manual/getting_started/installation
[deno-ext]: https://marketplace.visualstudio.com/items?itemName=denoland.vscode-deno

However, not all can be formatted automatically due to the laxness of markdown syntax. Here's notes
to avoid some weird issues from happening.

---

## [Asides](https://starlight.astro.build/guides/authoring-content/#asides)

### Newlines

there should be a newline between the asides and the content, otherwise the formatter will merge
them into one line, and the asides will not be rendered correctly.

:::caution Avoid this :::

<!-- deno-fmt-ignore -->
```md
this gets formatted to avobe

:::caution
Avoid this
:::
`````

:::note

do this instead

:::

```md
:::note

do this instead

:::
```

### [Custom Titles](https://starlight.astro.build/guides/authoring-content/#custom-aside-titles)

![](https://user-images.githubusercontent.com/54838975/266965654-71bb46d9-10fa-4fb5-a5af-54e1a6a5a9d3.png)

VSCode and many tools raise false positives due to ambiguity between link and aside label syntax. To
avoid this, BN uses [custom fork of starlight][PR] that uses `title` property instead of label
syntax to display custom titles.

[PR]: https://github.com/withastro/starlight/pull/707

:::tip{title="Use titles instead"}

:::

```md
:::caution[Avoid using labels]

:::

:::tip{title="Use titles instead"}

:::
```
