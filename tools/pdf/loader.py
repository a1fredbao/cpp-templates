from pathlib import Path
from typing import Any

from .manifest import PdfConfig


def read_yaml(path: str | Path) -> dict[str, Any]:
    path = Path(path)
    if not path.is_file():
        raise FileNotFoundError(f"config file not found: {path}")

    try:
        import yaml
    except ModuleNotFoundError as error:
        raise RuntimeError(
            "PyYAML is required. Install it with: "
            "pip install -r tools/pdf/requirements.txt"
        ) from error

    with path.open("r", encoding="utf-8") as file:
        data = yaml.safe_load(file)
    return data or {}


def load_config(path: str | Path) -> PdfConfig:
    path = Path(path)
    data = read_yaml(path)

    root_value = data.get("root-directory", data.get("root"))
    if not root_value:
        raise ValueError(f"missing root-directory in {path}")

    prelude_value = data.get("latex-pre", data.get("prelude"))
    postlude_value = data.get("latex-post", data.get("postlude"))

    return PdfConfig(
        name=str(data.get("name", path.stem)),
        root=Path(root_value),
        title=str(data.get("title", "XCPC Templates")),
        author=str(data.get("author", "")),
        prelude=Path(prelude_value) if prelude_value else None,
        postlude=Path(postlude_value) if postlude_value else None,
    )
