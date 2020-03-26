#!/bin/bash

# Script to package Dissolve GUI (and serial) versions on OSX systems.
# Intended to be called as a job within GitHub Actions.

#
# Configuration
#
# -- Enforce exit on first failure
$ErrorActionPreference = 'Stop'
echo $env:PATH
$env:DISSOLVE_DIR = $(Get-Location).path + "\build"
$env:FREETYPE_DIR = "$($HOME)\freetype-build"
$env:FTGL_DIR = "$($HOME)\ftgl-build"
echo $env:Qt5_DIR
$env:MINGW_DIR = $env:Qt5_DIR

#
# Install Prerequisites
#
# -- General
choco install -y zip innoextract

#
# Create Installer
#
iscc.exe /O.\ .\ci\windows\dissolve-gui.iss
$exe = Get-ChildItem  .\*.exe
echo "Executable installer is "$exe.Name

#
# Create Zip
#
innoextract.exe $exe.Name -d $exe.BaseName
mv "$($exe.BaseName)\app\bin\*" $exe.BaseName
mv "$($exe.BaseName)\app" ./
#rm -Force "$($exe.BaseName)\app"
$zip = $exe.BaseName + ".zip"
zip -r $zip $exe.BaseName

#
# Move packages to their own dir
#
mkdir packages
mv $zip packages
mv $exe.Name packages
