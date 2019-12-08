macro (set_defaults_flags_asm)
    set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -Wall")
    set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g")
    set(CMAKE_ASM_NASM_FLAGS_RELEASE "")

    if (WIN32)
        set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -Xvc")
    endif ()
endmacro ()


macro (set_defaults_flags_cpp)
    if (NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug"
        OR "${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
        message(WARNING "unknown build type '${CMAKE_BUILD_TYPE}'")
    endif ()

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -pedantic -Weverything  \
            -Wno-c++98-compat-pedantic -Wno-ctad-maybe-unsupported -Wno-disabled-macro-expansion  \
            -Wno-double-promotion -Wno-exit-time-destructors -Wno-float-equal -Wno-padded  \
            -Wno-return-std-move-in-c++11 -Wno-sign-conversion -Wno-weak-vtables"
        )
        set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG -Og -g -fno-omit-frame-pointer -fsanitize=address,undefined")
        set(CMAKE_CXX_FLAGS_RELEASE "-D NDEBUG -O3 -flto=thin")

        if (WIN32)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fms-compatibility")
            set(CMAKE_CXX_FLAGS_DEBUG "-D _DEBUG")
            if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                link_libraries("libcmtd.lib" "libcpmtd.lib")
            else ()
                link_libraries("libcmt.lib" "libcpmt.lib")
            endif ()
        endif ()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "-D DEBUG -Og -g -fno-omit-frame-pointer -fsanitize=address,undefined")
        set(CMAKE_CXX_FLAGS_RELEASE "-D NDEBUG -O3 -flto")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /diagnostics:caret /bigobj /MP /permissive- /EHsc /W4")
        set(CMAKE_CXX_FLAGS_DEBUG "/DDEBUG /MTd /GF /Oi /JMC /RTC1 /ZI")
        set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /MT /O2 /GL /Gw")

        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG:FASTLINK")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG:FULL /LTCG:INCREMENTAL")

        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
    
        set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "")
        set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "/LTCG")
    endif ()

    if (WIN32)
        link_libraries("onecore.lib")
    else ()
        link_libraries("pthread" "stdc++fs")
    endif ()

    _set_defaults_flags_cpp_linker()
endmacro ()


function (set_defaults_tools_cpp)
    if ("$ENV{CC}$ENV{CXX}" STREQUAL "")
        find_program(clang_PATH "clang")
        if (clang_PATH)
            set(CMAKE_C_COMPILER "${clang_PATH}" CACHE FILEPATH "")
            set(CMAKE_CXX_COMPILER "${clang_PATH}++" CACHE FILEPATH "")
        else ()
            message(WARNING "could not find clang compiler")

            if (WIN32)
                find_program(cl_PATH "cl")
                if (cl_PATH)
                    set(CMAKE_C_COMPILER "${cl_PATH}" CACHE FILEPATH "")
                    set(CMAKE_CXX_COMPILER "${cl_PATH}" CACHE FILEPATH "")
                else ()
                    message(WARNING "could not find msvc compiler")
                endif ()
            endif ()
        endif ()
    endif ()
endfunction ()


function (_set_defaults_flags_cpp_linker)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        find_program(lld_PATH "ld.lld")
        if (lld_PATH)
            set(ld_flags "-fuse-ld=lld")
            if (NOT WIN32)
                set(ld_flags "${ld_flags} -Wl,-z,notext")
            endif ()
        else ()
            message(WARNING "could not find lld linker")

            find_program(gold_PATH "ld.gold")
            if (gold_PATH)
                set(ld_flags "-fuse-ld=gold")
            else ()
                message(WARNING "could not find gold linker")
            endif ()
        endif ()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        find_program(gold_PATH "ld.gold")
        if (gold_PATH)
            set(ld_flags "-fuse-ld=gold")
        else ()
            message(WARNING "could not find gold linker")
        endif ()
    endif ()

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ld_flags}" PARENT_SCOPE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${ld_flags}" PARENT_SCOPE)
    set(CMAKE_STATIC_LINKER_FLAGS"${CMAKE_STATIC_LINKER_FLAGS} ${ld_flags}" PARENT_SCOPE)
endfunction ()
