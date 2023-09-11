import { join } from "node:path"
import { fileURLToPath } from "node:url"

import { defineConfig } from "astro/config"
import starlight from "@astrojs/starlight"
import starlightLinksValidator from "starlight-links-validator"
import { loadEnv } from "vite"

import { alternativeTipSyntax, fixRelativeLinks } from "./remark.js"

const envPath = join(fileURLToPath(import.meta.url), "..")
const env = loadEnv("development", envPath, "CUSTOM")

const { CUSTOM_SITE_URL, CUSTOM_REPO_URL } = env

const site = CUSTOM_SITE_URL ?? "https://docs.cataclysmbn.org"
const github = CUSTOM_REPO_URL ?? "https://github.com/cataclysmbnteam/Cataclysm-BN"
const itemGuide = "https://cbn-guide.mythoscraft.org"
const discord = "https://discord.gg/XW7XhXuZ89"

const docModes = (dir: string) => [
  { label: "Tutorial", autogenerate: { directory: `${dir}/tutorial` } },
  { label: "Guides", autogenerate: { directory: `${dir}/guides` } },
  { label: "Reference", autogenerate: { directory: `${dir}/reference` } },
  { label: "Explanation", autogenerate: { directory: `${dir}/explanation` } },
]

export default defineConfig({
  site,
  redirects: { "/": `/en/` },
  markdown: {
    remarkPlugins: [fixRelativeLinks, alternativeTipSyntax],
  },
  integrations: [
    starlightLinksValidator(),
    starlight({
      title: "Cataclysm: Bright Nights",
      defaultLocale: "en",
      locales: {
        en: { label: "English" },
        ko: { label: "한국어", lang: "ko-KR" },
      },
      logo: { src: "./src/assets/icon-round.svg" },
      social: { github, discord },
      /* https://starlight.astro.build/guides/css-and-tailwind/#color-theme-editor */
      customCss: ["./src/styles/theme.css"],
      editLink: { baseUrl: `${github}/edit/upload/doc` },
      lastUpdated: true,
      navbar: {
        json: {
          label: "JSON",
          link: "/json/explanation/json_style",
          translations: { "ko-KR": "JSON 모딩" },
          items: docModes("json"),
        },
        lua: {
          label: "Lua",
          link: "/lua/guides/modding",
          translations: { "ko-KR": "Lua 모딩" },
          items: docModes("lua"),
        },
        dev: {
          label: "Engine",
          link: "/dev/guides/building/building",
          translations: { "ko-KR": "게임 엔진" },
          items: docModes("dev"),
        },
        "i18n": {
          label: "I18n",
          link: "/i18n/tutorial/transifex",
          translations: { "ko-KR": "번역" },
          items: docModes("i18n"),
        },
        contributing: {
          label: "Contributing",
          link: "/contributing/contributing",
          translations: { "ko-KR": "기여하기" },
          items: [
            { label: "Contributing", autogenerate: { directory: "contributing" } },
            {
              label: "Style Guide",
              items: [
                { label: "Code Style", link: "/dev/explanation/code_style" },
                { label: "JSON Style", link: "/json/explanation/json_style" },
              ],
            },
          ],
        },
      },
    }),
  ],
})
