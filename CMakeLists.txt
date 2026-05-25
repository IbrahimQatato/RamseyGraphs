cmake_minimum_required(VERSION 3.20)
project(RamseyAnalyzer CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ramsey
    ./RamseyMain.cpp
)

add_executable(playground
    graph_module/Main.cpp
)

foreach(target ramsey playground)
    target_include_directories(${target} PRIVATE graph_module)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
endforeach()
