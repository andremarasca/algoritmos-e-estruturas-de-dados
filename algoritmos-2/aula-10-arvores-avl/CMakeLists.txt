cmake_minimum_required(VERSION 3.16)
project(aula_10_arvores_avl LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(aula_10 avl_tree.c)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(aula_10 PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror)
elseif(MSVC)
    target_compile_options(aula_10 PRIVATE /W4 /WX /utf-8)
endif()

enable_testing()
add_test(NAME demonstracao COMMAND aula_10)
