# Bundled catalog tooling and contract, independent of the Renderer checkout.
function(abc_add_gamedata target manifest)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(tools "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gamedata")
    string(TOLOWER "${target}" catalog)
    # Group every ABC catalog under gamedata/abc/. MetaHook recursively discovers
    # nested gamedata/**/index.json roots, so the shared abc/ parent stays valid.
    set(output "${PROJECT_BINARY_DIR}/assets/svencoop/metahook/gamedata/abc/${catalog}")
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
        set(destination "${ABC_GAME_DIR}/svencoop/metahook/gamedata/abc/${catalog}")
    else()
        set(destination "svencoop/metahook/gamedata/abc/${catalog}")
    endif()
    install(DIRECTORY "${output}/" DESTINATION "${destination}")
endfunction()
