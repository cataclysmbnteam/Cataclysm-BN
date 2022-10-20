#!/usr/bin/env python3

import logging
from logging.config import dictConfig
from pathlib import Path
from textwrap import dedent
from enum import StrEnum

import typer
from typer import Argument as Arg, Option as Opt

from compose.Tileset import Tileset
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


class LogLevel(StrEnum):
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"


class FeedBack(StrEnum):
    SILENT = "SILENT"
    CONCISE = "CONCISE"
    VERBOSE = "VERBOSE"


FEEDBACK_DESC = dedent(
    """
    When SILENT no output to terminal is given (run silently)."
    CONCISE displays limited progress feedbeck with no dependency"
    required. VERBOSE displays progress bar(s) that require TQDM"
    module (will prompt for installation if absent).",
    """
)
CONTEXT_SETTINGS = {"help_option_names": ["-h", "--help"]}

app = typer.Typer(context_settings=CONTEXT_SETTINGS)


# fmt: off
@app.command()
def main(
    source_dir:       Path     = Arg(...,                         help="Tileset source files directory path"),
    output_dir:       Path     = Arg(None,                        help="Output directory path"),

    use_all:          bool     = Opt(False, "--use_all",          help="Add unused images with id being their basename"),
    obsolete_fillers: bool     = Opt(False, "--obsolete-fillers", help="Warn about obsoleted fillers"),
    palette_copies:   bool     = Opt(False, "--palette-copies",   help="Produce copies of tilesheets quantized to 8bpp colormaps."),
    palette:          bool     = Opt(False, "--palette",          help="Quantize all tilesheets to 8bpp colormaps."),
    format_json:      bool     = Opt(False, "--format-json",      help="Use either BN formatter or Python json.tool"),
    only_json:        bool     = Opt(False, "--only-json",        help="Only output the tile_config.json"),
    fail_fast:        bool     = Opt(False, "--fail-fast",        help="Stop immediately after an error has occurred"),

    loglevel:         LogLevel = Opt(LogLevel.WARNING,            help="set verbosity level"),
    feedback:         FeedBack = Opt(FeedBack.SILENT,             help=FEEDBACK_DESC),
) -> int | ComposingException:
# fmt: on
    """
    Merge all tile entries and PNGs in a compositing tileset directory into
    a tile_config.json and tilesheet .png file(s) ready for use in BN.

    Examples:
        compose.py /gfx/Retrodays/
        compose.py --use-all gfx/MSX++UndeadPeopleEdition/

    By default, output is written back to the source directory. Pass an output
    directory as the last argument to place output files there instead. The
    output directory will be created if it does not already exist.
    """

    if output_dir is None:
        output_dir = source_dir


    dictConfig(LOGGING_CONFIG)
    log.setLevel(getattr(logging, loglevel))
    log_tracker = LevelTrackingFilter()
    log.addFilter(log_tracker)

    if fail_fast:
        failfast_handler = FailFastHandler()
        failfast_handler.setLevel(logging.ERROR)
        log.addHandler(failfast_handler)

    if feedback == "SILENT":
        global run_silent
        run_silent = True

    if feedback == "VERBOSE":
        run_silent = False
        feedback = setup_progress_bar()  # may fallback to CONCISE

    if feedback == "CONCISE":
        run_silent = False
        global no_tqdm
        no_tqdm = True  # equal to concise display

    # compose the tileset
    try:
        tileset_worker = Tileset(
            source_dir,
            output_dir,
            use_all,
            obsolete_fillers,
            palette_copies,
            palette,
            format_json,
            only_json,
        )
        tileset_worker.compose(ignore_file=ignore_file)

    except ComposingException as exception:
        return exception

    if log_tracker.level >= logging.ERROR:
        return 1

    return 0


if __name__ == "__main__":
    app()
