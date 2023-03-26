import { removeIndent } from "./checks.ts"
import { NextState, State } from "./converter.ts"

type StateFn = (state: State) => () => NextState
export type StateFns = Record<
  "appendLine" | "appendCode" | "openFence" | "closeFence",
  StateFn
>

// deno-fmt-ignore
export const stateFns = (lang: string): StateFns => ({
  appendLine: ({ line, acc, inCode }) => () => ({ inCode, acc: [...acc, line] }),
  appendCode: ({ line, acc, inCode }) => () => ({ inCode, acc: [...acc, removeIndent(line)] }),
  openFence: ({ acc, line }) => () => ({ inCode: true, acc: [...acc, "```" + lang, removeIndent(line)] }),
  closeFence: ({ line, acc }) => () => ({ inCode: false, acc: [...acc.slice(0, -1), "```", "", line] }),
})
