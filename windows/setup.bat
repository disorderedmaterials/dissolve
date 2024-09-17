@echo off

::
echo DEBUG: Setting python path variable
::
set "PYTHON_PATH=%~1" & :: Path to an MSVC-compiled python interpreter (version 3.9 or above)

::
echo DEBUG: Setting premake build preset variable
::
set "DISSOLVE_PRESET=%~2" & :: Build preset for the application

::
echo DEBUG: Checking python installation
::
if "%PYTHON_PATH%"=="" (
    ::
    echo DEBUG: Python path = ''
    ::
    for /f "tokens=2 delims= " %%i in ('python -c "import platform; print(platform.python_compiler())"') do (
        set PYTHON_COMPILER=%%i
    )
    echo Python compiler: %PYTHON_COMPILER%

    echo %PYTHON_COMPILER% | findstr /r /c:"MSC" >nul
    if %errorlevel% neq 0 (
        echo The supplied Python interpreter must be compiled with MSVC.
        exit /b 1
    )
    set "PYTHON=python"

) else (
    ::
    echo DEBUG: Python path supplied
    ::
    set "MSVC_PYTHON=%~1"

    if not exist "%MSVC_PYTHON%" (
        echo The specified Python executable does not exist: %MSVC_PYTHON%
        exit /b 1
    )
    set "PYTHON=%MSVC_PYTHON%"
)

::
echo DEBUG: Checking python version
::
%PYTHON% --version | findstr /r "^Python [3][9-9]\.[0-9]*$" >nul
if %errorlevel% neq 0 (
    echo Setting up Conan environment requires Python version 3.9 or later.
    exit /b 1
)

::
echo DEBUG: Creating python venv
::
%PYTHON% -m venv dissolve-conan-env

::
echo DEBUG: Activating python venv
::
call .\dissolve-conan-env\Scripts\activate

::
echo DEBUG: Upgrading pip
::
python -m pip install --upgrade pip

::
echo DEBUG: Installing conan
::
python -m pip install conan

::
echo DEBUG: Checking conan detect state
::
echo Detecting conan profile
conan profile detect

echo Conan environment preparent using the Python version:
echo python -c "import sys; print(sys.version)"

::
echo DEBUG: Setting premake build variable
::
set "PREMAKE_BUILD=%DISSOLVE_PRESET%"
::
echo DEBUG: Setting conan libs variable
::
set "CONAN_LIBS="

REM Default to debug for conan packages
::
echo DEBUG: Setting default build type to debug
::
set "BUILD_TYPE=Debug"

::
echo DEBUG: Assigning premake build
::
if "%PREMAKE_BUILD%"=="gui-debug" (
    ::
    echo DEBUG: gui-debug -> gui=True
    ::
    REM set "CONAN_LIBS=gui=True"
    set "CONAN_LIBS=--options gui=True"
)
if "%PREMAKE_BUILD%"=="gui-debug-test" (
    ::
    echo DEBUG: gui-debug-test -> gui=True,tests=True
    ::
    REM set "CONAN_LIBS=gui=True"
    set "CONAN_LIBS=--options gui=True --options tests=True"
)
if "%PREMAKE_BUILD%"=="gui-release" (
    ::
    echo DEBUG: gui-release -> gui=True, Release
    ::
    REM set "CONAN_LIBS=gui=True"
    set "CONAN_LIBS=--options gui=True"
    set "BUILD_TYPE=Release"
)
if "%PREMAKE_BUILD%"=="cli-release" (
    ::
    echo DEBUG: cli-release -> Release
    ::
    set "CONAN_LIBS="
    set "BUILD_TYPE=Release"
)
if "%PREMAKE_BUILD%"=="cli-debug" (
    ::
    echo DEBUG: cli-debug 
    ::
    set "CONAN_LIBS="
)
if "%PREMAKE_BUILD%"=="cli-debug-test" (
    ::
    echo DEBUG: cli-debug-test -> tests=True
    ::
    REM set "CONAN_LIBS=tests=True"
    set "CONAN_LIBS=--options tests=True"
)
if "%PREMAKE_BUILD%"=="all" (
    ::
    echo DEBUG: All
    ::
    set "CONAN_LIBS=--options all=True"
)

::
echo DEBUG: Run conan install
::
if "%CONAN_LIBS%"=="" (
    set "PREMAKE_BUILD=cli-debug"
    ::
    echo DEBUG: Install base packages
    ::
    conan install ./vendor --build missing --output-folder=./dependencies --settings=build_type=%BUILD_TYPE% --settings compiler.cppstd=17
) else (
    ::
    echo DEBUG: Install packages with options
    ::
    conan install ./vendor --build missing --output-folder=./dependencies --settings=build_type=%BUILD_TYPE% --settings compiler.cppstd=17 %CONAN_LIBS%
)

call deactivate

cd vendor

python ../patch_qt_vars.py %BUILD_TYPE%

premake5 --verbose vs2022 --config=%PREMAKE_BUILD%

