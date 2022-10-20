from pathlib import Path
import subprocess
import json
from typing import Any

from .log import log

class ComposingException(Exception):
    """
    Base class for all composing exceptions
    """

def read_properties(filepath: str) -> dict[str, str]:
    """
    tileset.txt reader
    """
    with open(filepath, "r", encoding="utf-8") as file:
        pairs: dict[str, str] = {}
        for line in file.readlines():
            line = line.strip()
            if line and not line.startswith("#"):
                key, value = line.split(":")
                pairs[key.strip()] = value.strip()

    return pairs


def write_to_json(
    pathname: str,
    data: dict[str, Any] | list[Any],
    format_json: bool = False,
) -> None:
    """
    Write data to a JSON file
    """
    kwargs: dict[str, Any] = {
        "ensure_ascii": False,
    }
    if format_json:
        kwargs["indent"] = 2

    with open(pathname, "w", encoding="utf-8") as file:
        json.dump(data, file, **kwargs)

    if not format_json:
        return

    json_formatter = Path("tools/format/json_formatter.cgi")
    if json_formatter.is_file():
        cmd = [json_formatter, pathname]
        subprocess.call(cmd)
    else:
        log.warning(
            "%s not found, Python built-in formatter was used.", json_formatter
        )
