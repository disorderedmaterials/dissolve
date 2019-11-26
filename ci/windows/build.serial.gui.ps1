# Dissolve Build Script

$ErrorActionPreference = 'Stop'

# Attempt to free up some disk space
choco uninstall -y mingw llvm wsl microsoft-build-tools

# Install Qt5 5.13.1
$Wc = New-Object System.Net.WebClient
$Wc.DownloadFile('http://download.qt.io/archive/qt/5.13/5.13.1/qt-opensource-windows-x86-5.13.1.exe', 'qt.exe')
echo 'Downloaded qt-opensource-windows-x86-5.13.1.exe'
$env:QT_INSTALL_DIR = 'C:\\Qt'
Start-Process qt.exe -ArgumentList '--verbose --script ci\windows\qtifwsilent.qs' -NoNewWindow -Wait
Remove-Item qt.exe
$env:QTDIR = "C:\\Qt\\5.13.1\\mingw73_64"

# Install prerequisites
choco install -y windows-sdk-10-version-1803-all
choco install -y unzip --version 6.0
choco install -y zip --version 3.0
choco install -y cmake
choco install -y ninja
choco install -y winflexbison
choco install -y innosetup
choco install -y 7zip
choco install -y innounp

# Add binary paths
$env:PATH += "C:\Qt\Tools\mingw730_64\bin;"
$env:PATH += "C:\Qt\5.13.1\mingw73_64\bin;"
$env:PATH += "C:\Program Files\CMake\bin;"

# Make a directory for 3rd party package builds and descend
mkdir 3rdparty
cd 3rdparty

# Retrieve / compile Freetype 2.10.1
$Wc = New-Object System.Net.WebClient
$Wc.DownloadFile('https://download.savannah.gnu.org/releases/freetype/ft2101.zip', 'ft.zip')
unzip ft.zip
Echo 'Compiling FreeType...'
mkdir freetype-build
cd freetype-build
cmake -G "Ninja" ..\freetype-2.10.1
ninja
cd ../
$env:INCLUDE += "$HOME\3rdparty\freetype-2.10.1\include;"
$env:LIB += "$HOME\3rdparty\freetype-build;"

# Retrieve / compile FTGL
$env:INCLUDE += "$HOME\3rdparty\freetype-2.10.1;"
$env:LIB += "$HOME\3rdparty\freetype-build;"
echo 'Compiling FTGL'
git clone 'https://github.com/frankheckenbach/ftgl.git' ftgl-latest
mkdir ftgl-build
cd ftgl-build
cmake -G "Ninja" ..\ftgl-latest
ninja
cd ../
$env:INCLUDE += "$HOME\3rdparty\ftgl-latest\src;"
$env:LIB += "$HOME\3rdparty\ftgl-build\src;"

# Return to main dir and create build dir for dissolve
cd ../
mkdir build
cd build
cmake -G "Ninja" -DGUI:bool=true ../
ninja
