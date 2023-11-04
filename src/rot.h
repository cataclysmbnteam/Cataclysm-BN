#pragma once
#ifndef CATA_SRC_ROT_H
#define CATA_SRC_ROT_H

enum class temperature_flag : int;

class map;
class item;

namespace rot
{

// TODO: Move to item_location method?
auto temperature_flag_for_location( const map &m, const item &loc ) -> temperature_flag;

} // namespace rot

#endif // CATA_SRC_ROT_H
