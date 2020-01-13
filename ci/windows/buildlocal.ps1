# Local build - Run from Dissolve topdir

$env:DISSOLVE_DIR = "$($HOME)\build\dissolve"
echo $env:DISSOLVE_DIR
$env:FREETYPE_DIR = "$($HOME)\build\ft"
echo $env:FREETYPE_DIR
$env:FTGL_DIR = "$($HOME)\build\ftgl"
echo $env:FTGL_DIR
$env:Qt5_DIR = "C:\Qt\5.13.1\mingw73_64"
echo $env:Qt5_DIR
$env:MINGW_DIR = $env:Qt5_DIR
iscc.exe /O.\ .\ci\windows\dissolve.iss
$exe = Get-ChildItem  .\*.exe
echo "Executable installer is "$exe.Name
innoextract.exe $exe.Name -d $exe.BaseName
mv "$($exe.BaseName)\app\bin\*" $exe.BaseName
mv "$($exe.BaseName)\app" ./
#rm -Force "$($exe.BaseName)\app"
$zip = $exe.BaseName + ".zip"
zip -r $zip $exe.BaseName
