# Flag: Use existing Qt installation (requires Qt6_DIR environment variable to be set)
param (
    [switch]$systemqt = $false
)

$colors = @{
    ForegroundColor = "White"
    BackgroundColor = "Cyan"
}

[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12

$projectDir = Get-Location

$threading = [bool]::Parse('True')

$dependencies = "dependencies"
New-Item -ItemType Directory -Path $dependencies -ErrorAction SilentlyContinue

#Install key dependencies with Chocolatey
Write-Host "Installing key dependencies with Chocolatey... " @colors
choco install -y git ninja wget pkgconfiglite

Write-Host "Locating git executable... " @colors
$gitExePath = where.exe git

# Setup Python packages
Write-Host "Creating a local Python virtual environment... " @colors
python -m venv msvc-env

Write-Host "Checking Python compiler type... " @colors
if ($(python -c "import sys; print(sys.version)") -match "MSC v\.\d+") 
{ 
    Write-Host " Python...compiler type evaluated to MSC" @colors
    $pythonEnvSourceDir = "Scripts" 
}
else 
{ 
    Write-Host " Python...compiler type is not MSC" @colors
    $pythonEnvSourceDir = "bin" 
}

$activate = "./msvc-env/$pythonEnvSourceDir/Activate.ps1"

Write-Host "Activating virtual environment with the command: $activate... " @colors
& $activate

Write-Host "Installing Python packages... " @colors
python -m pip install --upgrade pip
python -m pip install pprintjson conan aqtinstall conan==1.*

if (-not $systemqt) 
{
    # Install Qt6
    $qtVersion = "6.4.2"
    $qtInstallationDir = Join-Path -Path $dependencies -ChildPath "qt"
    New-Item -ItemType Directory -Path $qtInstallationDir -ErrorAction SilentlyContinue

    Write-Host "Installing Qt6... " @colors
    aqt install-qt --outputdir $qtInstallationDir windows desktop $qtVersion win64_msvc2019_64 -m all

    # Export Qt6_DIR to system environment variables
    $qt6Dir = Join-Path -Path $dependencies -ChildPath "qt\$qtVersion\msvc_2019_64"
    $qt6BinDir = Join-Path -Path $qt6Dir -ChildPath "bin"
    
    Write-Host "Locating system PATH... " @colors
    $systemPath = [Environment]::GetEnvironmentVariable("PATH", [EnvironmentVariableTarget]::Machine)

    Write-Host "Adding Qt6 directory to system PATH... " @colors
    if ($systemPath -notmatch [regex]::Escape($qt6BinDir)) {
        [Environment]::SetEnvironmentVariable("PATH", "$qt6BinDir;$systemPath", [EnvironmentVariableTarget]::Machine)
        Write-Host "Qt6 binary directory path added to system PATH." @colors
    } else {
        Write-Host "Did not write to PATH: Qt6 binary directory path already exists in system PATH." @colors
    }
}

# Build/retrieve Freetype
$freetypeVersion = "2.12.1"
$freetypeArchive = "https://download.savannah.gnu.org/releases/freetype/freetype-$freetypeVersion.tar.gz"
$freetypeRepo = "freetype-latest"
$freetypeInstall = "freetype-install"
$freetypeOutput = "freetype.tgz"

$freetypeInstallDir = (Join-Path -Path $dependencies -ChildPath $freetypeInstall)
New-Item -ItemType Directory -Path $freetypeInstallDir -ErrorAction SilentlyContinue

$freetypeBuildDir = (Join-Path -Path $dependencies -ChildPath "freetype-build")
New-Item -ItemType Directory -Path $freetypeBuildDir -ErrorAction SilentlyContinue

Write-Host "Downloading freetype archive... " @colors
Invoke-WebRequest -Uri $freetypeArchive -OutFile $freetypeOutput

Write-Host "Unpacking freetype... " @colors
tar -zxvf $freetypeOutput -C $dependencies

Remove-Item -Path $freetypeOutput -Force
Rename-Item -Path (Join-Path -Path $dependencies -ChildPath "freetype-$freetypeVersion") -NewName $freetypeRepo

Write-Host "Building freetype (from location: $freetypeBuildDir)... " @colors
Set-Location -Path $freetypeBuildDir

cmake ../$freetypeRepo -G Ninja -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_C_COMPILER=cl -DBUILD_SHARED_LIBS:STRING=ON -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BZip2:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_PNG:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB:bool=true -DCMAKE_DISABLE_FIND_PACKAGE_BrotliDec:bool=true -DCMAKE_INSTALL_PREFIX:path=../$freetypeInstall
cmake --build . --target install --config Release

# Build/retrieve FTGL
Set-Location -Path $projectDir

$ftglUri = "https://github.com/disorderedmaterials/ftgl-2.4.0.git"
$ftglRepo = "ftgl-latest"
$ftglInstall = "ftgl-install"
$freetypeRepoPath = (Join-Path -Path $dependencies -ChildPath $freetypeRepo)

$ftglInstallDir = (Join-Path -Path $dependencies -ChildPath $ftglInstall)
New-Item -ItemType Directory -Path $ftglInstallDir -ErrorAction SilentlyContinue

$ftglBuildDir = (Join-Path -Path $dependencies -ChildPath "ftgl-build")
New-Item -ItemType Directory -Path $ftglBuildDir -ErrorAction SilentlyContinue

$ftglRepoPath = (Join-Path -Path $dependencies -ChildPath "ftgl-latest")

Write-Host "Cloning FTGL (DisorderedMaterials fork) repo... " @colors
& "$gitExePath" clone $ftglUri $ftglRepoPath

Set-Location -Path $projectDir

$ftglLibPath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\lib"
$ftglBinPath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\bin"
$ftglIncludePath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\include"

$freetypeBinDir = Join-Path -Path $freetypeInstallDir -ChildPath "bin"
$freetypeLibDir = Join-Path -Path $freetypeInstallDir -ChildPath "lib"

Write-Host "Building FTGL (from location: $ftglBuildDir)... " @colors
Set-Location -Path $ftglBuildDir

cmake ../$ftglRepo -G Ninja -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_INSTALL_PREFIX:path=../$ftglInstall -DFREETYPE_LIBRARY=../$freetypeInstall/lib -DFREETYPE_INCLUDE_DIRS="$(Join-Path -Path $projectDir -ChildPath $freetypeInstallDir)\include\freetype2"
cmake --build . --target install --config Release

# Get ANTLR and Java
Set-Location -Path $projectDir

$antlrVersion = "4.13.1"
$antlrUri = "https://www.antlr.org/download/antlr-$antlrVersion-complete.jar"
$antlrOutput = "antlr-$antlrVersion-complete.jar"

$javaUri = "https://download.oracle.com/java/21/latest/jdk-21_windows-x64_bin.zip"
$javaOutput = "java.zip"
$jdkVersion = "21.0.5"

Set-Location -Path $dependencies

Write-Host "Downloading ANTLR... " @colors
Invoke-WebRequest -Uri $antlrUri -OutFile $antlrOutput

Write-Host "Downloading Java... " @colors
Invoke-WebRequest -Uri $javaUri -OutFile $javaOutput

Write-Host "Unpacking Java... " @colors
Expand-Archive -Path $javaOutput -DestinationPath . -Force
Remove-Item -Path $javaOutput -Force

$javaSDKPath = Join-Path -Path $projectDir -ChildPath "$dependencies\jdk-$jdkVersion"
$javaExePath = Join-Path -Path $javaSDKPath -ChildPath "bin\java"

$antlrExePath = "$(Join-Path -Path $projectDir -ChildPath "$dependencies")\$antlrOutput"
New-Item -ItemType Directory -Path $antlrExePath -ErrorAction SilentlyContinue
Move-Item -Path $antlrOutput -Destination $antlrExePath

# Set Conan
Set-Location -Path $projectDir

Write-Host "Setting up Conan profile... " @colors
conan profile new default --detect
conan profile update settings.compiler="Visual Studio" default
conan profile update settings.compiler.version=17 default

# Generate Cmake user presets JSON for MSVC Cmake configurations
$out = Join-Path -Path $projectDir -ChildPath "build"
$cacheVariables = @{
    CMAKE_C_COMPILER = "cl"
    CMAKE_CXX_COMPILER = "cl"
    FTGL_LIBRARY = "$ftglLibPath\ftgl.lib;$ftglBinPath\ftgl.dll"
    FTGL_INCLUDE_DIR = $ftglIncludePath
    ANTLR_EXECUTABLE = $antlrExePath
    Java_JAVA_EXECUTABLE = $javaExePath
    MULTI_THREADING = $threading
    CMAKE_INSTALL_PREFIX = $out
    fmt_DIR = "$out\fmt\include"
    antlr4-runtime_DIR = "$out\antlr4-cppruntime"
    toml11_DIR = "$out\toml11\include"
    pugixml_DIR = "$out\pugixml"
    CLI11_DIR = "$out\CLI11\include"
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
        name = "CLI-Release-MSVC"
        displayName = "CLI Release Build"
        description = "The preset for a CLI production build without tests on MSVC"
        inherits = @("CLI-Release")
    },
    [PSCustomObject]@{
        name = "CLI-Debug-MSVC"
        displayName = "CLI Debug Build"
        description = "The preset for a CLI debug build with tests on MSVC"
        inherits = @("CLI-Debug")
    },
    [PSCustomObject]@{
        name = "GUI-Release-MSVC"
        displayName = "GUI Release Build"
        description = "The preset for a GUI production build without tests on MSVC"
        inherits = @("GUI-Release")
    },
    [PSCustomObject]@{
        name = "GUI-Debug-MSVC"
        displayName = "GUI Debug Build"
        description = "The preset for a GUI debug build with tests on MSVC"
        inherits = @("GUI-Debug")
    }
)

foreach ($preset in $presets) {
    $preset | Add-Member -MemberType NoteProperty -Name cacheVariables -Value $cacheVariables
    $cmakeUserPresets.configurePresets += $preset
}

Write-Host "Outputting CMakeUserPresets Json for Dissolve MSVC configuration... " @colors
$cmakeUserPresetsJson = $cmakeUserPresets | ConvertTo-Json -Depth 10 -Compress

Set-Content -Path "CMakeUserPresets.json" -Value $cmakeUserPresetsJson -Encoding UTF8



