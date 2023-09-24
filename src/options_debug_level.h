#pragma once
#ifndef CATA_SRC_OPTIONS_DEBUG_LEVEL_H
#define CATA_SRC_OPTIONS_DEBUG_LEVEL_H

#include "debug.h"

struct debug_log_level {
    DL id;
    std::string opt_id;
    std::string opt_name;
    std::string opt_descr;
    bool opt_default;
};

struct debug_log_class {
    DC id;
    std::string opt_id;
    std::string opt_name;
    std::string opt_descr;
    bool opt_default;
};


#endif // CATA_SRC_OPTIONS_DEBUG_LEVEL_H
