cmake_minimum_required(VERSION 3.24)

project(AbcCore LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/AbcCommon.cmake")

set(ABC_METAHOOK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/MetaHook/include")

option(ABC_WITH_HTTP "Build the optional abc::Http module (needs UtilHTTPClient PluginLib headers)" OFF)

set(ABC_CORE_SOURCES
    src/Core.cpp
    src/Math.cpp
    src/Plugin.cpp
    src/Console.cpp
    src/CvarCmd.cpp
    src/Task.cpp
    src/Events.cpp
    src/Config.cpp
    src/Paths.cpp
    src/Platform.cpp
    src/Render.cpp
    src/SteamId.cpp
)
if(ABC_WITH_HTTP)
    list(APPEND ABC_CORE_SOURCES src/Http/HttpClient.cpp)
endif()

add_library(abc_core STATIC ${ABC_CORE_SOURCES})
add_library(Abc::Core ALIAS abc_core)

target_include_directories(abc_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        "${ABC_METAHOOK_INCLUDE_DIR}"
        "${ABC_METAHOOK_INCLUDE_DIR}/Interface"
        "${ABC_METAHOOK_INCLUDE_DIR}/HLSDK/common"
        "${ABC_METAHOOK_INCLUDE_DIR}/HLSDK/cl_dll"
        "${ABC_METAHOOK_INCLUDE_DIR}/HLSDK/engine"
        "${ABC_METAHOOK_INCLUDE_DIR}/HLSDK/pm_shared"
        "${ABC_METAHOOK_INCLUDE_DIR}/HLSDK/public"
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Renderer/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Renderer/include/Interface"
        # Public abc headers pull these in (Events.h -> eventpp, Config.h ->
        # AIGCJson, SteamId.h -> SteamSDK).
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/eventpp/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/AIGCJson/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SteamSDK/steam"
)
if(ABC_WITH_HTTP)
    target_include_directories(abc_core PUBLIC "${ABC_HTTPCLIENT_INCLUDE_DIR}")
    target_compile_definitions(abc_core PUBLIC ABC_WITH_HTTP=1)
endif()

target_compile_features(abc_core PUBLIC cxx_std_20)

target_link_libraries(abc_core PUBLIC user32 gdi32 shell32 advapi32 winmm)
