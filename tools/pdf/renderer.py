import re
from pathlib import Path
from typing import Any

from .loader import read_yaml
from .manifest import PdfConfig


def escape_latex_special_chars(text: str) -> str:
    special_chars = {
        "\\": r"\textbackslash{}",
        "&": r"\&",
        "%": r"\%",
        "$": r"\$",
        "#": r"\#",
        "_": r"\_",
        "{": r"\{",
        "}": r"\}",
        "~": r"\textasciitilde{}",
        "^": r"\textasciicircum{}",
    }
    return re.sub(
        "|".join(re.escape(key) for key in special_chars),
        lambda match: special_chars[match.group()],
        text,
    )


def read_file(path: str | Path) -> str:
    path = Path(path)
    if not path.is_file():
        raise FileNotFoundError(f"file not found: {path}")
    return path.read_text(encoding="utf-8")


def detect_style_by_extension(filename: str) -> str:
    if filename.endswith((".cpp", ".hpp", ".h")):
        return "cppstyle"
    if filename.endswith(".java"):
        return "javastyle"
    if filename.endswith(".py"):
        return "pythonstyle"
    return ""


def latex_path(path: Path) -> str:
    return path.as_posix()


def find_directory_config(directory: Path) -> Path:
    for filename in ("config.yml", "config.yaml"):
        path = directory / filename
        if path.is_file():
            return path
    raise FileNotFoundError(f"config file not found in {directory}")


def render_item(directory: Path, item: dict[str, Any], depth: int) -> str:
    if "name" not in item:
        raise ValueError(f"missing name in {directory / 'config.yml'}")

    name = escape_latex_special_chars(str(item["name"]))
    if depth == 0:
        heading = f"\\section{{{name}}}\n"
    elif depth == 1:
        heading = f"\\subsection{{{name}}}\n"
    elif depth == 2:
        heading = f"\\subsubsection{{{name}}}\n"
    else:
        raise ValueError(f"config depth exceeds 3: {directory}")

    parts = [heading]

    if "code-pre" in item:
        parts.append(read_file(directory / item["code-pre"]) + "\n")

    codes = item.get("codes")
    if isinstance(codes, str):
        codes = [codes]
    elif codes is None:
        codes = []
    elif not isinstance(codes, list):
        raise ValueError(f"invalid codes field in {directory / 'config.yml'}")

    for code_file in codes:
        path = directory / code_file
        if not path.is_file():
            raise FileNotFoundError(f"code file not found: {path}")
        style = detect_style_by_extension(str(code_file))
        if style:
            parts.append(
                f"\\lstinputlisting[style={style}]{{{latex_path(path)}}}\n"
            )
        else:
            parts.append(f"\\lstinputlisting{{{latex_path(path)}}}\n")

    if "code-post" in item:
        parts.append(read_file(directory / item["code-post"]) + "\n")

    return "\n".join(parts)


def render_directory(directory: Path, depth: int = 0) -> str:
    config = read_yaml(find_directory_config(directory))
    sections: list[str] = []

    for item in config.get("contents") or []:
        if "directory" in item:
            subdirectory = directory / str(item["directory"])
            if not subdirectory.is_dir():
                raise NotADirectoryError(
                    f"subdirectory not found: {subdirectory}"
                )
            sections.append(render_item(directory, item, depth))
            sections.append(render_directory(subdirectory, depth + 1))
        else:
            sections.append(render_item(directory, item, depth))

    return "\n".join(sections)


def render_tex(config: PdfConfig) -> str:
    prelude = read_file(config.prelude) if config.prelude else ""
    postlude = read_file(config.postlude) if config.postlude else ""

    title = escape_latex_special_chars(config.title)
    author = escape_latex_special_chars(config.author)
    prelude = prelude.replace("{PLACEHOLDER:TITLE}", title)
    prelude = prelude.replace("{PLACEHOLDER:AUTHOR}", author)

    return prelude + render_directory(config.root) + postlude
