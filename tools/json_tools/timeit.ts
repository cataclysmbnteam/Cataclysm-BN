import {
  bgBrightRed,
  bold,
  brightGreen,
  brightRed,
  brightWhite,
  brightYellow,
} from "https://deno.land/std@0.182.0/fmt/colors.ts"
import { c, p } from "https://deno.land/x/copb@v1.0.1/mod.ts"
import { match } from "npm:ts-pattern"

const color = (ms: number) =>
  match(ms)
    .when((x) => x < 100, () => brightGreen)
    .when((x) => x < 500, () => brightYellow)
    .when((x) => x < 1000, () => brightRed)
    .otherwise(() => c(p(bgBrightRed)(brightWhite)(bold)))

/** wait for a promise and returns its value, time it took, and color. */
export const timeitVerbose = async <T>(val: Promise<T>) => {
  const start = performance.now()
  const result = await val
  const end = performance.now()
  const ms = Math.round(end - start)
  const colorFn = color(ms)

  return { result, ms, color: colorFn, msColored: colorFn(`(${ms}ms)`) }
}

/** wait for a promise and log how long it took. */
export const timeit = (name: string) => async <T>(val: Promise<T>) => {
  const { result, msColored } = await timeitVerbose(val)
  console.log(`${name} ${msColored}`)
  return result
}
