#pragma once
#ifndef CATA_SRC_COLONY_H
#define CATA_SRC_COLONY_H

#include <list>
#include <vector>
class item;

namespace cata
{
//TODO!: Lmao
template<typename T>
using colony = std::vector<item *>;

}

using ItemList = std::vector<item *>;

#endif // CATA_SRC_COLONY_H
