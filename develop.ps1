<#
    .SYNOPSIS
        Script to install dependencies for Dissolve development environment in Visual Studio.
    .DESCRIPTION
        Installs the following dependencies for Dissolve (separate and prior to Conan-managed packages):
            - Python 3.12 (unless stated otherwise)
            - CMake 3.x
            - ninja
            - pkgconfiglite
            - Qt6 <VERSION>
            - Freetype
            - FTGL
            - Antlr4 (Java backend)
            - Java JDK (latest)
        
        These packages are installed into a folder called 'dependencies'.
    .PARAMETER qtVersion
        Qt version to install. Defaults to existing system Qt6 installation if none specified.
    .PARAMETER systemQt
        Path to existing installation of Qt6.
    .PARAMETER pythonPath
        Path to a Python executable.
    .PARAMETER forcePythonVersion
        Force installation of a given Python version.
    .PARAMETER antlrVersion
        ANTLR version to install. Defaults to ANTLR 4.13.1.
    .PARAMETER msvcVersion
        Version of MSVC to use.
    .PARAMETER generator
        Generator to use (options are "Visual Studio 17 2022", "Ninja").
    .PARAMETER release
        Flag - install packages for release, otherwise debug.
    .PARAMETER clean
        Flag - remove existing setup folders and files before running script. Invoking this flag deletes the following:
            - Dissolve installation folders ("/out", "/build")
            - dependencies folder
            - CMakeUserPresets.json
#>

param (
    [string]$qtVersion,
    [string]$systemQt,
    [string]$pythonPath,
    [string]$forcePythonVersion,
    [string]$msvcVersion,
    [string]$generator = "Visual Studio 17 2022",
    [string]$antlrVersion = "4.13.1",
    [switch]$release = $false,
    [switch]$clean = $false
)

$build = "Debug"
$binSuffix = "d"

if ($release) {
    $build = "Release"
    $binSuffix = ""
}

$info_colors = @{
    ForegroundColor = "White"
    BackgroundColor = "Black"
}

Write-Host "Building dependencies in $build configuration... " @info_colors

[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12

$projectDir = Get-Location

$threading = [bool]::Parse('True')

$dependencies = "dependencies"
New-Item -ItemType Directory -Path $dependencies -ErrorAction SilentlyContinue

function Find-And-Remove {
    <#
        .SYNOPSIS
            Remove Dissolve environment object, and recursively remove contents, if found in Dissolve project directory.
        .DESCRIPTION
            Deletes  specified folder and contents, or file, if it exists. Path is relative to the Dissolve project directory.
        .PARAMETER relativePath
            Relative path to object for deletion.
    #>
    param (
        [string]$relativePath = ""
    )

    if (Test-Path -Path $relativePath)
    {
        Write-Host "Existing instance of object $relativePath found, cleaning up... " @info_colors
        Remove-Item $relativePath -Recurse -Force
    }
    else
    {
        Write-Host "Existing instance of object $relativePath NOT found, could not clean up." @warn_colors
    }
}

function Normalise-Path {
    <#
        .SYNOPSIS
            Normalise path using spearator '\' to '/'.
        .DESCRIPTION
            Changes all occurences of Windows-specific path separator '\' to '/' for compatibility.
        .PARAMETER path
            Path to be normalised.

        .OUTPUT
            Normalised path.
    #>

    param(
        [string]$path
    )

    return (Resolve-Path $path).Path -replace '\\','/'
}

# Clean existing environment
Find-And-Remove -relativePath "out"
Find-And-Remove -relativePath "build"
Find-And-Remove -relativePath "dependencies"
Find-And-Remove -relativePath "CMakeUserPresets.json"
Find-And-Remove -relativePath "msvc-env"
Find-And-Remove -relativePath "cmake/Modules/conan_provider.cmake"

#Install key dependencies with Chocolatey
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

Import-Module $env:ChocolateyInstall\helpers\chocolateyProfile.psm1

Write-Host "Installing key dependencies with Chocolatey... " @info_colors
choco install -y ninja pkgconfiglite
choco install -y cmake --version=3.30.1 --force

# Ensure CMake version is 3.30.1
$cmakeVersion = "$(cmake --version)"
if (-not ($cmakeVersion -like "*3.30.1*"))
{
    choco install -y cmake.install --version=3.30.1 --force --installargs "ADD_CMAKE_TO_PATH=User"
}

# Find git, install if not found
try {
    & "git" --version
    Write-Output "Found system Git..."
} catch {
    Write-Output "Could not find system Git - installing with Chocolatey..."
    choco install -y git
}

# Find python, install if not found
if (-not [string]::IsNullOrEmpty($pythonPath))
{
    Write-Output "Using Python with path $pythonPath..." @info_colors
    $python = $pythonPath
}
else
{
    if (-not [string]::IsNullOrEmpty($forcePythonVersion))
    {
        Write-Output "Installing requested Python version $forcePythonVersion..." @info_colors
        choco install -y python --version=$forcePythonVersion --force
    }
    else
    {
        try {
            & "python" --version
            Write-Output "Found system Python..." @info_colors
            $pythonVersion = $(python -c "import sys; v = sys.version_info; print(v.major == 3, v.minor == 12)")
            $versionParts = $pythonVersion -split " "
            if (-not ($versionParts[0] -eq "True" -and $versionParts[1] -eq "True")) {
                Write-Output "System Python is version $(python --version) and it is recommended to be version == 3.12 - installing with Chocolatey..." @info_colors
                choco install -y python --version=3.12.0
            }
        } catch {
            Write-Output "Could not find system Python - installing with Chocolatey..." @info_colors
            choco install -y python --version=3.12.0
        }
    }

    $python = "python"
}

refreshenv

# Setup Python packages
Write-Host "Creating a local Python virtual environment with $(& $python --version)... " @info_colors
& $python -m venv msvc-env

Write-Host "Checking Python compiler type... " @info_colors
if ($(& $python -c "import sys; print(sys.version)") -match "MSC v\.\d+")
{ 
    Write-Host " ...Python compiler type evaluated to MSC" @info_colors
    $pythonEnvSourceDir = "Scripts"
}
else 
{ 
    Write-Host " ...Python compiler type is not MSC" @info_colors
    $pythonEnvSourceDir = "bin"
}

$activate = "./msvc-env/$pythonEnvSourceDir/Activate.ps1"

Write-Host "Activating Python virtual environment... " @info_colors
& $activate

Write-Host "Installing Python packages... " @info_colors
& $python -m pip install --upgrade pip
& $python -m pip install py7zr==1.1.0 aqtinstall conan

$pythonEnvPath = Join-Path -Path $projectDir -ChildPath "msvc-env\$pythonEnvSourceDir"

# Install Qt6, or find existing system Qt6 installation
$qt6CMakeDir = ""

if (-not [string]::IsNullOrEmpty($qtVersion))
{
    $qtInstallationDir = Join-Path -Path $dependencies -ChildPath "qt"
    New-Item -ItemType Directory -Path $qtInstallationDir -ErrorAction SilentlyContinue

    Write-Host "Installing Qt6 using aqt with $(& $python --version)... " @info_colors
    & $python -m aqt install-qt --outputdir $qtInstallationDir windows desktop $qtVersion win64_msvc2019_64 -m all

    # Export Qt6_DIR to system environment variables
    $qt6Dir = Join-Path -Path "$projectDir\$dependencies" -ChildPath "qt\$qtVersion\msvc2019_64"
    $qt6BinDir = Join-Path -Path $qt6Dir -ChildPath "bin"
    $qt6CMakeDir = Join-Path -Path $qt6Dir -ChildPath "lib\cmake"

} else {
    # We attempt to use an existing installation of Qt
    Write-Host "Attempting to use existing system installation of Qt6... " @info_colors

    if (-not [string]::IsNullOrEmpty($systemQt))
    {
        $qt6Version = Get-ChildItem -Path $systemQt -Directory | Where-Object { $_.Name -match '^\d+\.\d+\.\d+$'} | Select-Object -First 1
        $qt6ToolChain = Get-ChildItem -Path (Join-Path -Path $systemQt -ChildPath $qt6Version) -Directory | Where-Object { $_.Name -match '^msvc\d{4}_x64$'} | Select-Object -ExpandProperty Name
        $qt6BinDir = "$systemQt\$($qt6Version.Name)\$qt6ToolChain\bin"
        $qt6CMakeDir = "$systemQt\$($qt6Version.Name)\$qt6ToolChain\lib\cmake"

        if (-not (Test-Path -Path $qt6CMakeDir -PathType Container))
        {
            Write-Host "Attempted to find the directory $qt6CMakeDir. Could NOT find a valid Qt6 installation." @warn_colors
        }
    }
    else
    {
        Write-Host "Could NOT find a Qt6 installation. No path to Qt6 was supplied." @warn_colors
    }
}

# Build/retrieve Freetype
$freetypeVersion = "2.12.1"
$freetypeArchive = "https://download.savannah.gnu.org/releases/freetype/freetype-$freetypeVersion.tar.gz"
$freetypeRepo = "freetype-repo"
$freetypeInstall = "freetype-install"
$freetypeOutput = "freetype.tgz"

$freetypeInstallDir = (Join-Path -Path $dependencies -ChildPath $freetypeInstall)
New-Item -ItemType Directory -Path $freetypeInstallDir -ErrorAction SilentlyContinue

$freetypeBuildDir = (Join-Path -Path $dependencies -ChildPath "freetype-build")
New-Item -ItemType Directory -Path $freetypeBuildDir -ErrorAction SilentlyContinue

Write-Host "Downloading freetype archive... " @info_colors
Invoke-WebRequest -Uri $freetypeArchive -OutFile $freetypeOutput

Write-Host "Unpacking freetype... " @info_colors
tar -zxvf $freetypeOutput -C $dependencies

Remove-Item -Path $freetypeOutput -Force
try
{
    Rename-Item -Path (Join-Path -Path $dependencies -ChildPath "freetype-$freetypeVersion") -NewName $freetypeRepo -ErrorAction Stop
}
catch
{
    # Move freetype if error on rename
    $fromFreetype = (Join-Path -Path $dependencies -ChildPath "freetype-$freetypeVersion")
    $moveFreetype = (Join-Path -Path $dependencies -ChildPath $freetypeRepo)
    if (-not (Test-Path -Path $moveFreetype))
    {
        New-Item -Path $moveFreetype -ItemType Directory | Out-Null
    }

    Get-ChildItem -Path $fromFreetype -Force | Move-Item -Destination $moveFreetype -Force
    Remove-Item -Path $fromFreetype -Force
}


Write-Host "Building freetype (from location: $freetypeBuildDir)... " @info_colors
Set-Location -Path $freetypeBuildDir

cmake ../$freetypeRepo -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE:STRING=$build -DCMAKE_C_COMPILER=cl -DBUILD_SHARED_LIBS:STRING=ON -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BZip2:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_PNG:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec:bool=true -DCMAKE_INSTALL_PREFIX:path=../$freetypeInstall
cmake --build . --target install --config $build

$freetypeLib = "$freetypeInstall\lib"
$freetypeBin = "$freetypeInstall\bin"

$freetypeLibPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeLib"
$freetypeBinPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeBin"

$lib = [System.Environment]::GetEnvironmentVariable("LIB", [System.EnvironmentVariableTarget]::Machine)

$freetypeIncludePath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeRepo"
$freetype2IncludePath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeInstall\include\freetype2"

$include = [System.Environment]::GetEnvironmentVariable("INCLUDE", [System.EnvironmentVariableTarget]::Machine)

# Build/retrieve FTGL
Set-Location -Path $projectDir

$ftglUri = "https://github.com/disorderedmaterials/ftgl-2.4.0.git"
$ftglRepo = "ftgl-repo"
$ftglInstall = "ftgl-install"
$freetypeRepoPath = (Join-Path -Path $dependencies -ChildPath $freetypeRepo)

$ftglInstallDir = (Join-Path -Path $dependencies -ChildPath $ftglInstall)
New-Item -ItemType Directory -Path $ftglInstallDir -ErrorAction SilentlyContinue

$ftglBuildDir = (Join-Path -Path $dependencies -ChildPath "ftgl-build")
New-Item -ItemType Directory -Path $ftglBuildDir -ErrorAction SilentlyContinue

$ftglRepoPath = (Join-Path -Path $dependencies -ChildPath "ftgl-repo")

Write-Host "Cloning FTGL (DisorderedMaterials fork) repo... " @info_colors
git clone $ftglUri $ftglRepoPath

Set-Location -Path $projectDir

$ftglLibPath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\lib"
$ftglBinPath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\bin"
$ftglIncludePath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\include"

Write-Host "Building FTGL (from location: $ftglBuildDir)... " @info_colors

if (-not $release) {
    Copy-Item -Path "$freetypeBinPath\freetyped.dll" -Destination "$freetypeBinPath\freetype.dll"
    Copy-Item -Path "$freetypeLibPath\freetyped.lib" -Destination "$freetypeLibPath\freetype.lib"
}
    
Set-Location -Path $ftglBuildDir

cmake ../$ftglRepo -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE:STRING=$build -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_INSTALL_PREFIX:path=../$ftglInstall -DFREETYPE_LIBRARY="$(Join-Path -Path $freetypeLibPath -ChildPath "freetype.lib")" -DFREETYPE_INCLUDE_DIRS="$(Join-Path -Path $projectDir -ChildPath $freetypeInstallDir)\include\freetype2"
cmake --build . --target install --config $build

$ftglLib = "$ftglInstall\lib"
$ftglBin = "$ftglInstall\bin"

$ftglLibPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$ftglLib"
$ftglBinPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$ftglBin"

$lib = [System.Environment]::GetEnvironmentVariable("LIB", [System.EnvironmentVariableTarget]::Machine)

if ($lib -notlike "*$ftglInstall*") {
    Write-Host "Setting LIB environment variable with FTGL library... " @info_colors
    [System.Environment]::SetEnvironmentVariable("LIB", "$ftglLibPath;$ftglBinPath;$lib", [System.EnvironmentVariableTarget]::Machine)
}

$ftglInclude = "$ftglRepo\src"
$ftglIncludePath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$ftglInclude"

$include = [System.Environment]::GetEnvironmentVariable("INCLUDE", [System.EnvironmentVariableTarget]::Machine)

if ($include -notlike "*$ftglInclude*") {
    Write-Host "Setting INCLUDE environment variable with FTGL includes... " @info_colors
    [System.Environment]::SetEnvironmentVariable("INCLUDE", "$ftglIncludePath;$include", [System.EnvironmentVariableTarget]::Machine)
}

# Get ANTLR and Java
Set-Location -Path $projectDir

$antlrUri = "https://www.antlr.org/download/antlr-$antlrVersion-complete.jar"
$antlrOutput = "antlr-$antlrVersion-complete.jar"

$javaUri = "https://download.oracle.com/java/21/latest/jdk-21_windows-x64_bin.zip"
$javaOutput = "java.zip"

Set-Location -Path $dependencies

Write-Host "Downloading ANTLR... " @info_colors
Invoke-WebRequest -Uri $antlrUri -OutFile $antlrOutput

Write-Host "Downloading Java... " @info_colors
Invoke-WebRequest -Uri $javaUri -OutFile $javaOutput

Write-Host "Unpacking Java... " @info_colors
Expand-Archive -Path $javaOutput -DestinationPath . -Force
Remove-Item -Path $javaOutput -Force

$jdkVersion = $(Get-ChildItem -Path "." -Directory | Where-Object { $_.Name -match "^jdk-\d+\.\d+\.\d+$" } | Select-Object -ExpandProperty Name).split("-")[1]

$javaSDKPath = Join-Path -Path $projectDir -ChildPath "$dependencies\jdk-$jdkVersion"
$javaExePath = Join-Path -Path $javaSDKPath -ChildPath "bin\java.exe"

$antlrExePath = "$(Join-Path -Path $projectDir -ChildPath "$dependencies")\$antlrOutput"
New-Item -ItemType Directory -Path $antlrExePath -ErrorAction SilentlyContinue
Move-Item -Path $antlrOutput -Destination $antlrExePath

# Set Conan
Set-Location -Path $projectDir

New-Item -ItemType Directory -Path "conan" -ErrorAction SilentlyContinue

$env:CONAN_HOME = Join-Path -Path (Get-Location) -ChildPath "conan"
$conanProfiles = Join-Path -Path $env:CONAN_HOME -ChildPath "profiles"

New-Item -ItemType Directory -Force -Path $conanProfiles | Out-Null
$profileContent = @"
[settings]
arch=x86_64
build_type=$build
compiler=msvc
compiler.cppstd=14
compiler.runtime=dynamic
compiler.version=194
os=Windows

[options]
Dissolve/*:msvc_dev=True
hwloc/*:shared=True
"@

Set-Content -Path (Join-Path -Path $conanProfiles -ChildPath "default") -Value $profileContent -Encoding UTF8

# Generate Cmake user presets JSON for MSVC Cmake configurations
$out = Join-Path -Path $projectDir -ChildPath "build"
$cacheVariables = @{
    CMAKE_C_COMPILER = "cl"
    CMAKE_CXX_COMPILER = "cl"
    FTGL_LIBRARY = Normalise-Path -path "$ftglLibPath\ftgl$binSuffix.lib"
    FTGL_INCLUDE_DIR = Normalise-Path -path $ftglIncludePath
    FREETYPE_LIBRARY = Normalise-Path -path "$freetypeLibPath\freetype$binSuffix.lib"
    FREETYPE_INCLUDE_DIRS = "$(Normalise-Path -path $freetypeIncludePath);$(Normalise-Path -path $freetype2IncludePath)"
    ANTLR_EXECUTABLE = Normalise-Path -path $antlrExePath
    Java_JAVA_EXECUTABLE = Normalise-Path -path $javaExePath
    MULTI_THREADING = $threading
    MSVC_DEV = "ON"
    CMAKE_PREFIX_PATH = Normalise-Path -path "$qt6CMakeDir"
    CMAKE_MODULE_PATH = "`$penv{CONAN_HOME}"
}

# For MSVC version != v143 latest, and Visual Studio generator specified, set toolset with cache variable
if ((-not [string]::IsNullOrEmpty($msvcVersion)) -and ($generator -eq "Visual Studio 17 2022"))
{
    $cacheVariables["CMAKE_GENERATOR_TOOLSET"] = "version=$msvcVersion"
}

# For MSVC version != v143 latest, and Ninja generator specified, set toolset at preset level
if ((-not [string]::IsNullOrEmpty($msvcVersion)) -and ($generator -eq "Ninja"))
{
    $toolset = @{
        value = "version=$msvcVersion"
        strategy = "external"
    }
}

$cmakeUserPresets = [PSCustomObject]@{
    version = 3
    cmakeMinimumRequired = @{
        major = 3
        minor = 21
    }
    configurePresets = @()
}

$presets = @(
    [PSCustomObject]@{
        name = "CLI-$build-MSVC"
        displayName = "CLI $build Build"
        description = "The preset for a CLI $build build on MSVC"
        generator = $generator
        inherits = @("CLI-$build")
    },
    [PSCustomObject]@{
        name = "GUI-$build-MSVC"
        displayName = "GUI $build Build"
        description = "The preset for a GUI $build build on MSVC"
        generator = $generator
        inherits = @("GUI-$build")
    }
)

$environment = @{
    PATH = "$(Normalise-Path -path $qt6BinDir);$(Normalise-Path -path $pythonEnvPath);`$penv{PATH}"
    CONAN_HOME = Normalise-Path -path $env:CONAN_HOME
}

foreach ($preset in $presets) {
    # Set CMake cache variables
    $preset | Add-Member -MemberType NoteProperty -Name cacheVariables -Value $cacheVariables

    # Set environment variables
    $preset | Add-Member -MemberType NoteProperty -Name environment -Value $environment

    # Set toolset
    if ($toolset)
    {
        $preset | Add-Member -MemberType NoteProperty -Name toolset -Value $toolset
    }

    $cmakeUserPresets.configurePresets += $preset
}

Write-Host "Outputting CMakeUserPresets Json for Dissolve MSVC configuration... " @info_colors
$cmakeUserPresetsJson = $cmakeUserPresets | ConvertTo-Json -Depth 10 -Compress

[System.IO.File]::WriteAllText(
    (Join-Path $projectDir "CMakeUserPresets.json"),
    $cmakeUserPresetsJson,
    (New-Object System.Text.UTF8Encoding($false))  # $false = no BOM
)