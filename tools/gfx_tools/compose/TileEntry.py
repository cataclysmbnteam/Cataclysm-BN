from pathlib import Path
from typing import Any

from compose import Tilesheet
from .log import log


def list_or_first(iterable: list) -> Any:
    """
    Strip unneeded container list if there is only one value
    """
    return iterable[0] if len(iterable) == 1 else iterable


class TileEntry:
    """
    Tile entry handling
    """

    def __init__(
        self,
        tilesheet: Tilesheet,
        data: dict,
        filepath: str | Path,
    ) -> None:
        self.tilesheet = tilesheet
        self.data = data
        self.filepath = filepath

    def convert(
        self,
        entry: dict | None = None,
        prefix: str = "",
    ) -> dict | None:
        """
        Recursively compile input into game-compatible objects in-place
        """
        if entry is None:
            entry = self.data

        entry_ids = entry.get("id")
        fg_layer = entry.get("fg")
        bg_layer = entry.get("bg")

        if not entry_ids or (not fg_layer and not bg_layer):
            log.warning(
                "skipping empty entry in %s%s",
                self.filepath,
                f" with IDs {prefix}{entry_ids} " if entry_ids else "",
            )
            return None

        # make sure entry_ids is a list
        if entry_ids:
            if not isinstance(entry_ids, list):
                entry_ids = [entry_ids]

        # convert fg value
        if fg_layer:
            entry["fg"] = list_or_first(self.convert_entry_layer(fg_layer))
        else:
            # don't pop at the start because that affects order of the keys
            entry.pop("fg", None)

        # convert bg value
        if bg_layer:
            entry["bg"] = list_or_first(self.convert_entry_layer(bg_layer))
        else:
            # don't pop at the start because that affects order of the keys
            entry.pop("bg", None)

        # recursively convert additional_tiles value
        additional_entries = entry.get("additional_tiles", [])
        for additional_entry in additional_entries:
            # recursive part
            self.convert(additional_entry, f"{entry_ids[0]}_")

        # remember processed IDs and remove duplicates
        for entry_id in entry_ids:
            full_id = f"{prefix}{entry_id}"

            if full_id not in self.tilesheet.tileset.processed_ids:
                self.tilesheet.tileset.processed_ids.append(full_id)

            else:
                entry_ids.remove(entry_id)

                if self.tilesheet.is_filler:
                    if self.tilesheet.tileset.obsolete_fillers:
                        log.warning(
                            "skipping filler for %s from %s",
                            full_id,
                            self.filepath,
                        )

                else:
                    log.error(
                        "%s encountered more than once, last time in %s",
                        full_id,
                        self.filepath,
                    )

        # return converted entry if there are new IDs
        if entry_ids:
            entry["id"] = list_or_first(entry_ids)
            return entry

        return None

    def convert_entry_layer(
        self,
        entry_layer: list | str,
    ) -> list:
        """
        Convert sprite names to sprite indexes in one fg or bg tile entry part
        """
        output = []

        if isinstance(entry_layer, list):
            # "fg": [ "f_fridge_S", "f_fridge_W", "f_fridge_N", "f_fridge_E" ]
            for layer_part in entry_layer:
                if isinstance(layer_part, dict):
                    # weighted random variations
                    variations, valid = self.convert_random_variations(
                        layer_part.get("sprite")
                    )
                    if valid:
                        layer_part["sprite"] = (
                            variations[0]
                            if len(variations) == 1
                            else variations
                        )
                        output.append(layer_part)
                else:
                    self.append_sprite_index(layer_part, output)
        else:
            # "bg": "t_grass"
            self.append_sprite_index(entry_layer, output)

        return output

    def convert_random_variations(
        self,
        sprite_names: list | str,
    ) -> tuple[list, bool]:
        """
        Convert list of random weighted variation objects
        """
        valid = False
        converted_variations = []

        if isinstance(sprite_names, list):
            # list of rotations
            converted_variations = []
            for sprite_name in sprite_names:
                valid |= self.append_sprite_index(
                    sprite_name, converted_variations
                )
        else:
            # single sprite
            valid = self.append_sprite_index(sprite_names, converted_variations)
        return converted_variations, valid

    def append_sprite_index(
        self,
        sprite_name: str,
        entry: list,
    ) -> bool:
        """
        Get sprite index by sprite name and append it to entry
        """
        if sprite_name:
            sprite_index = self.tilesheet.tileset.pngname_to_pngnum.get(
                sprite_name, 0
            )
            if sprite_index:
                sheet_type = "filler" if self.tilesheet.is_filler else "main"
                try:
                    self.tilesheet.tileset.unreferenced_pngnames[
                        sheet_type
                    ].remove(sprite_name)
                except ValueError:
                    pass

                entry.append(sprite_index)
                return True

            log.error(
                "%(name)s.png file for %(name)s value from %(path)s "
                "was not found. It will not be added to %(target)s",
                {
                    "name": sprite_name,
                    "path": self.filepath,
                    "target": self.tilesheet.tileset.output_conf_file,
                },
            )

        return False
