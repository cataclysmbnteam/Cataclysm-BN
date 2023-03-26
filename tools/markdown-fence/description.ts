import { brightGreen, brightYellow, outdent } from "./deps.ts"

const fence = "```"

const before = outdent`${brightYellow("before)")}

  Some text

      ${brightYellow("Indented code")}

  End of text
`

const after = outdent`${brightGreen("after)")}

  Some text
  ${
  brightGreen(`
  ${fence}sh
  Indented code
  ${fence}
  `)
}
  End of text
`

export const description = outdent`
    Converts markdown code blocks from indented to fenced.

    ${before}

    ${after}
`
