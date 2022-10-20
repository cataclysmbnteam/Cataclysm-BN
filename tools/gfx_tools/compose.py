#!/usr/bin/env python3
"""
Merge all tile entries and PNGs in a compositing tileset directory into
a tile_config.json and tilesheet .png file(s) ready for use in BN.

Examples:

    %(prog)s /gfx/Retrodays/
    %(prog)s --use-all gfx/MSX++UndeadPeopleEdition/

By default, output is written back to the source directory. Pass an output
directory as the last argument to place output files there instead. The
output directory will be created if it does not already exist.
"""

import argparse
import logging
import sys
from logging.config import dictConfig
from pathlib import Path

from compose import Tileset
from compose.log import (
    log,
    FailFastHandler,
    LevelTrackingFilter,
    LOGGING_CONFIG,
)


# variable for silent script run (no output to terminal)
run_silent = True

# variable for progress bar support (tqdm module dependency)
no_tqdm = False

# File name to ignore containing directory
ignore_file = ".scratch"


class ComposingException(Exception):
    """
    Base class for all composing exceptions
    """


def fake_main() -> int | ComposingException:
    """
    Called when the script is executed directly
    """
    # read arguments and initialize objects
    arg_parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    arg_parser.add_argument(
        "source_dir", type=Path, help="Tileset source files directory path"
    )
    arg_parser.add_argument(
        "output_dir", nargs="?", type=Path, help="Output directory path"
    )
    arg_parser.add_argument(
        "--use-all",
        dest="use_all",
        action="store_true",
        help="Add unused images with id being their basename",
    )
    arg_parser.add_argument(
        "--obsolete-fillers",
        dest="obsolete_fillers",
        action="store_true",
        help="Warn about obsoleted fillers",
    )
    arg_parser.add_argument(
        "--palette-copies",
        dest="palette_copies",
        action="store_true",
        help="Produce copies of tilesheets quantized to 8bpp colormaps.",
    )
    arg_parser.add_argument(
        "--palette",
        dest="palette",
        action="store_true",
        help="Quantize all tilesheets to 8bpp colormaps.",
    )
    arg_parser.add_argument(
        "--format-json",
        dest="format_json",
        action="store_true",
        help="Use either BN formatter or Python json.tool "
        "to format the tile_config.json",
    )
    arg_parser.add_argument(
        "--only-json",
        dest="only_json",
        action="store_true",
        help="Only output the tile_config.json",
    )
    arg_parser.add_argument(
        "--fail-fast",
        dest="fail_fast",
        action="store_true",
        help="Stop immediately after an error has occurred",
    )
    arg_parser.add_argument(
        "--loglevel",
        dest="loglevel",
        choices=["INFO", "WARNING", "ERROR"],  # 'DEBUG', 'CRITICAL'
        default="WARNING",
        help="set verbosity level",
    )
    arg_parser.add_argument(
        "--feedback",
        dest="feedback",
        choices=["SILENT", "CONCISE", "VERBOSE"],
        default="SILENT",
        help="When SILENT no output to terminal is given (run silently)."
        " CONCISE displays limited progress feedbeck with no dependency"
        " required. VERBOSE displays progress bar(s) that require TQDM"
        " module (will prompt for installation if absent).",
    )

    args_dict = vars(arg_parser.parse_args())

    dictConfig(LOGGING_CONFIG)
    log.setLevel(getattr(logging, args_dict.get("loglevel")))
    log_tracker = LevelTrackingFilter()
    log.addFilter(log_tracker)

    if args_dict.get("fail_fast"):
        failfast_handler = FailFastHandler()
        failfast_handler.setLevel(logging.ERROR)
        log.addHandler(failfast_handler)

    if args_dict["feedback"] == "SILENT":
        global run_silent
        run_silent = True

    if args_dict["feedback"] == "VERBOSE":
        run_silent = False
        args_dict["feedback"] = setup_progress_bar()  # may fallback to CONCISE

    if args_dict["feedback"] == "CONCISE":
        run_silent = False
        global no_tqdm
        no_tqdm = True  # equal to concise display

    # compose the tileset
    try:
        tileset_worker = Tileset(
            source_dir=Path(args_dict.get("source_dir")),
            output_dir=Path(
                args_dict.get("output_dir") or args_dict.get("source_dir")
            ),
            use_all=args_dict.get("use_all", False),
            obsolete_fillers=args_dict.get("obsolete_fillers", False),
            palette_copies=args_dict.get("palette_copies", False),
            palette=args_dict.get("palette", False),
            format_json=args_dict.get("format_json", False),
            only_json=args_dict.get("only_json", False),
        )
        tileset_worker.compose(ignore_file=ignore_file)
    except ComposingException as exception:
        return exception

    if log_tracker.level >= logging.ERROR:
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(fake_main())
