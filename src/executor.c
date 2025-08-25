/**
 * @file executor.c
 * @brief Реализация исполнителя команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 */

#include "executor.h"
#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

// Глобальные переменные для перенаправления
static int original_stdin = -1;
static int original_stdout = -1;

/**
 * @brief Выполнение команды
 * @param cmd Команда для выполнения
 * @return Код выхода команды
 */
int execute_command(command_t *cmd) {
    if (!cmd || !cmd->name) {
        return -1;
    }
    
    // Настройка перенаправлений
    if (setup_redirections(cmd) != 0) {
        return -1;
    }
    
    int exit_code = 0;
    
    // Проверка на встроенную команду
    if (is_builtin(cmd->name)) {
        exit_code = execute_builtin(cmd);
    } else {
        exit_code = execute_external(cmd);
    }
    
    // Восстановление стандартного ввода/вывода
    restore_stdio();
    
    return exit_code;
}

/**
 * @brief Выполнение внешней программы
 * @param cmd Команда для выполнения
 * @return Код выхода программы
 */
int execute_external(command_t *cmd) {
    if (!cmd || !cmd->name) {
        return -1;
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Ошибка создания процесса");
        return -1;
    } else if (pid == 0) {
        // Дочерний процесс
        
        // Выполнение команды
        if (execvp(cmd->name, cmd->args) == -1) {
            perror("Ошибка выполнения команды");
            exit(EXIT_FAILURE);
        }
    } else {
        // Родительский процесс
        
        if (cmd->background) {
            // Фоновое выполнение
            printf("[%d] %s\n", pid, cmd->name);
            return 0;
        } else {
            // Ожидание завершения
            int status;
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                printf("Процесс %d завершен сигналом %d\n", pid, WTERMSIG(status));
                return -1;
            }
        }
    }
    
    return 0;
}

/**
 * @brief Выполнение встроенной команды
 * @param cmd Команда для выполнения
 * @return Код выхода команды
 */
int execute_builtin(command_t *cmd) {
    if (!cmd || !cmd->name) {
        return -1;
    }
    
    if (strcmp(cmd->name, "cd") == 0) {
        return builtin_cd(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "pwd") == 0) {
        return builtin_pwd(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "echo") == 0) {
        return builtin_echo(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "exit") == 0) {
        return builtin_exit(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "help") == 0) {
        return builtin_help(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "clear") == 0) {
        return builtin_clear(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "history") == 0) {
        return builtin_history(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "touch") == 0) {
        return builtin_touch(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "rm") == 0) {
        return builtin_rm(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "mkdir") == 0) {
        return builtin_mkdir(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "rmdir") == 0) {
        return builtin_rmdir(cmd->args, cmd->argc);
    } else if (strcmp(cmd->name, "ls") == 0) {
        return builtin_ls(cmd->args, cmd->argc);
    }
    
    return -1;
}

/**
 * @brief Настройка перенаправления ввода/вывода
 * @param cmd Команда с настройками перенаправления
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int setup_redirections(command_t *cmd) {
    if (!cmd) {
        return -1;
    }
    
    // Сохранение оригинальных дескрипторов
    if (original_stdin == -1) {
        original_stdin = dup(STDIN_FILENO);
    }
    if (original_stdout == -1) {
        original_stdout = dup(STDOUT_FILENO);
    }
    
    // Перенаправление ввода
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1) {
            perror("Ошибка открытия файла ввода");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("Ошибка перенаправления ввода");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    // Перенаправление вывода
    if (cmd->output_file) {
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("Ошибка открытия файла вывода");
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Ошибка перенаправления вывода");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    return 0;
}

/**
 * @brief Восстановление стандартного ввода/вывода
 */
void restore_stdio(void) {
    if (original_stdin != -1) {
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);
        original_stdin = -1;
    }
    
    if (original_stdout != -1) {
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        original_stdout = -1;
    }
}

/**
 * @brief Ожидание завершения фоновых процессов
 */
void wait_for_background(void) {
    int status;
    pid_t pid;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("[%d] Завершен с кодом %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[%d] Завершен сигналом %d\n", pid, WTERMSIG(status));
        }
    }
}

/**
 * @brief Проверка статуса фоновых процессов
 */
void check_background_status(void) {
    wait_for_background();
}
