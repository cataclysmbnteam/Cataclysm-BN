/**
 * Get values of an object. Object needs to be immutable in order to narrow down the type.
 *
 * @example
 * ```ts
 * test("ValueOf", (t) => [
 *   t.equal<ValueOf<{ a: 1; b: "b" }>, 1 | "b">(),
 *   t.equal<ValueOf<{ a: true; b: false }>, boolean>(),
 *   // @ts-expect-error: type should be narrowed down to true
 *   t.equal<ValueOf<{ a: true; b: true }>, boolean>(),
 *   t.equal<ValueOf<{ a: true; b: true }>, true>(),
 *   t.equal<ValueOf<{ a: false; b: false }>, false>(),
 * ])
 * ```
 */
export type ValueOf<T> = T[keyof T]

/**
 * Maps all values of an object to true if they match the given type, false otherwise.
 *
 * @example
 * ```ts
 * const obj = { a: 1, b: "b" }
 * type Result = TypeExistsMap<typeof before, number> // { a: true, b: false }
 */
type TypeExistsMap<Object, Type> = {
  [K in keyof Object]: Object[K] extends Type ? true : false
}

/**
 * Checks that an object has a value of given type.
 *
 * @returns true if object has a value of given type, false otherwise.
 *
 * @example
 * ```ts
 * test("HasType", (t) => [
 *   t.equal<HasType<{ a: 1; b: "b" }, number>, true>(),
 *   t.equal<HasType<{ a: 1; b: "b" }, 1>, true>(),
 *   t.equal<HasType<{ b: "b" }, number>, false>(),
 * ])
 * ```
 */
export type HasType<Object, Type> = ValueOf<TypeExistsMap<Object, Type>> extends false ? false
  : true
