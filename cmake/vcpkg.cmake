function (vcpkg_install)
    cmake_parse_arguments(ARG "" "" "LIBS" ${ARGN})
    if (UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "unparsed arguments")
    endif ()

    find_package("Python3" REQUIRED "Interpreter")
    execute_process(COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/cmake/vcpkg.py" "" ${ARG_LIBS}
        RESULT_VARIABLE exit_code)
    if (exit_code)
        message(FATAL_ERROR "could not install vcpkg packages")
    endif ()
endfunction ()


function (vcpkg_setup vcpkg_ref)
    find_package("Python3" REQUIRED "Interpreter")
    execute_process(COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/cmake/vcpkg.py" "${vcpkg_ref}"
        RESULT_VARIABLE exit_code)
    if (exit_code)
        message(FATAL_ERROR "could not setup vcpkg")
    endif ()

    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")
endfunction ()
