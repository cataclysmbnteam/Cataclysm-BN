import dataclasses
import json
import os
from dataclasses import dataclass
from pathlib import Path

from .Fallback import FALLBACK
from .log import log
from tqdm import tqdm

PROPERTIES_FILENAME = "tileset.txt"


def read_properties(filepath: str) -> dict:
    """
    tileset.txt reader
    """
    with open(filepath, "r", encoding="utf-8") as file:
        pairs = {}
        for line in file.readlines():
            line = line.strip()
            if line and not line.startswith("#"):
                key, value = line.split(":")
                pairs[key.strip()] = value.strip()
    return pairs


@dataclass
class Tileset:
    """
    Referenced sprites memory and handling, tile entries conversion
    """

    source_dir: Path
    output_dir: Path
    use_all: bool = False
    obsolete_fillers: bool = False
    palette_copies: bool = False
    palette: bool = False
    format_json: bool = False
    only_json: bool = False

    def __post_init__(self):
        if not self.source_dir.is_dir() or not os.access(
            self.source_dir, os.R_OK
        ):
            raise ComposingException(
                f"Error: cannot open directory {self.source_dir}"
            )

        self.output_conf_file = None
        self.pngnum = 0
        self.unreferenced_pngnames = {
            "main": [],
            "filler": [],
        }
        self.pngname_to_pngnum = {"null_image": 0}

        self.processed_ids = []
        info_path = self.source_dir.joinpath("tile_info.json")
        self.sprite_width = 16
        self.sprite_height = 16
        self.pixelscale = 1
        self.iso = False
        self.retract_dist_min = -1.0
        self.retract_dist_max = 1.0

        self.info = json.loads(info_path.read_text())

        if not os.access(info_path, os.R_OK):
            raise ComposingException(f"Error: cannot open {info_path}")

        self.sprite_width = self.info[0].get("width", self.sprite_width)
        self.sprite_height = self.info[0].get("height", self.sprite_height)
        self.pixelscale = self.info[0].get("pixelscale", self.pixelscale)
        self.retract_dist_min = self.info[0].get(
            "retract_dist_min", self.retract_dist_min
        )
        self.retract_dist_max = self.info[0].get(
            "retract_dist_max", self.retract_dist_max
        )
        self.iso = self.info[0].get("iso", self.iso)

    def determine_conffile(self) -> str:
        """
        Read JSON value from tileset.txt
        """
        properties = {}

        for candidate_path in (self.source_dir, self.output_dir):
            properties_path = candidate_path.joinpath(PROPERTIES_FILENAME)
            if os.access(properties_path, os.R_OK):
                properties = read_properties(properties_path)
                if properties:
                    break

        if not properties:
            raise ComposingException(f"No valid {PROPERTIES_FILENAME} found")

        conf_filename = properties.get("JSON", None)

        if not conf_filename:
            raise ComposingException(
                f"No JSON key found in {PROPERTIES_FILENAME}"
            )

        self.output_conf_file = conf_filename
        return self.output_conf_file

    def compose(
        self, *, ignore_file: str, run_silent=False, no_tqdm=False
    ) -> None:
        """
        Convert a composing tileset into a package readable by the game
        """
        from .Tilesheet import Tilesheet

        self.output_dir.mkdir(parents=True, exist_ok=True)
        tileset_confpath = self.output_dir.joinpath(self.determine_conffile())
        typed_sheets = {
            "main": [],
            "filler": [],
            "fallback": [],
        }
        fallback_name = "fallback.png"

        # loop through tilesheets and parse all configs in subdirectories,
        # create sheet images
        added_first_null = False
        for config in self.info[1:]:
            sheet = Tilesheet(self, config)

            if not added_first_null:
                sheet.sprites.append(sheet.null_image)
                added_first_null = True

            if sheet.is_filler:
                sheet_type = "filler"
            elif sheet.is_fallback:
                sheet_type = "fallback"
            else:
                sheet_type = "main"

            log.info("parsing %s tilesheet %s", sheet_type, sheet.name)
            if not run_silent:
                print(
                    f"Composing [{sheet_type}] tilesheet [{sheet.name}]...",
                    end=" " if no_tqdm else "\n",
                    flush=True,
                )
            if sheet_type != "fallback":
                sheet.walk_dirs(ignore_file=ignore_file, no_tqdm=no_tqdm)
                # TODO: generate JSON first
                # then create sheets if there are no errors
                if no_tqdm and not run_silent:
                    print("done.", flush=True)
                if not run_silent:
                    print("Saving output PNG...", end=" ", flush=True)
                # write output PNGs
                if not sheet.write_composite_png():
                    continue
                if not run_silent:
                    print("done.", flush=True)
                sheet.max_index = self.pngnum

            typed_sheets[sheet_type].append(sheet)

        # combine config data in the correct order
        sheet_configs = (
            typed_sheets["main"]
            + typed_sheets["filler"]
            + typed_sheets["fallback"]
        )

        # prepare "tiles-new", but remember max index of each sheet in keys
        tiles_new_dict = dict()

        def create_tile_entries_for_unused(
            unused: list,
            fillers: bool,
        ) -> None:
            # the list must be empty without use_all
            mode = unused if no_tqdm or run_silent else tqdm(unused)
            for unused_png in mode:
                if unused_png in self.processed_ids:
                    if not fillers:
                        log.warning(
                            "%(1)s sprite was not mentioned in any tile "
                            "entry but there is a tile entry for the %(1)s ID",
                            {"1": unused_png},
                        )
                    if fillers and self.obsolete_fillers:
                        log.warning(
                            "there is a tile entry for %s "
                            "in a non-filler sheet",
                            unused_png,
                        )
                    continue
                unused_num = self.pngname_to_pngnum[unused_png]
                sheet_min_index = 0
                for sheet_max_index, sheet_data in tiles_new_dict.items():
                    if sheet_min_index < unused_num <= sheet_max_index:
                        sheet_data["tiles"].append(
                            {
                                "id": unused_png,
                                "fg": unused_num,
                            }
                        )
                        self.processed_ids.append(unused_png)
                        break
                    sheet_min_index = sheet_max_index

        main_finished = False

        for sheet in sheet_configs:
            if sheet.is_fallback:
                fallback_name = sheet.name
                if not sheet.is_standard():
                    FALLBACK.sprite_width = sheet.sprite_width
                    FALLBACK.sprite_height = sheet.sprite_height
                    FALLBACK.sprite_offset_x = sheet.offset_x
                    FALLBACK.sprite_offset_y = sheet.offset_y
                    if (
                        sheet.offset_x_retracted != sheet.offset_x
                        or sheet.offset_y_retracted != sheet.offset_y
                    ):
                        FALLBACK.sprite_offset_x_retracted = (
                            sheet.offset_x_retracted
                        )
                        FALLBACK.sprite_offset_y_retracted = (
                            sheet.offset_y_retracted
                        )
                    if sheet.pixelscale != 1.0:
                        FALLBACK.pixelscale = sheet.pixelscale
                continue
            if sheet.is_filler and not main_finished:
                create_tile_entries_for_unused(
                    self.handle_unreferenced_sprites("main"), fillers=False
                )
                main_finished = True
            sheet_entries = []

            for tile_entry in sheet.tile_entries:
                # TODO: pop?
                converted_tile_entry = tile_entry.convert()
                if converted_tile_entry:
                    sheet_entries.append(converted_tile_entry)

            sheet_conf = {
                "file": sheet.name,
                "//": f"range {sheet.first_index} to {sheet.max_index}",
            }

            if not sheet.is_standard():
                sheet_conf["sprite_width"] = sheet.sprite_width
                sheet_conf["sprite_height"] = sheet.sprite_height
                sheet_conf["sprite_offset_x"] = sheet.offset_x
                sheet_conf["sprite_offset_y"] = sheet.offset_y
                if (
                    sheet.offset_x_retracted != sheet.offset_x
                    or sheet.offset_y_retracted != sheet.offset_y
                ):
                    sheet_conf[
                        "sprite_offset_x_retracted"
                    ] = sheet.offset_x_retracted
                    sheet_conf[
                        "sprite_offset_y_retracted"
                    ] = sheet.offset_y_retracted
                if sheet.pixelscale != 1.0:
                    sheet_conf["pixelscale"] = sheet.pixelscale

            sheet_conf["tiles"] = sheet_entries

            tiles_new_dict[sheet.max_index] = sheet_conf

        if not main_finished:
            create_tile_entries_for_unused(
                self.handle_unreferenced_sprites("main"),
                fillers=False,
            )

        create_tile_entries_for_unused(
            self.handle_unreferenced_sprites("filler"),
            fillers=True,
        )

        # finalize "tiles-new" config
        tiles_new = list(tiles_new_dict.values())

        FALLBACK.file = fallback_name
        tiles_new.append(dataclasses.asdict(FALLBACK))
        output_conf = {
            "tile_info": [
                {
                    "pixelscale": self.pixelscale,
                    "width": self.sprite_width,
                    "height": self.sprite_height,
                    "iso": self.iso,
                    "retract_dist_min": self.retract_dist_min,
                    "retract_dist_max": self.retract_dist_max,
                }
            ],
            "tiles-new": tiles_new,
        }

        # save the config
        write_to_json(tileset_confpath, output_conf, self.format_json)
        if no_tqdm and not run_silent:
            print("done.", flush=True)

    def handle_unreferenced_sprites(
        self,
        sheet_type: str,
    ) -> list:
        """
        Either warn about unused sprites or return the list
        """
        if self.use_all:
            return self.unreferenced_pngnames[sheet_type]

        for pngname in self.unreferenced_pngnames[sheet_type]:
            if pngname in self.processed_ids:
                log.error(
                    "%(1)s.png not used when %(1)s ID "
                    "is mentioned in a tile entry",
                    {"1": pngname},
                )

            else:
                log.warning(
                    "sprite filename %s was not used in any %s %s entries",
                    pngname,
                    sheet_type,
                    self.output_conf_file,
                )
        return []
