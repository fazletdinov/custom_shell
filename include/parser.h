/**
 * @file parser.h
 * @brief Заголовочный файл для парсера команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит функции для разбора входных команд пользователя,
 * включая парсинг аргументов, перенаправления ввода/вывода и фонового выполнения.
 */

#ifndef PARSER_H
#define PARSER_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Разбор входной строки на команды
 * @param input Входная строка для разбора
 * @param commands Массив команд для заполнения
 * @param max_commands Максимальное количество команд
 * @return Количество найденных команд
 */
int parse_input(const char *input, command_t *commands, int max_commands);

/**
 * @brief Разбор одной команды
 * @param cmd_str Строка команды
 * @param cmd Указатель на структуру команды для заполнения
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int parse_command(const char *cmd_str, command_t *cmd);

/**
 * @brief Разбор аргументов команды
 * @param args_str Строка с аргументами
 * @param args Массив для аргументов
 * @param max_args Максимальное количество аргументов
 * @return Количество аргументов
 */
int parse_arguments(const char *args_str, char ***args, int max_args);

/**
 * @brief Очистка структуры команды
 * @param cmd Указатель на структуру команды
 */
void free_command(command_t *cmd);

/**
 * @brief Очистка массива команд
 * @param commands Массив команд
 * @param count Количество команд
 */
void free_commands(command_t *commands, int count);

/**
 * @brief Проверка на встроенную команду
 * @param cmd_name Имя команды
 * @return 1 если встроенная, 0 если внешняя
 */
int is_builtin(const char *cmd_name);

#ifdef __cplusplus
}
#endif

#endif /* PARSER_H */
