import { test } from "npm:ts-spec"

import { HasType, ValueOf } from "./type_helper.ts"

test("ValueOf", (t) => [
  t.equal<ValueOf<{ a: 1; b: "b" }>, 1 | "b">(),
  t.equal<ValueOf<{ a: true; b: false }>, boolean>(),
  // @ts-expect-error: type should be narrowed down to true
  t.equal<ValueOf<{ a: true; b: true }>, boolean>(),
  t.equal<ValueOf<{ a: true; b: true }>, true>(),
  t.equal<ValueOf<{ a: false; b: false }>, false>(),
])

test("HasType", (t) => [
  t.equal<HasType<{ a: 1; b: "b" }, number>, true>(),
  t.equal<HasType<{ a: 1; b: "b" }, 1>, true>(),
  t.equal<HasType<{ b: "b" }, number>, false>(),
])

