param(
  [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\..\..\..\build-cppp\editors'),
  [string]$VisualStudioInstallDir
)
$ErrorActionPreference = 'Stop'
if (-not $VisualStudioInstallDir) {
  $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
  $VisualStudioInstallDir = & $vswhere -latest -products * -property installationPath
  if ($LASTEXITCODE -ne 0 -or -not $VisualStudioInstallDir) {
    throw 'Visual Studio was not found. Set -VisualStudioInstallDir.'
  }
}
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$msbuild = Join-Path $VisualStudioInstallDir 'MSBuild\Current\Bin\MSBuild.exe'
$project = Join-Path $PSScriptRoot 'visual-studio\CXXX.SyntaxHighlighting.csproj'
$bin = Join-Path $OutputDirectory 'bin'
$obj = Join-Path $OutputDirectory 'obj'
& $msbuild $project /nologo /v:minimal /restore /t:Test `
  "/p:VisualStudioInstallDir=$VisualStudioInstallDir" `
  "/p:OutputPath=$bin\" "/p:BaseIntermediateOutputPath=$obj\" `
  "/p:IntermediateOutputPath=$obj\"
if ($LASTEXITCODE -ne 0) { throw 'Visual Studio extension build or tests failed.' }

Add-Type -AssemblyName System.IO.Compression, System.IO.Compression.FileSystem
function Write-Vsix([string]$Name, [hashtable]$Files) {
  $path = Join-Path $OutputDirectory $Name
  if (Test-Path -LiteralPath $path) { Remove-Item -LiteralPath $path }
  $zip = [IO.Compression.ZipFile]::Open($path, [IO.Compression.ZipArchiveMode]::Create)
  try {
    [IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
      $zip, (Join-Path $PSScriptRoot 'ContentTypes.xml'), '[Content_Types].xml') | Out-Null
    foreach ($entry in $Files.GetEnumerator()) {
      [IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
        $zip, $entry.Value, $entry.Key) | Out-Null
    }
  } finally { $zip.Dispose() }
  Write-Output $path
}

$code = Join-Path $PSScriptRoot 'vscode'
Write-Vsix 'cxxx-vscode.vsix' @{
  'extension.vsixmanifest' = Join-Path $code 'extension.vsixmanifest'
  'extension/package.json' = Join-Path $code 'package.json'
  'extension/extension.js' = Join-Path $code 'extension.js'
  'extension/cxxx.tmLanguage.json' = Join-Path $code 'cxxx.tmLanguage.json'
  'extension/language-configuration.json' = Join-Path $code 'language-configuration.json'
}
$vsix = Join-Path $OutputDirectory 'cxxx-visual-studio.vsix'
Copy-Item -LiteralPath (Join-Path $bin 'CXXX.SyntaxHighlighting.vsix') -Destination $vsix
Write-Output $vsix
