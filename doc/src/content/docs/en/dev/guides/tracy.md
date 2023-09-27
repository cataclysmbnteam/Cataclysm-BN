---
title: Profiling with tracy
---

![](https://camo.githubusercontent.com/addc8ec15d303dd7084724123e18c3c47fbe721e000c6e2c58a2ac1185badf1f/68747470733a2f2f6d656469612e646973636f72646170702e6e65742f6174746163686d656e74732f3833303931363435313531373835373839342f313135323137353437343038353139393837322f696d6167652e706e673f77696474683d31303939266865696768743d363235)

Tracy is a tool for profiling and debugging your code. It is separated into two parts: server and
client. Server is a part of your application that collects data and client is a GUI that displays
it.

## Install tracy client

### Linux, MacOS

```sh
make -j 10 -C profiler/build/unix LEGACY=1 release
```

Clone <https://github.com/wolfpld/tracy> and build from source. For more instructions, check
[tracy's github actions](https://github.com/wolfpld/tracy/blob/master/.github/workflows/gcc.yml).

:::note{title="for X11"}

tracy uses wayland by default, if you want to use X11, you need to add `LEGACY=1` flag.

:::

### Windows

![image](https://github.com/cataclysmbnteam/Cataclysm-BN/assets/54838975/b6f73c09-969c-4305-b8fb-070d14fb834a)

Download pre-compiled executable from <https://github.com/wolfpld/tracy/releases>.

## Build BN with tracy server

Build on cmake with `-D TRACY=ON` flag. See
[CMake options](building/cmake.md#cataclysmbn-specific-options) for more information.
