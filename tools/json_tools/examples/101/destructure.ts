// learn more at:
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Destructuring_assignment

import { assertEquals } from "https://deno.land/std@0.186.0/testing/asserts.ts"

// arrays and objects can be destructured using the destructuring assignment syntax
{
  const array = [1, 2, 3, 4, 5]
  const [first, second, ...rest] = array

  assertEquals(first, 1)
  assertEquals(second, 2)
  assertEquals(rest, [3, 4, 5])
}
{
  const object = { foo: 1, bar: 2, baz: 3, qux: 4 }
  const { foo, bar, ...rest } = object

  assertEquals(foo, 1)
  assertEquals(bar, 2)
  assertEquals(rest, { baz: 3, qux: 4 })
}

// destructuring assignment can be used to override properties of an object
{
  const mealSpam = { meal: "spam", foo: 1 } as const
  const mealEgg = { meal: "egg", bar: 2 } as const

  // any duplicate properties will be overriden by the last one
  const mealOverridenToEgg = { ...mealSpam, ...mealEgg } as const

  // therefore, meal is "egg"
  type Meal = typeof mealOverridenToEgg
  // type Meal = {
  //     readonly meal: "egg";
  //     readonly bar: 2;
  //     readonly foo: 1;
  // }
}
