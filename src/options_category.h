#pragma once
#ifndef CATA_SRC_OPTIONS_CATEGORY_H
#define CATA_SRC_OPTIONS_CATEGORY_H

constexpr auto general = "general";
constexpr auto interface = "interface";
constexpr auto graphics = "graphics";
constexpr auto world_default = "world_default";
constexpr auto debug = "debug";
#if defined(__ANDROID__)
constexpr auto android = "android";
#endif

#endif // CATA_SRC_OPTIONS_CATEGORY_H
