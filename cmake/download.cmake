include(ExternalProject)

function (download name url)
    ExternalProject_Add("${name}"
        SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${name}"
        URL "${url}"
        DOWNLOAD_NO_PROGRESS ON
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
    )
    set(${name}_DIR "${CMAKE_CURRENT_BINARY_DIR}/${name}" PARENT_SCOPE)
endfunction ()


function (download_github name repo ref)
    download("${name}" "https://github.com/${repo}/archive/${ref}.zip")
    set(${name}_DIR "${${name}_DIR}" PARENT_SCOPE)
endfunction ()
