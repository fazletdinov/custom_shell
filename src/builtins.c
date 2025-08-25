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
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

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
            fprintf(stderr, "\033[31mcd: переменная HOME не установлена\033[0m\n");
            return -1;
        }
    } else if (argc == 2) {
        target_dir = args[1];
    } else {
        fprintf(stderr, "\033[31mcd: слишком много аргументов\033[0m\n");
        return -1;
    }
    
    if (chdir(target_dir) != 0) {
        fprintf(stderr, "\033[31mcd: %s\033[0m\n", strerror(errno));
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
    
    // Проверяем поддержку цветов
    extern int supports_colors(void);
    
    printf("Custom Shell - Встроенные команды:\n");
    printf("  cd [директория]     - смена директории\n");
    printf("  pwd                 - показать текущую директорию\n");
    printf("  echo [текст]        - вывести текст\n");
    printf("  exit [код]          - выход из оболочки\n");
    printf("  help                - показать эту справку\n");
    printf("  clear               - очистить экран\n");
    printf("  history             - показать историю команд\n");
    printf("  touch <файл>        - создать файл\n");
    printf("  rm <файл>           - удалить файл\n");
    printf("  mkdir <директория>  - создать директорию\n");
    printf("  rmdir <директория>  - удалить директорию\n");
    printf("  ls [директория]     - показать содержимое директории\n");
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
    
    // Получаем указатель на состояние оболочки (глобальная переменная)
    extern shell_state_t *g_shell_state;
    
    if (!g_shell_state) {
        printf("История команд недоступна.\n");
        return -1;
    }
    
    if (g_shell_state->history_count == 0) {
        printf("История команд пуста.\n");
        return 0;
    }
    
    printf("История команд (%d записей):\n", g_shell_state->history_count);
    printf("%-4s %-20s %-10s %s\n", "№", "Время", "Код", "Команда");
    printf("---- -------------------- ---------- ------------------------\n");
    
    for (int i = 0; i < g_shell_state->history_count; i++) {
        char time_str[20];
        struct tm *tm_info = localtime(&g_shell_state->history[i].timestamp);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("%-4d %-20s %-10d %s\n", 
               i + 1, 
               time_str, 
               g_shell_state->history[i].exit_code,
               g_shell_state->history[i].command);
    }
    
    printf("\nИспользование истории:\n");
    printf("  !5        - выполнить команду №5 из истории\n");
    printf("  !ls       - выполнить последнюю команду, начинающуюся с 'ls'\n");
    printf("  history   - показать эту справку\n");
    
    return 0;
}

/**
 * @brief Встроенная команда touch (создание файла)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_touch(char **args, int argc) {
    if (argc < 2) {
        fprintf(stderr, "touch: требуется указать имя файла\n");
        fprintf(stderr, "Использование: touch <файл> [файл2] ...\n");
        return -1;
    }
    
    int success_count = 0;
    for (int i = 1; i < argc; i++) {
        FILE *file = fopen(args[i], "a");
        if (file) {
            fclose(file);
            success_count++;
        } else {
            fprintf(stderr, "touch: не удалось создать файл '%s': %s\n", 
                    args[i], strerror(errno));
        }
    }
    
    if (success_count == argc - 1) {
        return 0;
    } else if (success_count > 0) {
        return 1; // Частичный успех
    } else {
        return -1; // Полная неудача
    }
}

/**
 * @brief Встроенная команда rm (удаление файла)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_rm(char **args, int argc) {
    if (argc < 2) {
        fprintf(stderr, "rm: требуется указать имя файла\n");
        fprintf(stderr, "Использование: rm <файл> [файл2] ...\n");
        return -1;
    }
    
    int success_count = 0;
    for (int i = 1; i < argc; i++) {
        if (unlink(args[i]) == 0) {
            success_count++;
        } else {
            fprintf(stderr, "rm: не удалось удалить файл '%s': %s\n", 
                    args[i], strerror(errno));
        }
    }
    
    if (success_count == argc - 1) {
        return 0;
    } else if (success_count > 0) {
        return 1; // Частичный успех
    } else {
        return -1; // Полная неудача
    }
}

/**
 * @brief Встроенная команда mkdir (создание директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_mkdir(char **args, int argc) {
    if (argc < 2) {
        fprintf(stderr, "mkdir: требуется указать имя директории\n");
        fprintf(stderr, "Использование: mkdir <директория> [директория2] ...\n");
        return -1;
    }
    
    int success_count = 0;
    for (int i = 1; i < argc; i++) {
        if (mkdir(args[i], 0755) == 0) {
            success_count++;
        } else {
            fprintf(stderr, "mkdir: не удалось создать директорию '%s': %s\n", 
                    args[i], strerror(errno));
        }
    }
    
    if (success_count == argc - 1) {
        return 0;
    } else if (success_count > 0) {
        return 1; // Частичный успех
    } else {
        return -1; // Полная неудача
    }
}

/**
 * @brief Встроенная команда rmdir (удаление директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_rmdir(char **args, int argc) {
    if (argc < 2) {
        fprintf(stderr, "rmdir: требуется указать имя директории\n");
        fprintf(stderr, "Использование: rmdir <директория> [директория2] ...\n");
        return -1;
    }
    
    int success_count = 0;
    for (int i = 1; i < argc; i++) {
        if (rmdir(args[i]) == 0) {
            success_count++;
        } else {
            fprintf(stderr, "rmdir: не удалось удалить директорию '%s': %s\n", 
                    args[i], strerror(errno));
        }
    }
    
    if (success_count == argc - 1) {
        return 0;
    } else if (success_count > 0) {
        return 1; // Частичный успех
    } else {
        return -1; // Полная неудача
    }
}

/**
 * @brief Встроенная команда ls (просмотр содержимого директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_ls(char **args, int argc) {
    const char *dir_path = ".";
    
    if (argc > 1) {
        dir_path = args[1];
    }
    
    // Проверяем поддержку цветов
    extern int supports_colors(void);
    
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "ls: не удалось открыть директорию '%s': %s\n", 
                dir_path, strerror(errno));
        return -1;
    }
    
    printf("Содержимое директории '%s':\n", dir_path);
    printf("%-20s %-10s %-8s %s\n", "Имя", "Размер", "Права", "Тип");
    printf("-------------------- ---------- -------- ------------------------\n");
    
    struct dirent *entry;
    int file_count = 0;
    int dir_count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0) {
            // Определение типа файла и цвета
            const char *type = "файл";
            const char *color = "\033[37m"; // Белый для обычных файлов
            
            if (S_ISDIR(st.st_mode)) {
                type = "директория";
                color = "\033[34m"; // Синий для директорий
                dir_count++;
            } else if (S_ISLNK(st.st_mode)) {
                type = "ссылка";
                color = "\033[36m"; // Голубой для ссылок
            } else if (S_ISFIFO(st.st_mode)) {
                type = "канал";
                color = "\033[35m"; // Пурпурный для каналов
            } else if (S_ISSOCK(st.st_mode)) {
                type = "сокет";
                color = "\033[33m"; // Желтый для сокетов
            } else {
                file_count++;
            }
            
            // Форматирование прав доступа
            char perms[10];
            snprintf(perms, sizeof(perms), "%c%c%c%c%c%c%c%c%c",
                    (st.st_mode & S_IRUSR) ? 'r' : '-',
                    (st.st_mode & S_IWUSR) ? 'w' : '-',
                    (st.st_mode & S_IXUSR) ? 'x' : '-',
                    (st.st_mode & S_IRGRP) ? 'r' : '-',
                    (st.st_mode & S_IWGRP) ? 'w' : '-',
                    (st.st_mode & S_IXGRP) ? 'x' : '-',
                    (st.st_mode & S_IROTH) ? 'r' : '-',
                    (st.st_mode & S_IWOTH) ? 'w' : '-',
                    (st.st_mode & S_IXOTH) ? 'x' : '-');
            
            // Цветной вывод
            if (supports_colors()) {
                printf("%s%-20s\033[0m %-10ld %-8s %s\n", 
                       color, entry->d_name, 
                       (long)st.st_size, 
                       perms, 
                       type);
            } else {
                printf("%-20s %-10ld %-8s %s\n", 
                       entry->d_name, 
                       (long)st.st_size, 
                       perms, 
                       type);
            }
        }
    }
    
    closedir(dir);
    
    printf("\nИтого: %d файлов, %d директорий\n", file_count, dir_count);
    return 0;
}
