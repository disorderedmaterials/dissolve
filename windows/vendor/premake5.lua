include "../dependencies/conandeps.premake5.lua"
include "../dissolve_vars.lua"

local basepath = "../../"

--Product name
workspace(Solution)
    -- architecture "x64"
    architecture "x86_64"
    configurations { "debug", "release" }
    platforms { "Win32", "Win64" }

    newoption {
        trigger     = "config",
        value       = "PREMAKE_BUILD",
        description = "Build preset",
        default     = "cli-debug",
        category    = "Build Options",
        allowed = {
           { "cli-release", "CLI app" },
           { "cli-debug", "CLI app with debug symbols" },
           { "cli-debug-test", "CLI app with debug symbols, tests and benchmarks" },
           { "gui-release", "GUI app" },
           { "gui-debug", "GUI app with debug symbols" },
           { "gui-debug-test", "GUI app with debug symbols, tests and benchmarks" },
           { "all", "GUI app with debug symbols, tests and benchmarks" }
        }
    }

    if _ACTION:startswith("vs") then
        require("vstudio")
    end

    --Solution-wide properties
    language "C++"
    cppdialect "C++17"

    filter { "configurations:debug",  "configurations:release" }
        targetdir "build/%{cfg.buildcfg}/bin"
        objdir "build/%{cfg.buildcfg}/obj"

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "On"

    if _OPTIONS["config"] == "cli-release" or _OPTIONS["config"] == "cli-debug" then
        
        --Project: Dissolve
        project "Dissolve"
            kind "ConsoleApp"
            local _src={}

            table.insert(_src, basepath .. Src .. "/main.cpp")

            for _, dir in ipairs(Basic_libs) do
                table.insert(_src, basepath .. Src .. dir .. "/**.h")
                table.insert(_src, basepath .. Src .. dir .. "/**.cpp")
            end

            files(_src)
            conan_setup()
    end

    if _OPTIONS["config"] == "gui-release" or _OPTIONS["config"] =="gui-debug" then

        --Project: Dissolve
        project "Dissolve"
            language "C++"
            cppdialect "C++17"
            buildoptions { "/Zc:__cplusplus", "/permissive-" }

            local _src={}
            local _links=Gui_libs_extern

            filter "configurations:debug"
                _links=Gui_libs_extern_debug

            table.insert(_src, basepath .. Src .. "/main-gui.cpp")

            for _, dir in ipairs(Basic_libs) do
                table.insert(_src, basepath .. Src .. dir .. "/**.h")
                table.insert(_src, basepath .. Src .. dir .. "/**.cpp")
            end

            for _, dir in ipairs(Gui_libs) do
                table.insert(_src, basepath .. Src .. dir .. "/**.h")
                table.insert(_src, basepath .. Src .. dir .. "/**.cpp")
                table.insert(_src, basepath .. Src .. dir .. "/**.ui")
            end

            kind "WindowedApp"
            files(_src)
            links(_links)
            conan_setup()

            filter {}
    end

    if _OPTIONS["config"] == "cli-debug-test" or _OPTIONS["config"] == "gui-debug-test" or _OPTIONS["config"] == "all" then

        --Project: Dissolve
        project "Dissolve"
            language "C++"
            cppdialect "C++17"

            local _src={}
            local _gui=false
            local _kind="ConsoleApp"

            for _, dir in ipairs(Basic_libs) do
                table.insert(_src, basepath .. Src .. dir .. "/**.h")
                table.insert(_src, basepath .. Src .. dir .. "/**.cpp")
            end

            if _gui then
                _gui=true
                _kind="WindowedApp"
                links(Gui_libs_extern_debug)
                buildoptions { "/Zc:__cplusplus", "/permissive-" }

                table.insert(_src, basepath .. Src .. "/main-gui.cpp")
                for _, dir in ipairs(Gui_libs) do
                    table.insert(_src, basepath .. Src .. dir .. "/**.h")
                    table.insert(_src, basepath .. Src .. dir .. "/**.cpp")
                    table.insert(_src, basepath .. Src .. dir .. "/**.ui")
                end     
            else
                table.insert(_src, basepath .. Src .. "/main.cpp")
            end

            files(_src)
            kind(_kind)
            conan_setup()

        --Project: Tests
        project "Tests"
            kind "ConsoleApp"
            local _tests={}

            table.insert(_tests, basepath .. Tests .. "/main.cpp")

            files(_tests)
            conan_setup()

        --Project: Benchmark
        project "Benchmark"
            kind "ConsoleApp"
            local _benchmark={}

            table.insert(_benchmark, basepath .. Benchmark .. "/main.cpp")

            files(_benchmark)
            conan_setup()
    end

    filter "action:vs*"
        buildoptions { "/utf-8", "/Zc:__cplusplus" }

