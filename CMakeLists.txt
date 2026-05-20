cmake_minimum_required(VERSION 3.15)

project(tally
    VERSION 0.1.0
    DESCRIPTION "A minimal line/word/character counter"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(tally
    src/main.c
    src/tally.c
)

target_include_directories(tally PRIVATE include)

target_link_libraries(tally PRIVATE m)

target_compile_options(tally PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
)

install(TARGETS tally RUNTIME DESTINATION bin)
