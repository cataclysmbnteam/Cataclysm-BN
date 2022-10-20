import json
import os
from pathlib import Path


from .Tileset import Tileset
from tqdm import tqdm
from .log import log

try:
    vips_path = os.getenv("LIBVIPS_PATH")
    if vips_path is not None and vips_path != "":
        os.environ["PATH"] += f";{os.path.join(vips_path, 'bin')}"
    import pyvips  # type: ignore

    Vips = pyvips
except ImportError:
    import gi

    gi.require_version("Vips", "8.0")  # NoQA
    from gi.repository import Vips

PNGSAVE_ARGS = {
    "compression": 9,
    "strip": True,
    "filter": 8,
}


class Tilesheet:
    """
    Tilesheet reading and compositing
    """

    def __init__(
        self,
        tileset: Tileset,
        config: dict,
    ) -> None:
        self.tileset = tileset

        self.name = next(iter(config))
        specs = config.get(self.name, {})

        self.sprite_width = specs.get("sprite_width", tileset.sprite_width)
        self.sprite_height = specs.get("sprite_height", tileset.sprite_height)
        self.offset_x = specs.get("sprite_offset_x", 0)
        self.offset_y = specs.get("sprite_offset_y", 0)
        self.offset_x_retracted = specs.get(
            "sprite_offset_x_retracted", self.offset_x
        )
        self.offset_y_retracted = specs.get(
            "sprite_offset_y_retracted", self.offset_y
        )

        self.pixelscale = specs.get("pixelscale", 1.0)

        self.sprites_across = specs.get("sprites_across", 16)
        self.exclude = specs.get("exclude", tuple())

        self.is_fallback = specs.get("fallback", False)
        self.is_filler = not self.is_fallback and specs.get("filler", False)

        output_root = self.name.split(".png")[0]
        dir_name = (
            f"pngs_{output_root}_{self.sprite_width}x{self.sprite_height}"
        )
        self.subdir_path = tileset.source_dir.joinpath(dir_name)

        self.output = tileset.output_dir.joinpath(self.name)

        self.tile_entries = []
        self.null_image = Vips.Image.grey(self.sprite_width, self.sprite_height)
        self.sprites = []

        self.first_index = self.tileset.pngnum + 1
        self.max_index = self.tileset.pngnum

    def is_standard(self) -> bool:
        """
        Check whether output object needs a non-standard size or offset config
        """
        if self.offset_x or self.offset_y:
            return False
        if (
            self.offset_x_retracted != self.offset_x
            or self.offset_y_retracted != self.offset_y
        ):
            return False
        if self.sprite_width != self.tileset.sprite_width:
            return False
        if self.sprite_height != self.tileset.sprite_height:
            return False
        if self.pixelscale != 1.0:
            return False
        return True

    def walk_dirs(
        self, *, ignore_file: str, no_tqdm=False, run_silent=False
    ) -> None:
        """
        Find and process all JSON and PNG files within sheet directory
        """

        def filtered_tree(excluded):
            for root, dirs, filenames in os.walk(
                self.subdir_path, followlinks=True
            ):
                # replace dirs in-place to prevent walking down excluded paths
                dirs[:] = [
                    d
                    for d in dirs
                    if Path(root).joinpath(d) not in excluded
                    and not Path(root).joinpath(d, ignore_file).is_file()
                ]
                yield [root, dirs, filenames]

        sorted_files = sorted(
            filtered_tree(list(map(self.subdir_path.joinpath, self.exclude))),
            key=lambda d: d[0],
        )
        mode = sorted_files if no_tqdm or run_silent else tqdm(sorted_files)
        for subdir_fpath, dirs, filenames in mode:
            subdir_fpath = Path(subdir_fpath)
            for filename in sorted(filenames):
                filepath = subdir_fpath.joinpath(filename)

                if filepath.suffixes == [".png"]:
                    self.process_png(filepath)

                elif filepath.suffixes == [".json"]:
                    self.process_json(filepath)

    def process_png(
        self,
        filepath: Path,
    ) -> None:
        """
        Verify image root name is unique, load it and register
        """
        if filepath.stem in self.tileset.pngname_to_pngnum:
            if not self.is_filler:
                log.error("duplicate root name %s: %s", filepath.stem, filepath)

            if self.is_filler and self.tileset.obsolete_fillers:
                log.warning(
                    "root name %s is already present in a non-filler sheet: "
                    "%s",
                    filepath.stem,
                    filepath,
                )

            return

        if not self.tileset.only_json:
            self.sprites.append(self.load_image(filepath))
        else:
            self.sprites.append(None)

        self.tileset.pngnum += 1
        self.tileset.pngname_to_pngnum[filepath.stem] = self.tileset.pngnum
        self.tileset.unreferenced_pngnames[
            "filler" if self.is_filler else "main"
        ].append(filepath.stem)

    def load_image(
        self,
        png_path: Path,
    ) -> pyvips.Image:
        """
        Load and verify an image using pyvips
        """
        try:
            image = Vips.Image.pngload(str(png_path))
        except pyvips.error.Error as pyvips_error:
            raise ComposingException(
                f"Cannot load {png_path}: {pyvips_error.message}"
            ) from None
        except UnicodeDecodeError:
            raise ComposingException(
                f"Cannot load {png_path} with UnicodeDecodeError, "
                "please report your setup at "
                "https://github.com/libvips/pyvips/issues/80"
            ) from None
        if image.interpretation != "srgb":
            image = image.colourspace("srgb")

        try:
            if not image.hasalpha():
                image = image.addalpha()
            if image.get_typeof("icc-profile-data") != 0:
                image = image.icc_transform("srgb")
        except Vips.Error as vips_error:
            log.error("%s: %s", png_path, vips_error)

        if (
            image.width != self.sprite_width
            or image.height != self.sprite_height
        ):
            log.error(
                "%s is %sx%s, but %s sheet sprites have to be %sx%s.",
                png_path,
                image.width,
                image.height,
                self.name,
                self.sprite_width,
                self.sprite_height,
            )

        return image

    def process_json(
        self,
        filepath: Path,
    ) -> None:
        """
        Load and store tile entries from the file
        """
        with open(filepath, "r", encoding="utf-8") as file:
            try:
                tile_entries = json.load(file)
            except Exception:
                log.error("error loading %s", filepath)
                raise

            if not isinstance(tile_entries, list):
                tile_entries = [tile_entries]
            from .TileEntry import TileEntry

            for input_entry in tile_entries:
                self.tile_entries.append(TileEntry(self, input_entry, filepath))

    def write_composite_png(self) -> bool:
        """
        Compose and save tilesheet PNG if there are sprites to work with
        """
        if not self.sprites:
            return False

        # count empty spaces in the last row
        self.tileset.pngnum += self.sprites_across - (
            (len(self.sprites) % self.sprites_across) or self.sprites_across
        )

        if self.tileset.only_json:
            return True

        sheet_image = Vips.Image.arrayjoin(
            self.sprites, across=self.sprites_across
        )

        pngsave_args = PNGSAVE_ARGS.copy()

        if self.tileset.palette:
            pngsave_args["palette"] = True

        sheet_image.pngsave(str(self.output), **pngsave_args)

        if self.tileset.palette_copies and not self.tileset.palette:
            sheet_image.pngsave(f"{self.output}8", palette=True, **pngsave_args)

        return True
