import argparse
from pathlib import Path


def base_name(filename: str) -> str | None:
    if filename.endswith((".hpp", ".h", ".json", ".py")):
        return Path(filename).stem
    if filename.endswith("-pre.tex"):
        return filename.removesuffix("-pre.tex")
    if filename.endswith("-post.tex"):
        return filename.removesuffix("-post.tex")
    return None


def generate_config(directory: Path, recursive: bool = False) -> Path:
    entries: list[dict[str, object]] = []
    processed: set[str] = set()

    for entry in sorted(directory.iterdir(), key=lambda path: path.name):
        if entry.name.startswith(".") or entry.name == "config.yml":
            continue

        if entry.is_dir():
            entries.append({"name": entry.name, "directory": entry.name})
            if recursive:
                generate_config(entry, recursive=True)
            continue

        name = base_name(entry.name)
        if name is None or name in processed:
            continue
        processed.add(name)

        codes = []
        for suffix in (".hpp", ".h", ".json", ".py"):
            if (directory / f"{name}{suffix}").is_file():
                codes.append(f"{name}{suffix}")

        item: dict[str, object] = {"name": name, "codes": codes}
        if (directory / f"{name}-pre.tex").is_file():
            item["code-pre"] = f"{name}-pre.tex"
        if (directory / f"{name}-post.tex").is_file():
            item["code-post"] = f"{name}-post.tex"
        entries.append(item)

    try:
        import yaml
    except ModuleNotFoundError as error:
        raise RuntimeError(
            "PyYAML is required. Install it with: "
            "pip install -r tools/pdf/requirements.txt"
        ) from error

    output = directory / "config.yml"
    output.write_text(
        yaml.safe_dump(
            {"contents": entries},
            sort_keys=False,
            allow_unicode=True,
        ),
        encoding="utf-8",
    )
    return output


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="tools.pdf.configgen",
        description="Regenerate config.yml files for the PDF generator.",
    )
    parser.add_argument("-r", "--recursive", action="store_true")
    parser.add_argument("directories", nargs="+", type=Path)
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    for directory in args.directories:
        if not directory.is_dir():
            print(f"skip non-directory: {directory}")
            continue
        output = generate_config(directory, args.recursive)
        print(f"generated {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
