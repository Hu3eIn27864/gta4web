$ErrorActionPreference = "Stop"

emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

Write-Host "Build complete: build/web/gta4_web.html"
Write-Host "Run: python -m http.server 8080 -d build/web"
