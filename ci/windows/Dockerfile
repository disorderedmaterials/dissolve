FROM mcr.microsoft.com/windows/servercore:ltsc2019
LABEL Description="Windows Server Core / Qt 5.12.2 / Chocolatey"

# Disable crash dialog for release-mode runtimes
RUN reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting" /v Disabled /t REG_DWORD /d 1 /f
RUN reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting" /v DontShowUI /t REG_DWORD /d 1 /f

# Install Qt5 5.12.2
COPY qtifwsilent.qs C:\\qtifwsilent.qs
RUN powershell -NoProfile -ExecutionPolicy Bypass -Command \
    $ErrorActionPreference = 'Stop'; \
    $Wc = New-Object System.Net.WebClient ; \
    $Wc.DownloadFile('http://download.qt.io/archive/qt/5.12/5.12.2/qt-opensource-windows-x86-5.12.2.exe', 'C:\qt.exe') ; \
    Echo 'Downloaded qt-opensource-windows-x86-5.12.2.exe' ; \
    $Env:QT_INSTALL_DIR = 'C:\\Qt' ; \
    Start-Process C:\qt.exe -ArgumentList '--verbose --script C:/qtifwsilent.qs' -NoNewWindow -Wait ; \
    Remove-Item C:\qt.exe -Force ; \
    Remove-Item C:\qtifwsilent.qs -Force
ENV QTDIR C:\\Qt\\5.12.2\\msvc2015
ENV QTDIR64 C:\\Qt\\5.12.2\\msvc2015_64
RUN dir "%QTDIR64%" && dir "%QTDIR64%\bin"

# Install choco for psuedo package manager
RUN @powershell -NoProfile -ExecutionPolicy Bypass -Command \
    $Env:chocolateyVersion = '0.10.14' ; \
    $Env:chocolateyUseWindowsCompression = 'false' ; \
    "iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"
RUN choco install -y python2 --version 2.7.14 && refreshenv && python --version && pip --version
RUN choco install -y qbs && qbs --version
RUN choco install -y unzip --version 6.0 && unzip -v
RUN choco install -y vcbuildtools -ia "/Full"
RUN choco install -y zip --version 3.0 && zip -v
RUN choco install -y cmake
RUN choco install -y windows-sdk-10.1
RUN choco install -y vcredist2008 --ve
