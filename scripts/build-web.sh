#!/usr/bin/env bash
set -euo pipefail

emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

echo "Build complete: build/web/gta4_web.html"
echo "Run: python3 -m http.server 8080 -d build/web"
