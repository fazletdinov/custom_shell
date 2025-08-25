/**
 * @file builtins.c
 * @brief Реализация встроенных команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 */

#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

// Глобальная переменная для выхода из оболочки
extern int g_should_exit;

/**
 * @brief Встроенная команда cd (смена директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_cd(char **args, int argc) {
    const char *target_dir = NULL;
    
    if (argc == 1) {
        // cd без аргументов - переход в домашнюю директорию
        target_dir = getenv("HOME");
        if (!target_dir) {
            fprintf(stderr, "cd: переменная HOME не установлена\n");
            return -1;
        }
    } else if (argc == 2) {
        target_dir = args[1];
    } else {
        fprintf(stderr, "cd: слишком много аргументов\n");
        return -1;
    }
    
    if (chdir(target_dir) != 0) {
        perror("cd");
        return -1;
    }
    
    return 0;
}

/**
 * @brief Встроенная команда pwd (текущая директория)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_pwd(char **args, int argc) {
    (void)args; // Неиспользуемый параметр
    (void)argc; // Неиспользуемый параметр
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    } else {
        perror("pwd");
        return -1;
    }
}

/**
 * @brief Встроенная команда echo (вывод текста)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_echo(char **args, int argc) {
    for (int i = 1; i < argc; i++) {
        printf("%s", args[i]);
        if (i < argc - 1) {
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

/**
 * @brief Встроенная команда exit (выход из оболочки)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return Код выхода
 */
int builtin_exit(char **args, int argc) {
    int exit_code = 0;
    
    if (argc > 1) {
        exit_code = atoi(args[1]);
    }
    
    // Установка флага для выхода из основного цикла
    g_should_exit = 1;
    
    return exit_code;
}

/**
 * @brief Встроенная команда help (справка)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_help(char **args, int argc) {
    (void)args; // Неиспользуемый параметр
    (void)argc; // Неиспользуемый параметр
    
    printf("Custom Shell - Встроенные команды:\n");
    printf("  cd [директория]     - смена директории\n");
    printf("  pwd                 - показать текущую директорию\n");
    printf("  echo [текст]        - вывести текст\n");
    printf("  exit [код]          - выход из оболочки\n");
    printf("  help                - показать эту справку\n");
    printf("  clear               - очистить экран\n");
    printf("  history             - показать историю команд\n");
    printf("\n");
    printf("Также поддерживаются внешние команды системы.\n");
    printf("Используйте Ctrl+C для прерывания команд.\n");
    
    return 0;
}

/**
 * @brief Встроенная команда clear (очистка экрана)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_clear(char **args, int argc) {
    (void)args; // Неиспользуемый параметр
    (void)argc; // Неиспользуемый параметр
    
    // ANSI escape sequence для очистки экрана
    printf("\033[2J\033[H");
    fflush(stdout);
    
    return 0;
}

/**
 * @brief Встроенная команда history (история команд)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_history(char **args, int argc) {
    (void)args; // Неиспользуемый параметр
    (void)argc; // Неиспользуемый параметр
    
    printf("История команд пока не реализована.\n");
    printf("Эта функция будет добавлена в следующих версиях.\n");
    
    return 0;
}
