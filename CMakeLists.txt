cmake_minimum_required(VERSION 3.10)

project(MaximalSand LANGUAGES CXX)

option(SINGLETHREAD "Build a single threaded binary" OFF)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(MaximalSand ${SOURCES})
if(SINGLETHREAD)
    if(EMSCRIPTEN)
        set_target_properties(MaximalSand PROPERTIES
            OUTPUT_NAME "MaximalSand_st"
        )
    endif()
    add_compile_definitions(SINGLETHREAD)
else()
    if(EMSCRIPTEN)
        target_compile_options(MaximalSand PRIVATE -sUSE_PTHREADS=1 -msimd128)
        target_link_options(MaximalSand PRIVATE -sUSE_PTHREADS=1 -sPTHREAD_POOL_SIZE=5)
    endif()
endif()


set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto"
)
if(EMSCRIPTEN)
    target_compile_options(MaximalSand PRIVATE
        -sUSE_SDL=2
    )

    target_link_options(MaximalSand PRIVATE
        -sUSE_SDL=2
        -sUSE_WEBGL2=1
        -sWASM=1
        -sALLOW_MEMORY_GROWTH
        -sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR
    )
    set_target_properties(MaximalSand PROPERTIES
        SUFFIX ".js"
    )
else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(MaximalSand PRIVATE SDL2::SDL2)
    target_include_directories(MaximalSand PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(MaximalSand PRIVATE ${SDL2_LIBRARIES})
endif()
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g"
)
