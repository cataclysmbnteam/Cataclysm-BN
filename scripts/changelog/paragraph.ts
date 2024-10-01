/**
 * template tag function
 * convert newline into space, only if it's not consecutive
 */
export const paragraph = (strings: TemplateStringsArray, ...values: string[]) =>
  strings
    .map((str, i) => str + (values[i] ?? ""))
    .join("")
    .replace(/\n(?!\n)/g, " ")
    .replaceAll("\n", "\n\n")
