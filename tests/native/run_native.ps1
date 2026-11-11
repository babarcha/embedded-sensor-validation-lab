
$ErrorActionPreference = "Stop"

$root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path

$src = Join-Path $root "firmware\main\sensor_service.c"
$test = Join-Path $PSScriptRoot "test_sensor_service.c"
$exe = Join-Path $PSScriptRoot "test_sensor_service.exe"

$gcc = "C:\msys64\ucrt64\bin\gcc.exe"

# Ensure GCC internal tools can locate MSYS2 DLLs.
$env:Path = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:Path"

if (-not (Test-Path $gcc)) {
    throw "GCC not found: $gcc"
}

& $gcc `
    -std=c11 `
    -Wall -Wextra -Werror `
    -I (Join-Path $PSScriptRoot "stubs") `
    -I (Join-Path $root "firmware\main") `
    $src `
    $test `
    -o $exe

if ($LASTEXITCODE -ne 0) {
    throw "Native compilation failed (exit code $LASTEXITCODE)"
}

& $exe

if ($LASTEXITCODE -ne 0) {
    throw "Native tests failed (exit code $LASTEXITCODE)"
}
