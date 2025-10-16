#include "data_vars.h"

#include "creature_tracker.h"
#include "json.h"

void data_vars::set(const key_type &name, const mapped_type &value) {
  data[name] = value;
}

data_vars::mapped_type data_vars::get(const key_type &name,
                                      const mapped_type &default_value) const {
  const auto it = find(name);
  if (it == end()) {
    return default_value;
  }
  return it->second;
}

bool data_vars::try_get(const key_type &name, mapped_type &value) const {
  const auto it = find(name);
  if (it == end()) {
    return false;
  }
  value = it->second;
  return true;
}