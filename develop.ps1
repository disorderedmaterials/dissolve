# Flag: Use existing Qt installation (requires Qt6_DIR environment variable to be set)
param (
    [switch]$systemqt = $false
)

[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.SecurityProtocolType]::Tls12

$projectDir = Get-Location

$threading = [bool]::Parse('True')

$dependencies = "dependencies"
New-Item -ItemType Directory -Path $dependencies -ErrorAction SilentlyContinue

#Install key dependencies with Chocolatey
choco install -y git ninja wget pkgconfiglite

$gitExePath = where.exe git

# Setup Python packages
python -m venv msvc-env
./msvc-env/Scripts/Activate.ps1
python -m pip install --upgrade pip
python -m pip install pprintjson conan aqtinstall conan==1.*

if (-not $systemqt) {
    # Install Qt6
    $qtVersion = "6.4.2"
    $qtInstallationDir = Join-Path -Path $dependencies -ChildPath "qt"
    New-Item -ItemType Directory -Path $qtInstallationDir -ErrorAction SilentlyContinue

    aqt install-qt --outputdir $qtInstallationDir windows desktop $qtVersion win64_msvc2019_64 -m all

    # Export Qt6_DIR to system environment variables
    $qt6Dir = Join-Path -Path $dependencies -ChildPath "qt\$qtVersion\msvc_2019_64"
    $qt6BinDir = Join-Path -Path $qt6Dir -ChildPath "bin"
    

    $systemPath = [Environment]::GetEnvironmentVariable("PATH", [EnvironmentVariableTarget]::Machine)

    if ($systemPath -notmatch [regex]::Escape($qt6BinDir)) {
        [Environment]::SetEnvironmentVariable("PATH", "$qt6BinDir;$systemPath", [EnvironmentVariableTarget]::Machine)
        Write-Output "Qt6 binary directory path added to system PATH."
    } else {
        Write-Output "Did not write to PATH: Qt6 binary directory path already exists in system PATH."
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

Invoke-WebRequest -Uri $freetypeArchive -OutFile $freetypeOutput

tar -zxvf $freetypeOutput -C $dependencies

Remove-Item -Path $freetypeOutput -Force
Rename-Item -Path (Join-Path -Path $dependencies -ChildPath "freetype-$freetypeVersion") -NewName $freetypeRepo

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
& "$gitExePath" clone $ftglUri $ftglRepoPath

Set-Location -Path $projectDir

$ftglLibPath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\lib"
$ftglIncludePath = Join-Path -Path "$(Get-Location)" -ChildPath "$dependencies\$ftglInstall\include\FTGL"

$freetypeBinDir = Join-Path -Path $freetypeInstallDir -ChildPath "bin"
$freetypeLibDir = Join-Path -Path $freetypeInstallDir -ChildPath "lib"

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

Invoke-WebRequest -Uri $antlrUri -OutFile $antlrOutput
Invoke-WebRequest -Uri $javaUri -OutFile $javaOutput
Expand-Archive -Path $javaOutput -DestinationPath . -Force
Remove-Item -Path $javaOutput -Force

$javaSDKPath = Join-Path -Path $projectDir -ChildPath "$dependencies\jdk-$jdkVersion"
$javaExePath = Join-Path -Path $javaSDKPath -ChildPath "bin"

$antlrExePath = "$(Join-Path -Path $projectDir -ChildPath "$dependencies")\$antlrOutput"
New-Item -ItemType Directory -Path $antlrExePath -ErrorAction SilentlyContinue
Move-Item -Path $antlrOutput -Destination $antlrExePath

# Set Conan
Set-Location -Path $projectDir
conan profile new default --detect
conan profile update settings.compiler="Visual Studio" default
conan profile update settings.compiler.version=17 default

# Generate Cmake user presets JSON for MSVC Cmake configurations
$out = Join-Path -Path $projectDir -ChildPath "build"
$cacheVariables = @{
    CMAKE_C_COMPILER = "cl"
    CMAKE_CXX_COMPILER = "cl"
    FTGL_LIBRARY = $ftglLibPath
    FTGL_INCLUDE_DIR = $ftglIncludePath
    ANTLR_EXECUTABLE = $antlrExePath
    Java_JAVA_EXECUTABLE = $javaExePath
    MULTI_THREADING = $threading
    CMAKE_INSTALL_PREFIX = $out
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

$cmakeUserPresetsJson = $cmakeUserPresets | ConvertTo-Json -Depth 10 -Compress
$unformattedJson = "presets.json"

Set-Content -Path $unformattedJson -Value $cmakeUserPresetsJson -Encoding UTF8

python -c "from pprintjson import pprintjson; import json, sys; pprintjson(json.load(open('$unformattedJson')), indent=4)" > CMakeUserPresets.json

Remove-Item -Path $unformattedJson -Force



