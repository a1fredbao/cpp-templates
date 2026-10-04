#!/usr/bin/env bash
set -euo pipefail

mkdir -p fonts
curl -fL -o ./fonts/SourceHanSansHWSC-Regular.otf \
  https://raw.githubusercontent.com/adobe-fonts/source-han-sans/release/OTF/SimplifiedChineseHW/SourceHanSansHWSC-Regular.otf
curl -fL -o ./fonts/SourceHanSansHWSC-Bold.otf \
  https://raw.githubusercontent.com/adobe-fonts/source-han-sans/release/OTF/SimplifiedChineseHW/SourceHanSansHWSC-Bold.otf
