/**
 * @file parser.c
 * @brief Реализация парсера команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 */

#include "parser.h"
#include "builtins.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Разбор входной строки на команды
 * @param input Входная строка для разбора
 * @param commands Массив команд для заполнения
 * @param max_commands Максимальное количество команд
 * @return Количество найденных команд
 */
int parse_input(const char *input, command_t *commands, int max_commands) {
    if (!input || !commands || max_commands <= 0) {
        return 0;
    }
    
    // Разделение по точкам с запятой (для множественных команд)
    char **cmd_strings = NULL;
    int cmd_count = 0;
    
    cmd_strings = split_string(input, ";", &cmd_count);
    if (!cmd_strings) {
        return 0;
    }
    
    // Ограничение количества команд
    if (cmd_count > max_commands) {
        cmd_count = max_commands;
    }
    
    // Разбор каждой команды
    int parsed_count = 0;
    for (int i = 0; i < cmd_count && parsed_count < max_commands; i++) {
        if (parse_command(cmd_strings[i], &commands[parsed_count]) == 0) {
            parsed_count++;
        }
    }
    
    // Очистка временных строк
    free_string_array(cmd_strings, cmd_count);
    
    return parsed_count;
}

/**
 * @brief Разбор одной команды
 * @param cmd_str Строка команды
 * @param cmd Указатель на структуру команды для заполнения
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int parse_command(const char *cmd_str, command_t *cmd) {
    if (!cmd_str || !cmd) {
        return -1;
    }
    
    // Инициализация структуры команды
    memset(cmd, 0, sizeof(command_t));
    
    // Удаление пробелов в начале и конце
    char *trimmed = trim_string(strdup(cmd_str));
    if (!trimmed) {
        return -1;
    }
    
    // Проверка на пустую команду
    if (strlen(trimmed) == 0) {
        free(trimmed);
        return -1;
    }
    
    // Разбор перенаправлений
    char *input_redir = strstr(trimmed, "<");
    char *output_redir = strstr(trimmed, ">");
    char *background = strstr(trimmed, "&");
    
    // Установка флагов
    if (background) {
        cmd->background = 1;
        *background = '\0';
    }
    
    // Обработка перенаправления ввода
    if (input_redir) {
        *input_redir = '\0';
        input_redir++;
        while (isspace(*input_redir)) input_redir++;
        cmd->input_file = strdup(input_redir);
    }
    
    // Обработка перенаправления вывода
    if (output_redir) {
        *output_redir = '\0';
        output_redir++;
        while (isspace(*output_redir)) output_redir++;
        cmd->output_file = strdup(output_redir);
    }
    
    // Разбор аргументов
    cmd->argc = parse_arguments(trimmed, &cmd->args, MAX_ARGS);
    if (cmd->argc > 0) {
        cmd->name = strdup(cmd->args[0]);
    }
    
    free(trimmed);
    return 0;
}

/**
 * @brief Разбор аргументов команды
 * @param args_str Строка с аргументами
 * @param args Массив для аргументов
 * @param max_args Максимальное количество аргументов
 * @return Количество аргументов
 */
int parse_arguments(const char *args_str, char ***args, int max_args) {
    if (!args_str || !args || max_args <= 0) {
        return 0;
    }
    
    // Разделение по пробелам
    char **parts = NULL;
    int part_count = 0;
    
    parts = split_string(args_str, " \t", &part_count);
    if (!parts) {
        return 0;
    }
    
    // Выделение памяти для массива аргументов
    *args = malloc((part_count + 1) * sizeof(char *));
    if (!*args) {
        free_string_array(parts, part_count);
        return 0;
    }
    
    // Копирование аргументов
    int arg_count = 0;
    for (int i = 0; i < part_count && arg_count < max_args; i++) {
        if (strlen(parts[i]) > 0) {
            (*args)[arg_count] = strdup(parts[i]);
            arg_count++;
        }
    }
    
    (*args)[arg_count] = NULL; // Завершающий NULL
    
    // Очистка временных частей
    free_string_array(parts, part_count);
    
    return arg_count;
}

/**
 * @brief Очистка структуры команды
 * @param cmd Указатель на структуру команды
 */
void free_command(command_t *cmd) {
    if (!cmd) {
        return;
    }
    
    if (cmd->name) {
        free(cmd->name);
    }
    
    if (cmd->args) {
        for (int i = 0; i < cmd->argc; i++) {
            if (cmd->args[i]) {
                free(cmd->args[i]);
            }
        }
        free(cmd->args);
    }
    
    if (cmd->input_file) {
        free(cmd->input_file);
    }
    
    if (cmd->output_file) {
        free(cmd->output_file);
    }
    
    memset(cmd, 0, sizeof(command_t));
}

/**
 * @brief Очистка массива команд
 * @param commands Массив команд
 * @param count Количество команд
 */
void free_commands(command_t *commands, int count) {
    if (!commands || count <= 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        free_command(&commands[i]);
    }
}

/**
 * @brief Проверка на встроенную команду
 * @param cmd_name Имя команды
 * @return 1 если встроенная, 0 если внешняя
 */
int is_builtin(const char *cmd_name) {
    if (!cmd_name) {
        return 0;
    }
    
    const char *builtins[] = {
        "cd", "pwd", "echo", "exit", "help", "clear", "history"
    };
    
    int builtin_count = sizeof(builtins) / sizeof(builtins[0]);
    
    for (int i = 0; i < builtin_count; i++) {
        if (strcmp(cmd_name, builtins[i]) == 0) {
            return 1;
        }
    }
    
    return 0;
}
