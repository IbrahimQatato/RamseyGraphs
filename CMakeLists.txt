cmake_minimum_required(VERSION 3.20)
project(RamseyAnalyzer CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ramsey
    ./RamseyMain.cpp
    ./Color.cpp
    ./ExhaustiveSearch.cpp
    ./RandomSampler.cpp
    ./GraphExporter.cpp
    ./options_module/Options.cpp
)

add_executable(playground
    graph_module/Main.cpp
)

add_executable(tests
    ./tests/Tests.cpp
    ./Color.cpp
    ./ExhaustiveSearch.cpp
    ./RandomSampler.cpp
    ./GraphExporter.cpp
)

foreach(target ramsey playground tests)
    target_include_directories(${target} PRIVATE graph_module . options_module)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
endforeach()
