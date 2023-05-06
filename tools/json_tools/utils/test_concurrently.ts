export type Step = Deno.TestContext["step"]

/** wrapper to easily run concurrent Deno test step. */
export const concurrently = (step: Step) => ({ name, fn }: Deno.TestStepDefinition) =>
  step({ name, fn, sanitizeOps: false, sanitizeResources: false, sanitizeExit: false })
