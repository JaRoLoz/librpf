project "librpf"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"

    files { "include/**.h*", "**.c*" }
    includedirs { "include", "../zlib" }
    links { "zlib" }