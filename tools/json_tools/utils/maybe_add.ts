import { match } from "npm:ts-pattern"

const returnEmpty = () => ({})
const returnField = (field: string) => <T>(x: T) => ({ [field]: x })

// add field to object if given array or object is not empty.
export const maybeAdd = (field: string) => (x: Record<string, unknown> | unknown[]) =>
  match(x)
    .when((x) => Object.keys(x).length === 0, returnEmpty)
    .otherwise(returnField(field))
