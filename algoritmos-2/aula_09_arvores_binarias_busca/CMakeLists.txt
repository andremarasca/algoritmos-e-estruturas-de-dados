cmake_minimum_required(VERSION 3.16)
project(aula_09_arvores_binarias_busca LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(aula_09 main.c ArvoreBinaria.c)
add_executable(test_arvore_binaria tests/test_arvore_binaria.c)

foreach(target aula_09 test_arvore_binaria)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -Werror)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX /utf-8)
    endif()
endforeach()

enable_testing()
add_test(NAME arvore_binaria COMMAND test_arvore_binaria)
add_test(NAME demonstracao COMMAND aula_09)
