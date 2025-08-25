/**
 * @file main.c
 * @brief Главный файл пользовательской оболочки bash
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит функцию main и инициализацию оболочки.
 */

#include "shell.h"
#include "parser.h"
#include "executor.h"
#include "builtins.h"
#include "utils.h"

/**
 * @brief Главная функция программы
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код выхода программы
 */
int main(int argc, char *argv[]) {
    shell_state_t shell_state;
    int exit_code = 0;
    
    // Инициализация оболочки
    if (shell_init(&shell_state) != 0) {
        fprintf(stderr, "Ошибка инициализации оболочки\n");
        return 1;
    }
    
    // Установка обработчика сигналов
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    
    // Основной цикл оболочки
    exit_code = shell_run(&shell_state);
    
    // Очистка ресурсов
    shell_cleanup(&shell_state);
    
    return exit_code;
}
