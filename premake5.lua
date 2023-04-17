workspace "re2d"
    configurations { "debug", "release" }

project "re2d"
    kind "ConsoleApp"
    language "C"
    cdialect "C99"

    files { "src/*.c" }

    includedirs {
        "src",
        "/opt/homebrew/Cellar/raylib/4.5.0/include"
    }

    libdirs {
        "/opt/homebrew/Cellar/raylib/4.5.0/lib"
    }

    links { "raylib" }

    filter "configurations:debug"
        defines { "DEBUG" }
        optimize "Debug"

    filter "configurations:release"
        optimize "Speed"
