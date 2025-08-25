/**
 * @file executor.h
 * @brief Заголовочный файл для исполнителя команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит функции для выполнения команд, включая
 * создание процессов, перенаправление ввода/вывода и управление сигналами.
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "shell.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Выполнение команды
 * @param cmd Команда для выполнения
 * @return Код выхода команды
 */
int execute_command(command_t *cmd);

/**
 * @brief Выполнение внешней программы
 * @param cmd Команда для выполнения
 * @return Код выхода программы
 */
int execute_external(command_t *cmd);

/**
 * @brief Выполнение встроенной команды
 * @param cmd Команда для выполнения
 * @return Код выхода команды
 */
int execute_builtin(command_t *cmd);

/**
 * @brief Настройка перенаправления ввода/вывода
 * @param cmd Команда с настройками перенаправления
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int setup_redirections(command_t *cmd);

/**
 * @brief Восстановление стандартного ввода/вывода
 */
void restore_stdio(void);

/**
 * @brief Ожидание завершения фоновых процессов
 */
void wait_for_background(void);

/**
 * @brief Проверка статуса фоновых процессов
 */
void check_background_status(void);

#ifdef __cplusplus
}
#endif

#endif /* EXECUTOR_H */
