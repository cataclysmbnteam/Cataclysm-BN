from dataclasses import dataclass, field
from typing import TypedDict
from itertools import chain, count, cycle, repeat


OTHER_COLORS = ("RED", "GREEN", "BLUE", "CYAN", "MAGENTA", "YELLOW")

COLORS = list(
    chain(
        zip(cycle((False, True)), ("BLACK", "WHITE", "WHITE", "BLACK")),
        zip(repeat(False), OTHER_COLORS),
        zip(repeat(True), OTHER_COLORS),
    )
)


class Ascii(TypedDict):
    offset: int
    bold: bool
    color: str


def get_colors() -> list[Ascii]:
    return [
        {"offset": offset, "bold": pair[0], "color": pair[1]}
        for offset, pair in zip(count(0, step=256), COLORS)
    ]


@dataclass
class FallBack:
    file: str = "fallback.png"
    tiles: list[str] = field(default_factory=list)
    ascii: list[Ascii] = field(default_factory=get_colors)

    sprite_width: int | None = None
    sprite_height: int | None = None
    sprite_offset_x: int | None = None
    sprite_offset_y: int | None = None
    sprite_offset_x_retracted: int | None = None
    sprite_offset_y_retracted: int | None = None
    pixelscale: float | None = None


FALLBACK = FallBack()
