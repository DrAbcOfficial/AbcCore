cmake_minimum_required(VERSION 3.24)

project(AbcCore LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/AbcCommon.cmake")

set(ABC_METAHOOK_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/MetaHook/include")

add_library(abc_core STATIC
    src/Core.cpp
)
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
)

target_compile_features(abc_core PUBLIC cxx_std_20)

target_link_libraries(abc_core PUBLIC user32 gdi32 shell32 advapi32 winmm)
