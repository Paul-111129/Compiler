workspace "Compiler"
    architecture "x64"
    location "."

    configurations
    {
        "Debug",
        "Release"
    }

project "Compiler"
    language "C++"
    kind "ConsoleApp"

    pchheader "src/pch.h"
    pchsource "src/pch.cpp"

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp"
    }
    
    filter "system:windows"
        cppdialect "C++20"
    filter {}
    
    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        optimize "On"