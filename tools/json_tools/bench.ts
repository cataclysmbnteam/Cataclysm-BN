import {
  bgBrightRed,
  bold,
  brightGreen,
  brightRed,
  brightWhite,
  brightYellow,
} from "https://deno.land/std@0.182.0/fmt/colors.ts"
import { c, p } from "https://deno.land/x/copb@v1.0.1/mod.ts"
import { match, P } from "npm:ts-pattern"

/** wait for a promise and log how long it took. */
export const bench = (name: string) => async <T>(val: Promise<T>) => {
  const start = performance.now()
  const result = await val
  const end = performance.now()
  const ms = Math.round(end - start)
  const color = match({ ms })
    .with({ ms: P.when((x) => x < 100) }, () => brightGreen)
    .with({ ms: P.when((x) => x < 500) }, () => brightYellow)
    .with({ ms: P.when((x) => x < 1000) }, () => brightRed)
    .otherwise(() => c(p(bgBrightRed)(brightWhite)(bold)))

  console.log(color(`${name} took ${ms}ms`))
  return { result, ms }
}
