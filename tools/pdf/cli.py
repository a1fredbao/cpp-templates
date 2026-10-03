import argparse
from pathlib import Path

from .loader import load_config
from .renderer import render_tex

PRESETS = {
    "default": Path("tools/pdf/configs/default.yml"),
    "compact": Path("tools/pdf/configs/compact.yml"),
}


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="tools.pdf", description="Generate the XCPC template LaTeX source."
    )
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument(
        "--preset",
        choices=("default", "compact", "both"),
        help="Use a built-in full, compact, or both configuration.",
    )
    mode.add_argument(
        "--config",
        "--config-file",
        dest="config",
        type=Path,
        help="Path to a root config.yml.",
    )
    parser.add_argument(
        "--root-dir",
        type=Path,
        help="Load config.yml from this directory when --config is omitted.",
    )
    parser.add_argument(
        "--output",
        "--output-file",
        dest="output",
        type=Path,
        help="Output .tex path for a single preset or config.",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("build"),
        help="Output directory used by --preset both.",
    )
    return parser


def generate(config_path: Path, output: Path) -> None:
    config = load_config(config_path)
    latex = render_tex(config)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(latex, encoding="utf-8")
    print(f"generated {output}")


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    try:
        if args.preset == "both":
            for name, config_path in PRESETS.items():
                suffix = "-compact" if name == "compact" else ""
                generate(config_path, args.output_dir / f"output{suffix}.tex")
            return 0

        if args.preset is not None:
            config_path = PRESETS[args.preset]
        elif args.root_dir is not None:
            config_name = args.config if args.config is not None else Path("config.yml")
            config_path = args.root_dir / config_name
        elif args.config is not None:
            config_path = args.config
        else:
            config_path = PRESETS["default"]

        output = args.output or Path("build/output.tex")
        generate(config_path, output)
    except (FileNotFoundError, NotADirectoryError, RuntimeError, ValueError) as error:
        print(f"pdf generator error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
