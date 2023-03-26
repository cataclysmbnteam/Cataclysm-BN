# Markdown indented to fenced code blocks converter

![image](https://user-images.githubusercontent.com/54838975/227755928-bd0c2e3a-3026-493d-a4a3-2bd9c09b5e25.png)

indentation based code blocks can be easily broken and does not support language highlight from github. this tool converts indentation based code blocks to fenced code blocks.

## How to use

1. install [deno](https://deno.land/manual/getting_started/installation)
2. execute `tools/markdown-fence/fenced.ts`. 
3. access help by adding `--help` flag at the end

## Benchmarks

it's blazingly fast because it uses full concurrency.

```sh
time tools/markdown-fence/fenced.ts -w doc/

Wrote 50 files

________________________________________________________
Executed in   77.75 millis    fish           external
   usr time   82.41 millis  179.00 micros   82.23 millis
   sys time   20.62 millis   59.00 micros   20.56 millis
```

## Testing

example tests are written in `tools/markdown-fence/fenced_test.ts`
and can be run by `deno test --allow-read`
