# PDF Generator

The PDF generator is the single implementation for template document
generation. It preserves the legacy `config.yml` format:

- root config: `root-directory`, `latex-pre`, `latex-post`, `title`, `author`
- directory config: recursive `contents`
- item fields: `name`, `directory`, `codes`, `code-pre`, `code-post`

## Commands

Generate the default full document:

```sh
pip install -r tools/pdf/requirements.txt
python -m tools.pdf --preset default --output build/output.tex
```

Generate both full and compact documents:

```sh
python -m tools.pdf --preset both --output-dir build
```

Use a custom root config:

```sh
python -m tools.pdf --config tools/pdf/configs/default.yml --output build/output.tex
```

Regenerate directory manifests after moving or renaming files:

```sh
python -m tools.pdf.configgen src/alfred -r
```

`renderer.py` is the rendering seam, `loader.py` handles configuration
loading, and `configgen.py` is the cross-platform replacement for the old
`gen.sh`.
