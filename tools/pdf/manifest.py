from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class PdfConfig:
    name: str
    root: Path
    title: str
    author: str
    prelude: Path | None
    postlude: Path | None
