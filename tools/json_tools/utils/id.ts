/** identity function that preserves type.
 * @param x any value.
 * @returns identical value.
 * @example
 * ```ts
 * id(123) // 123
 * id("abc") // "abc"
 * id([1,2,3]) // [1,2,3]
 * id({ foo: "bar" }) // { foo: "bar" }
 * ```
 */
export const id = <const T>(x: T): T => x
