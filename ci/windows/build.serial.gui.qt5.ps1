# Dissolve Windows Build
# One of a sequence of scripts.
# - Install Qt 5.13.1

$ErrorActionPreference = 'Stop'

# Attempt to free up some disk space before we install
choco uninstall -y mingw llvm wsl microsoft-build-tools

# Install Qt 5.13.1
$Wc = New-Object System.Net.WebClient
$Wc.DownloadFile('http://download.qt.io/archive/qt/5.13/5.13.1/qt-opensource-windows-x86-5.13.1.exe', 'qt.exe')
echo 'Downloaded qt-opensource-windows-x86-5.13.1.exe'
$env:QT_INSTALL_DIR = 'C:\\Qt'
Start-Process qt.exe -ArgumentList '--verbose --script ci\windows\qtifwsilent.qs' -NoNewWindow -Wait
Remove-Item qt.exe

# Set environment
$env:QTDIR = "C:\\Qt\\5.13.1\\mingw73_64"
$env:PATH += "C:\\Qt\\Tools\\mingw730_64\\bin;"
$env:PATH += "C:\\Qt\\5.13.1\\mingw73_64\\bin;"
