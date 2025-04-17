#pragma once

#include <set>
#include <vector>

class Character;
class inventory;
class item;

bool try_salvage( Character &who, item &it, bool mute = true );

void cut_up( Character &who, item &cut );

int time_to_cut_up( const item &it );

bool has_salvage_tools( const inventory &inv, item &item, bool check_charges = false );




