import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

import { test } from "npm:ts-spec"

import { MigrationSchema } from "./schema_type.ts"
import { id } from "../utils/id.ts"

const propertyMigration = z.object({ a: z.number().transform(id) })
type PropertyMigration = MigrationSchema<typeof propertyMigration> // same as typeof fooZod

const objectMigration = z.object({ b: z.string() }).transform(id)
type ObjectMigration = MigrationSchema<typeof objectMigration> // same as typeof barZod

const bothMigration = z.object({ a: z.number().transform(id) }).transform(id)
type BothMigration = MigrationSchema<typeof bothMigration> // same as typeof bazZod

const notMigration = z.object({ a: z.number() })
type NotMigration = MigrationSchema<typeof notMigration> // never

test("MigrationSchema", (t) => [
  t.equal<PropertyMigration, typeof propertyMigration>(),
  t.equal<ObjectMigration, typeof objectMigration>(),
  t.equal<BothMigration, typeof bothMigration>(),
  t.equal<NotMigration, never>(),
])
