import lume from "lume/mod.ts"
import date from "lume/plugins/date.ts"
import wiki from "wiki/mod.ts"
import { slugify } from "lume/cms/core/utils/string.ts"

import { SEPARATOR } from "lume/deps/path.ts"

import "npm:prismjs/components/prism-c.js"
import "npm:prismjs/components/prism-cpp.js"
import "npm:prismjs/components/prism-diff.js"
import "npm:prismjs/components/prism-bash.js"

import { ko } from "npm:date-fns/locale/ko"

const site = lume()

const languages = ["en", "ko", "ru", "de"]

/**
 * Assigns default `id` to files based on their path excluding the language code.
 * e.g `/ko/foo/bar.md` will have `id` of `foo/bar`
 */
const autoId = (options: { languages: string[] }) => {
  const LANGUAGE_CODE = new RegExp(
    `^\\${SEPARATOR}(${options.languages.join("|")})(?=\\${SEPARATOR})`,
  )
  return (site: Lume.Site) => {
    site.preprocess([".md"], (files) =>
      files.forEach((file) => {
        if (!file.data.id) file.data.id = slugify(file.src.path).replace(LANGUAGE_CODE, "")
      }))
  }
}

site
  .ignore("README.md")
  .use(autoId({ languages }))
  .use(wiki({ languages }))
  .use(date({
    locales: { ko },
    formats: { HUMAN_DATE: "yyyy-MM-dd", HUMAN_DATETIME: "yyyy-MM-dd ppp" },
  }))

export default site
