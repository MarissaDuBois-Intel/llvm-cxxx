param(
  [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\..\..\..\build-cppp\editors'),
  [string]$CodeExecutable
)
$ErrorActionPreference = 'Stop'
if (-not $CodeExecutable) {
  $command = (Get-Command code -ErrorAction Stop).Source
  $CodeExecutable = [IO.Path]::GetFullPath((Join-Path (Split-Path $command) '..\Code.exe'))
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
$env:CXXX_EDITOR_TEST_DIR = Join-Path $OutputDirectory 'test-files'
$source = Join-Path $PSScriptRoot 'vscode'
$tests = Join-Path $source 'integration-tests.cjs'
& $CodeExecutable "--extensionDevelopmentPath=$source" "--extensionTestsPath=$tests" `
  "--user-data-dir=$(Join-Path $OutputDirectory 'vscode-test-data')" `
  "--extensions-dir=$(Join-Path $OutputDirectory 'vscode-test-extensions')" `
  --disable-extensions --skip-welcome --skip-release-notes | Out-Host
if ($LASTEXITCODE -ne 0) { throw "VS Code tests failed ($LASTEXITCODE)." }
