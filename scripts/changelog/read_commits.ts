import { zip } from "jsr:@std/collections"
import $ from "jsr:@david/dax"

import type { MergeExclusive } from "npm:type-fest"

type DateLike = string | Date

type RangeOption =
  & MergeExclusive<{ after?: DateLike }, { since?: DateLike }>
  & MergeExclusive<{ before?: DateLike }, { until?: DateLike }>

const readLog = (option: RangeOption) => (format: string) => {
  const range = Object.entries(option)
    .map(([k, v]) => `--${k}=${JSON.stringify(typeof v === "string" ? v : v.toISOString())}`)
    .join(" ")

  return $.raw`git log ${range} --oneline --pretty=format:${format}`.lines()
}

export type Commit = {
  subject: string
  author: string
  date: Date
}

export const readCommits = async (option: RangeOption): Promise<Commit[]> => {
  const [subject, author, date] = await Promise
    .all(["%s", "%an", "%ad"].map(readLog(option)))

  return zip(subject, author, date)
    .map(([subject, author, date]) => ({ subject, author, date: new Date(date) }))
}

if (import.meta.main) {
  console.log(await readCommits({ since: "1 week" }))
}
