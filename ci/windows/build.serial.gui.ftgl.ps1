# Dissolve Windows Build
# One of a sequence of scripts.
# - Build Freetype

$ErrorActionPreference = 'Stop'

# Retrieve / compile FTGL
echo 'Compiling FTGL'
git clone 'https://github.com/frankheckenbach/ftgl.git' ftgl-latest
cd $HOME
mkdir ftgl-build
cd ftgl-build
cmake -G "Ninja" ..\ftgl-latest
ninja
cd ../

# Set environment
$env:INCLUDE += "$HOME\\ftgl-latest\\src;"
$env:LIB += "$HOME\\ftgl-build\\src;"

