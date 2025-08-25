/**
 * @file shell.c
 * @brief Реализация основных функций оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 */

#include "shell.h"
#include "parser.h"
#include "executor.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Глобальные переменные для обработки сигналов
static volatile sig_atomic_t g_signal_received = 0;
static volatile sig_atomic_t g_signal_number = 0;

// Глобальная переменная для выхода из оболочки
int g_should_exit = 0;

/**
 * @brief Обработчик сигналов
 * @param sig Номер сигнала
 */
void signal_handler(int sig) {
    g_signal_received = 1;
    g_signal_number = sig;
    
    if (sig == SIGINT) {
        printf("\n");
        fflush(stdout);
    }
}

/**
 * @brief Инициализация оболочки
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int shell_init(shell_state_t *state) {
    if (!state) {
        return -1;
    }
    
    // Инициализация состояния
    state->prompt = strdup("custom_shell$ ");
    if (!state->prompt) {
        return -1;
    }
    
    // Получение текущей директории
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        state->current_dir = strdup(cwd);
        if (!state->current_dir) {
            free(state->prompt);
            return -1;
        }
    } else {
        state->current_dir = strdup(".");
        if (!state->current_dir) {
            free(state->prompt);
            return -1;
        }
    }
    
    state->exit_code = 0;
    state->should_exit = 0;
    
    return 0;
}

/**
 * @brief Основной цикл оболочки
 * @param state Указатель на состояние оболочки
 * @return Код выхода оболочки
 */
int shell_run(shell_state_t *state) {
    char input[MAX_INPUT_SIZE];
    command_t commands[MAX_ARGS];
    
    printf("Добро пожаловать в Custom Shell!\n");
    printf("Введите 'help' для получения справки, 'exit' для выхода.\n\n");
    
    while (!state->should_exit) {
        // Обновление текущей директории
        if (getcwd(state->current_dir, MAX_PATH) == NULL) {
            strcpy(state->current_dir, ".");
        }
        
        // Вывод приглашения
        printf("%s", state->prompt);
        fflush(stdout);
        
        // Чтение ввода
        if (!fgets(input, sizeof(input), stdin)) {
            if (feof(stdin)) {
                printf("\n");
                break;
            }
            continue;
        }
        
        // Удаление символа новой строки
        input[strcspn(input, "\n")] = 0;
        
        // Пропуск пустых строк
        if (strlen(input) == 0) {
            continue;
        }
        
        // Разбор ввода
        int cmd_count = parse_input(input, commands, MAX_ARGS);
        if (cmd_count <= 0) {
            continue;
        }
        
        // Выполнение команд
        for (int i = 0; i < cmd_count; i++) {
            if (commands[i].name) {
                state->exit_code = execute_command(&commands[i]);
                if (state->should_exit) {
                    break;
                }
            }
        }
        
        // Очистка команд
        free_commands(commands, cmd_count);
        
        // Проверка сигналов
        if (g_signal_received) {
            g_signal_received = 0;
            continue;
        }
        
        // Проверка глобального флага выхода
        if (g_should_exit) {
            state->should_exit = 1;
            break;
        }
    }
    
    return state->exit_code;
}

/**
 * @brief Очистка ресурсов оболочки
 * @param state Указатель на состояние оболочки
 */
void shell_cleanup(shell_state_t *state) {
    if (state) {
        if (state->prompt) {
            free(state->prompt);
        }
        if (state->current_dir) {
            free(state->current_dir);
        }
    }
}
