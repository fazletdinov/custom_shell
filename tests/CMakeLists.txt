# CMakeLists.txt для тестов

# Поиск библиотеки для тестирования
find_package(GTest QUIET)

if(GTest_FOUND)
    # Создание исполняемого файла для тестов
    add_executable(shell_tests
        test_shell.c
        test_parser.c
        test_utils.c
    )
    
    # Включение директорий
    target_include_directories(shell_tests PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
    )
    
    # Линковка с библиотеками
    target_link_libraries(shell_tests
        ${GTEST_LIBRARIES}
        pthread
    )
    
    # Добавление тестов
    add_test(NAME ShellTests COMMAND shell_tests)
    
    # Включение тестов в основную цель
    add_dependencies(shell_tests custom_shell)
else()
    message(STATUS "Google Test не найден, тесты не будут собраны")
endif()
