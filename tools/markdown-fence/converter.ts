import { match } from "./deps.ts"

import { isCode, isEmpty } from "./checks.ts"
import { StateFns } from "./state_fns.ts"

export type Converter = (input: string[]) => string[]
type ConverterConstructor = (stateFns: StateFns) => Converter

export type State = { inCode: boolean; acc: string[]; line: string }
export type NextState = Omit<State, "line">
type BranchFn = (statefns: StateFns) => (state: State) => NextState
type BranchFns = Record<"textFn" | "incodeFn", BranchFn>

// deno-fmt-ignore
const { textFn, incodeFn }: BranchFns = {
  textFn: ({ appendLine, openFence }) => (s) => match(s.line)
    .when(isCode, openFence(s))
    .otherwise(appendLine(s)),
  incodeFn: ({ appendLine, appendCode, closeFence }) => (cur) => match(cur.line)
    .when(isEmpty, appendLine(cur))
    .when(isCode, appendCode(cur))
    .otherwise(closeFence(cur)),
}

export const converter: ConverterConstructor = (stateFns) => (input) =>
  input.reduce(
    ({ inCode, acc }, line) => (inCode ? incodeFn : textFn)(stateFns)({ inCode, acc, line }),
    { inCode: false, acc: [] as string[] },
  ).acc
