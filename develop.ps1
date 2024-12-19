# Flag: Use existing Qt installation (requires Qt6_DIR environment variable to be set)
param (
    [switch]$systemqt = $false,
    [switch]$release = $false
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

$warn_colors = @{
    ForegroundColor = "White"
    BackgroundColor = "Red"
}

Write-Host "Building dependencies in $build configuration... " @info_colors

[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12

$projectDir = Get-Location

$threading = [bool]::Parse('True')

$dependencies = "dependencies"
New-Item -ItemType Directory -Path $dependencies -ErrorAction SilentlyContinue

#Install key dependencies with Chocolatey
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

Write-Host "Installing key dependencies with Chocolatey... " @info_colors
choco install -y git ninja pkgconfiglite

Write-Host "Locating git executable... " @info_colors
$gitExePath = where.exe git

# Setup Python packages
Write-Host "Creating a local Python virtual environment... " @info_colors
python -m venv msvc-env

Write-Host "Checking Python compiler type... " @info_colors
if ($(python -c "import sys; print(sys.version)") -match "MSC v\.\d+") 
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

Write-Host "Activating virtual environment with the command: $activate... " @info_colors
& $activate

Write-Host "Installing Python packages... " @info_colors
python -m pip install --upgrade pip
python -m pip install conan aqtinstall conan==1.*

if (-not $systemqt) 
{
    # Install Qt6
    $qtVersion = "6.4.2"
    $qtInstallationDir = Join-Path -Path $dependencies -ChildPath "qt"
    New-Item -ItemType Directory -Path $qtInstallationDir -ErrorAction SilentlyContinue

    Write-Host "Installing Qt6... " @info_colors
    aqt install-qt --outputdir $qtInstallationDir windows desktop $qtVersion win64_msvc2019_64 -m all

    # Export Qt6_DIR to system environment variables
    $qt6Dir = Join-Path -Path $dependencies -ChildPath "qt\$qtVersion\msvc2019_64"
    $qt6BinDir = Join-Path -Path $qt6Dir -ChildPath "bin"
    
    Write-Host "Locating system PATH... " @info_colors
    $systemPath = [Environment]::GetEnvironmentVariable("PATH", [EnvironmentVariableTarget]::Machine)

    Write-Host "Adding Qt6 directory to system PATH... " @info_colors
    if ($systemPath -notmatch [regex]::Escape($qt6BinDir)) {
        [Environment]::SetEnvironmentVariable("PATH", "$(Join-Path -Path $projectDir -ChildPath $qt6BinDir);$systemPath", [EnvironmentVariableTarget]::Machine)
        Write-Host "Qt6 binary directory path added to system PATH." @info_colors
    } else {
        Write-Host "Did not write to PATH: Qt6 binary directory path already exists in system PATH." @info_colors
    }
} else {
    Write-Host "Attempting to use existing system installation of Qt6... " @info_colors
    $systemPath = [Environment]::GetEnvironmentVariable("PATH", [EnvironmentVariableTarget]::Machine)

    if ($systemPath -notmatch [regex]::Escape($qtVersion)) {
        Write-Host "Found Qt6 version that is NOT ${qtVersion} in system PATH. It is strongly recommended to use Qt ${qtVersion}" @warn_colors
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
Rename-Item -Path (Join-Path -Path $dependencies -ChildPath "freetype-$freetypeVersion") -NewName $freetypeRepo

Write-Host "Building freetype (from location: $freetypeBuildDir)... " @info_colors
Set-Location -Path $freetypeBuildDir

cmake ../$freetypeRepo -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE:STRING=$build -DCMAKE_C_COMPILER=cl -DBUILD_SHARED_LIBS:STRING=ON -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BZip2:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_PNG:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec:bool=true -DCMAKE_INSTALL_PREFIX:path=../$freetypeInstall
cmake --build . --target install --config $build

$freetypeLib = "$freetypeInstall\lib"
$freetypeBin = "$freetypeInstall\bin"

$freetypeLibPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeLib"
$freetypeBinPath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeBin"

$lib = [System.Environment]::GetEnvironmentVariable("LIB", [System.EnvironmentVariableTarget]::Machine)

if ($lib -notlike "*$freetypeInstall*") {
    Write-Host "Setting LIB environment variable with Freetype library... " @info_colors
    [System.Environment]::SetEnvironmentVariable("LIB", "$freetypeLibPath;$freetypeBinPath;$lib", [System.EnvironmentVariableTarget]::Machine)
}

$freetypeIncludePath =  Join-Path -Path $projectDir -ChildPath "$dependencies\$freetypeRepo"

$include = [System.Environment]::GetEnvironmentVariable("INCLUDE", [System.EnvironmentVariableTarget]::Machine)

if ($include -notlike "*$freetypeRepo*") {
    Write-Host "Setting INCLUDE environment variable with Freetype includes... " @info_colors
    [System.Environment]::SetEnvironmentVariable("INCLUDE", "$freetypeIncludePath;$include", [System.EnvironmentVariableTarget]::Machine)
}

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
& "$gitExePath" clone $ftglUri $ftglRepoPath

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

$antlrVersion = "4.13.1"
$antlrUri = "https://www.antlr.org/download/antlr-$antlrVersion-complete.jar"
$antlrOutput = "antlr-$antlrVersion-complete.jar"

$javaUri = "https://download.oracle.com/java/21/latest/jdk-21_windows-x64_bin.zip"
$javaOutput = "java.zip"
$jdkVersion = "21.0.5"

Set-Location -Path $dependencies

Write-Host "Downloading ANTLR... " @info_colors
Invoke-WebRequest -Uri $antlrUri -OutFile $antlrOutput

Write-Host "Downloading Java... " @info_colors
Invoke-WebRequest -Uri $javaUri -OutFile $javaOutput

Write-Host "Unpacking Java... " @info_colors
Expand-Archive -Path $javaOutput -DestinationPath . -Force
Remove-Item -Path $javaOutput -Force

$javaSDKPath = Join-Path -Path $projectDir -ChildPath "$dependencies\jdk-$jdkVersion"
$javaExePath = Join-Path -Path $javaSDKPath -ChildPath "bin\java"

$antlrExePath = "$(Join-Path -Path $projectDir -ChildPath "$dependencies")\$antlrOutput"
New-Item -ItemType Directory -Path $antlrExePath -ErrorAction SilentlyContinue
Move-Item -Path $antlrOutput -Destination $antlrExePath

# Set Conan
Set-Location -Path $projectDir

Write-Host "Setting up Conan profile... " @info_colors
conan profile new default --detect
conan profile update settings.compiler="Visual Studio" default
conan profile update settings.compiler.version=17 default

# Generate Cmake user presets JSON for MSVC Cmake configurations
$out = Join-Path -Path $projectDir -ChildPath "build"
$cacheVariables = @{
    CMAKE_C_COMPILER = "cl"
    CMAKE_CXX_COMPILER = "cl"
    FTGL_LIBRARY = "$ftglLibPath\ftgl$binSuffix.lib"
    FTGL_INCLUDE_DIR = $ftglIncludePath
    ANTLR_EXECUTABLE = $antlrExePath
    Java_JAVA_EXECUTABLE = $javaExePath
    MULTI_THREADING = $threading
    MSVC_DEV = "ON"
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
        inherits = @("CLI-$build")
    },
    [PSCustomObject]@{
        name = "GUI-$build-MSVC"
        displayName = "GUI $build Build"
        description = "The preset for a GUI $build build on MSVC"
        inherits = @("GUI-$build")
    }
)

foreach ($preset in $presets) {
    $preset | Add-Member -MemberType NoteProperty -Name cacheVariables -Value ($cacheVariables + @{
        CONFIG = "$($preset.name)-x64"
    })
    $cmakeUserPresets.configurePresets += $preset
}

Write-Host "Outputting CMakeUserPresets Json for Dissolve MSVC configuration... " @info_colors
$cmakeUserPresetsJson = $cmakeUserPresets | ConvertTo-Json -Depth 10 -Compress

Set-Content -Path "CMakeUserPresets.json" -Value $cmakeUserPresetsJson -Encoding UTF8



