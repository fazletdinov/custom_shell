cmake_minimum_required(VERSION 3.16)
project(custom_shell VERSION 1.0.0 LANGUAGES C)

# Установка стандарта C
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Опции сборки
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_DOXYGEN "Enable Doxygen documentation" ON)

# Поиск Doxygen
if(ENABLE_DOXYGEN)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    endif()
endif()

# Исходные файлы
set(SOURCES
    src/main.c
    src/shell.c
    src/parser.c
    src/executor.c
    src/builtins.c
    src/utils.c
)

set(HEADERS
    include/shell.h
    include/parser.h
    include/executor.h
    include/builtins.h
    include/utils.h
)

# Создание исполняемого файла
add_executable(custom_shell ${SOURCES} ${HEADERS})

# Включение директорий
target_include_directories(custom_shell PRIVATE include)

# Установка
install(TARGETS custom_shell DESTINATION bin)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
