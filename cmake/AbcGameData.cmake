# Reuse the pinned Renderer consumer's catalog tooling and contract.
function(abc_add_gamedata target manifest)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(tools "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../thirdparty/Renderer/scripts")
    string(TOLOWER "${target}" catalog)
    set(output "${PROJECT_BINARY_DIR}/assets/svencoop/metahook/gamedata/${catalog}")
    add_custom_target(${target}GameData
        COMMAND "${Python3_EXECUTABLE}" "${tools}/sync-gamedata.py"
            --manifest "${manifest}" --target-dir "${output}"
            --temp-root "${PROJECT_BINARY_DIR}/gamedata-sync"
        COMMAND "${Python3_EXECUTABLE}" "${tools}/validate-gamedata.py"
            "${output}" --manifest "${manifest}"
        COMMENT "Synchronizing and validating ${target} gamedata"
        VERBATIM)
    add_dependencies(${target} ${target}GameData)
    if(ABC_GAME_DIR)
        set(destination "${ABC_GAME_DIR}/svencoop/metahook/gamedata/${catalog}")
    else()
        set(destination "svencoop/metahook/gamedata/${catalog}")
    endif()
    install(DIRECTORY "${output}/" DESTINATION "${destination}")
endfunction()
