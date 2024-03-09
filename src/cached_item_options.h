#pragma once
#ifndef CATA_SRC_CACHED_ITEM_OPTIONS_H
#define CATA_SRC_CACHED_ITEM_OPTIONS_H

enum class merge_comestible_t {
    merge_legacy,
    merge_liquid,
    merge_all,
};

/**
 * Merge similar comestibles.  Legacy: default behavior.  Liquid: Merge only liquid comestibles.  All: Merge all comestibles.
 */
extern merge_comestible_t merge_comestible_mode;

/**
 * Only merge comestibles fresher than given threshold. Lower value means stricter merging. 0.0 is fresh, 1.0 is rotten.
 */
extern float relative_rot_threshold;

#endif // CATA_SRC_CACHED_ITEM_OPTIONS_H
