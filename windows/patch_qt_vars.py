import sys
import os

debug_path = "..\\dependencies\\conan_qt_vars_debug_x86_64.premake5.lua"
release_path = "..\\dependencies\\conan_qt_vars_release_x86_64.premake5.lua"

if __name__=="__main__":
    mode = sys.argv[1].lower()

    if mode == "debug":
        file = debug_path
    elif mode == "release":
        file = release_path
    else:
        print("Invalid argument - not in 'debug' or 'release'")
        sys.exit(1)
    try:
        with open(file, "r", encoding="utf-8") as f:
            content = f.read()

        content = content.replace("\\", "/")

        with open(file, "w", encoding="utf-8") as f:
            f.write(content)
                    
        print(f"File {file} has been corrected.")

    except FileNotFoundError:
        print("Relevant lua script not found. This may be a CLI build, or Qt has not been installed by Conan")