#pragma once
#ifndef CATA_SRC_JSON_EXPORT_H
#define CATA_SRC_JSON_EXPORT_H

#include "json.h"

class vehicle;

namespace json_export
{
auto vehicle( JsonOut &json, const vehicle &v ) -> void;

} // namespace json_export

#endif // CATA_SRC_JSON_EXPORT_H
