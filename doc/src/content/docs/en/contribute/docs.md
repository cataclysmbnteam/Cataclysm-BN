---
title: Updating documentation
---

To update the documentation site, you need to:

- [learn markdown in y minutes](https://learnxinyminutes.com/docs/markdown/)
- [create a github account](https://github.com/join) as the source code for docs site is hosted on
  github.

## Browser

### Update existing docs

<video controls>
  <source src="https://github.com/scarf005/Cataclysm-BN/assets/54838975/b0ba517c-5faf-4f6b-81a1-93b52d8370e7"
  type="video/mp4" />
</video>

this is the most straightforward way to update the documentation site.

![edit page][edit-button]

1. Click the `Edit page` button on the bottom of the page.

[edit-button]: https://github.com/scarf005/Cataclysm-BN/assets/54838975/b31d27d7-f2e5-434b-9541-8bdab85e491e

![github web editor](https://github.com/scarf005/Cataclysm-BN/assets/54838975/eba2738d-5e65-4262-bb3c-354ea75d430a)

2. You will be redirected to the github page of the documentation site. You can edit and preview
   your changes here.

:::note

- `.md` in `CONTRIBUTING.md` stands for markdown files
- `.mdx` in `docs.mdx` stands for [MarkDown eXtended](https://mdxjs.com)
  - it's a superset of markdown with javascript and [jsx component][jsx] support
  - they are a bit more complicated but allows to use interactive components

[jsx]: https://www.typescriptlang.org/docs/handbook/jsx.html

:::

![propose changes window](https://github.com/scarf005/Cataclysm-BN/assets/54838975/d4a06795-1680-4706-a84c-072346bff109)

3. Click the `Commit changes...` button on the top right corner to
   [commit your changes](https://github.com/git-guides/git-commit). Make sure to

- Write a short and descriptive `Commit message`
- Check the `Create a new branch for this commit and start a pull request` checkbox.

![comparing changes page](https://github.com/scarf005/Cataclysm-BN/assets/54838975/3551797e-847b-45fe-8869-8b0b15bfb948)

4. You will be redirected to the `Comparing changes` page. Click the `Create pull request` button to
   [create a pull request](./contributing.md#pull-request-notes).

![open a pull request page](https://github.com/scarf005/Cataclysm-BN/assets/54838975/2a987c19-b165-43c2-a5a2-639f22202926)

5. Fill in the `Open a pull request` page and click the `Create pull request` button to
   [open a PR](./contributing.md#pull-request-notes).

### Creating a new page

<video controls>
  <source src="https://github.com/scarf005/Cataclysm-BN/assets/54838975/29a64b6b-1b1d-4ec2-bbd0-ffd6de277de6"
  type="video/mp4" />
</video>

This one is a bit more involved, but similar to the previous one.

![image](https://github.com/scarf005/Cataclysm-BN/assets/54838975/978568f4-3d76-4d22-bc49-ccc539ea7911)

1. Go to the directory you want to create a new post. Click the `Edit link` button then immediately
   navigate to directory you'd like to edit (in this case, `contributing`).

![Create new file](https://github.com/scarf005/Cataclysm-BN/assets/54838975/209a5dae-a0ec-410c-a523-462b8860aaac)

2. Click the `Add file` > `Create new file` button on the top right corner. Name your file
   `<your-filename>.md` then add the [frontmatter](https://jekyllrb.com/docs/front-matter/) like:

```md
---
title: <your-title>
---
```

currently only `title` is required.

3. After editing, follow the same steps as [Update existing docs](#update-existing-docs) to create a
   pull request.

## Local development

:::note

This section assumes you have some knowledge of [git](https://git-scm.com),
[node](https://nodejs.org/en) and
[javascript](https://developer.mozilla.org/en-US/docs/Web/JavaScript). Of course, you can learn them
as you go.

:::

To run the documentation site locally, you need to:

- install [node](https://nodejs.org/en) for dev server
- (optional) [pnpm](https://pnpm.io) as faster and more space efficient alternative to node's stock
  npm
- (optional) [deno](https://deno.com) to format and generate automated documentation

### Setup dev server

<video controls>
  <source src="https://github.com/scarf005/Cataclysm-BN/assets/54838975/bb8fc5ba-6110-46c0-bea2-2697e81938ff" type="video/mp4" />
</video>

```sh
(Cataclysm-BN) $ cd doc
(Cataclysm-BN/doc) $ pnpm install # 'pnpm i' for short
(Cataclysm-BN/doc) $ pnpm dev
```

Or, if you're not using pnpm:

```sh
(Cataclysm-BN) $ cd doc
(Cataclysm-BN/doc) $ npm install # 'npm i' for short
(Cataclysm-BN/doc) $ npm run dev
```

You will be able to access the documentation site at `http://localhost:4321`.

## Live preview

<video controls>
  <source src="https://github.com/scarf005/Cataclysm-BN/assets/54838975/36a5d7fb-4737-45f9-8248-b6b188b1a48c"
  type="video/mp4" />
</video>

The dev server will automatically reload when you make changes to the documentation.

### Automated docs generation

Lua and CLI docs are generated automatically from the source code. To generate them, go to project
root and run:

```sh
(Cataclysm-BN) $ deno task docs
```

## License

- By contributing to markdown files (including, but not limited to `.md` and `.mdx` files), you
  agree to license your contributions under
  [CC-BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/), the same license as the game.

- By contributing to source code of documentation page (including, but not limited to `.ts` and
  `.astro` files), you agree to license your contributions under
  [AGPL 3.0](https://www.gnu.org/licenses/agpl-3.0.en.html).
