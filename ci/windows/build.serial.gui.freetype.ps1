# Dissolve Windows Build
# One of a sequence of scripts.
# - Build Freetype

$ErrorActionPreference = 'Stop'

echo $env:PATH
echo $Env:PATH
$env:PATH += "C:\\Qt\\Tools\\mingw730_64\\bin;"
echo $env:PATH
echo $Env:PATH


# Retrieve / compile Freetype 2.10.1
cd $HOME
wget 'https://download.savannah.gnu.org/releases/freetype/ft2101.zip' -O 'ft.zip'
unzip ./ft.zip
Echo 'Compiling FreeType...'
mkdir freetype-build
cd freetype-build
cmake -G "Ninja" ../freetype-2.10.1
ls
type CMakeFiles/CMakeOutput.log
ninja

# Set environment
$env:INCLUDE += "$HOME\\freetype-2.10.1;"
$env:LIB += "$HOME\\freetype-build;"

