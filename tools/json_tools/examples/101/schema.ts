// deno-lint-ignore-file no-unused-vars
// JSON schema 101
//
// BN uses [zod](https://zod.dev/) typescript library to parse and transform complex JSON structures. by using zod, this JSON entry

import { z } from "https://deno.land/x/zod@v3.20.5/mod.ts"

// this JSON
const achievementJSON = {
  "id": "It can be any string",
  "type": "achievement",
  "name": "It can be any string",
  "requirements": [
    { "event_statistic": "It can be any string", "is": ">=", "target": 1 },
  ],
}

// can be expressed in zod schema as...
{
  const achievementSchema = {
    id: z.string(), // matches any string
    type: z.literal("achievement"), // matches only "achievement"
    name: z.string(),
    requirements: z.array(
      z.object({
        event_statistic: z.string(),
        is: z.enum([">=", "<=", "=="]),
        target: z.number(),
      }),
    ),
  }
}
