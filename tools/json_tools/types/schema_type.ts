import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

import { HasType } from "./type_helper.ts"

export type AnyZodEffect = z.ZodEffects<z.ZodTypeAny, unknown, unknown>
export type ObjectSchema = z.ZodObject<z.ZodRawShape>

export type HasZodEffect<T> = T extends ObjectSchema ? HasType<T["shape"], AnyZodEffect> : never

/**
 * Asserts that schema has a transform effect for either the whole object or a property.
 *
 * @example
 * ```ts
 * const propertyMigration = z.object({ a: z.number().transform(id) }) // OK
 * const objectMigration = z.object({ b: z.string() }).transform(id) // OK
 * const bothMigration = z.object({ a: z.number().transform(id) }).transform(id) // OK
 * const notMigration = z.object({ a: z.number() }) // Error
 *
 * type PropertyMigration = MigrationSchema<typeof propertyMigration> // same as typeof fooZod
 * type ObjectMigration = MigrationSchema<typeof objectMigration> // same as typeof barZod
 * type BothMigration = MigrationSchema<typeof bothMigration> // same as typeof bazZod
 * type NotMigration = MigrationSchema<typeof notMigration> // never
 *
 * test("MigrationSchema", (t) => [
 *   t.equal<PropertyMigration, typeof propertyMigration>(),
 *   t.equal<ObjectMigration, typeof objectMigration>(),
 *   t.equal<BothMigration, typeof bothMigration>(),
 *   t.equal<NotMigration, never>(),
 * ])
 * ```
 */
export type MigrationSchema<T> = HasZodEffect<T> extends true ? T
  : T extends z.ZodEffects<ObjectSchema> ? T
  : never
