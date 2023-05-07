// https://www.typescriptlang.org/docs/handbook/2/typeof-types.html

// this interactive example will show you
// what and how typeof keyword is useful in typescript.

const a = { foo: 1, bar: "2" }
type A = typeof a // A is of type { foo: number, bar: string }
const b = { foo: 1, bar: "2" } as const

// type B is much more specific than type A because it's using const assertion.
// const assertion means that the value is immutable, and typescript can
// safely assume that the value will never change.
// hence the much more specific type.
type B = typeof b // B is of type { readonly foo: 1, readonly bar: "2" }

// in javascript, you can access properties of an object using
// the dot notation, or the bracket notation.
// for example, here's
b.foo === 1
b["foo"] === 1

// typescript follows the same rules for accessing properties of object and array types.
type B_Foo = B["foo"] // 1
type B_Bar = B["bar"] // "2"
