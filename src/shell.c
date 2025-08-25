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
#include <time.h>

// Глобальные переменные для обработки сигналов
static volatile sig_atomic_t g_signal_received = 0;
static volatile sig_atomic_t g_signal_number = 0;

// Глобальная переменная для выхода из оболочки
int g_should_exit = 0;

// Глобальная переменная для доступа к состоянию оболочки
shell_state_t *g_shell_state = NULL;

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
    
    // Получаем имя пользователя и хоста
    char *username = getenv("USER");
    if (!username) {
        strcpy(username, "user");
    }
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "localhost");
    }
    
    // Получение текущей директории
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        state->current_dir = strdup(cwd);
        if (!state->current_dir) {
            return -1;
        }
    } else {
        state->current_dir = strdup(".");
        if (!state->current_dir) {
            return -1;
        }
    }
    
    // Создание цветного приглашения
    state->prompt = create_colored_prompt(username, hostname, state->current_dir);
    if (!state->prompt) {
        // Если не удалось создать цветное, используем простое
        state->prompt = strdup("custom_shell$ ");
        if (!state->prompt) {
            free(state->current_dir);
            return -1;
        }
    }
    
    state->exit_code = 0;
    state->should_exit = 0;
    
    // Инициализация истории команд
    state->history_count = 0;
    state->history_index = 0;
    memset(state->history, 0, sizeof(state->history));
    
    // Загружаем историю команд из файла
    load_history_from_file(state);
    
    // Устанавливаем глобальную переменную
    g_shell_state = state;
    
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
        
        // Обновляем приглашение с новой директорией
        const char *username = getenv("USER") ? getenv("USER") : "user";
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0) {
            strcpy(hostname, "localhost");
        }
        
        // Освобождаем старое приглашение
        if (state->prompt) {
            free(state->prompt);
        }
        
        // Создаем новое цветное приглашение
        state->prompt = create_colored_prompt(username, hostname, state->current_dir);
        if (!state->prompt) {
            state->prompt = strdup("custom_shell$ ");
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
        
        // Обработка расширения истории команд
        char expanded_input[MAX_INPUT_SIZE];
        if (process_history_expansion(input, expanded_input, sizeof(expanded_input)) == 0) {
            // Если есть изменения, показываем расширенную команду
            if (strcmp(input, expanded_input) != 0) {
                printf("Выполняется: %s\n", expanded_input);
            }
            strcpy(input, expanded_input);
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
                // Добавляем команду в историю
                add_to_history(state, input, state->exit_code);
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
        // Сохраняем историю при выходе
        save_history_to_file(state);
    }
}

/**
 * @brief Добавление команды в историю
 * @param state Указатель на состояние оболочки
 * @param command Текст команды
 * @param exit_code Код выхода команды
 */
void add_to_history(shell_state_t *state, const char *command, int exit_code) {
    if (!state || !command || strlen(command) == 0) {
        return;
    }
    
    // Сдвигаем историю, если она переполнена
    if (state->history_count >= MAX_HISTORY_SIZE) {
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++) {
            state->history[i] = state->history[i + 1];
        }
        state->history_count = MAX_HISTORY_SIZE - 1;
    }
    
    // Добавляем новую команду в конец
    int index = state->history_count;
    strncpy(state->history[index].command, command, MAX_HISTORY_LENGTH - 1);
    state->history[index].command[MAX_HISTORY_LENGTH - 1] = '\0';
    state->history[index].timestamp = time(NULL);
    state->history[index].exit_code = exit_code;
    
    state->history_count++;
    state->history_index = state->history_count;
}

/**
 * @brief Получение команды из истории по индексу
 * @param state Указатель на состояние оболочки
 * @param index Индекс команды в истории
 * @return Указатель на команду или NULL если индекс неверный
 */
const char *get_history_command(shell_state_t *state, int index) {
    if (!state || index < 0 || index >= state->history_count) {
        return NULL;
    }
    return state->history[index].command;
}

/**
 * @brief Поиск команды в истории по началу строки
 * @param state Указатель на состояние оболочки
 * @param prefix Префикс для поиска
 * @return Индекс найденной команды или -1 если не найдено
 */
int search_history(shell_state_t *state, const char *prefix) {
    if (!state || !prefix) {
        return -1;
    }
    
    for (int i = state->history_count - 1; i >= 0; i--) {
        if (strncmp(state->history[i].command, prefix, strlen(prefix)) == 0) {
            return i;
        }
    }
    
    return -1;
}

/**
 * @brief Очистка истории команд
 * @param state Указатель на состояние оболочки
 */
void clear_history(shell_state_t *state) {
    if (!state) {
        return;
    }
    
    state->history_count = 0;
    state->history_index = 0;
    memset(state->history, 0, sizeof(state->history));
}

/**
 * @brief Проверка поддержки цветов терминалом
 * @return 1 если цвета поддерживаются, 0 если нет
 */
int supports_colors(void) {
    const char *term = getenv("TERM");
    if (!term) {
        return 0;
    }
    
    // Проверяем, что это цветной терминал
    return (strstr(term, "xterm") != NULL || 
            strstr(term, "linux") != NULL || 
            strstr(term, "vt100") != NULL ||
            strstr(term, "color") != NULL);
}

/**
 * @brief Установка цвета для вывода
 * @param color ANSI escape-код цвета
 */
void set_color(const char *color) {
    if (supports_colors()) {
        printf("%s", color);
        fflush(stdout);
    }
}

/**
 * @brief Сброс цвета вывода
 */
void reset_color(void) {
    if (supports_colors()) {
        printf("%s", COLOR_RESET);
        fflush(stdout);
    }
}

/**
 * @brief Создание цветной строки приглашения
 * @param username Имя пользователя
 * @param hostname Имя хоста
 * @param current_dir Текущая директория
 * @return Цветная строка приглашения
 */
char *create_colored_prompt(const char *username, const char *hostname, const char *current_dir) {
    if (!supports_colors()) {
        // Если цвета не поддерживаются, возвращаем простую строку
        char *simple_prompt = malloc(256);
        if (simple_prompt) {
            snprintf(simple_prompt, 256, "%s@%s:%s$ ", username, hostname, current_dir);
        }
        return simple_prompt;
    }
    
    // Создаем цветную строку
    char *colored_prompt = malloc(512);
    if (!colored_prompt) {
        return NULL;
    }
    
    // Формат: username@hostname:directory$
    snprintf(colored_prompt, 512, 
             "%s%s%s@%s%s:%s%s%s$ %s",
             COLOR_BOLD, COLOR_GREEN, username,           // username (жирный зеленый)
             COLOR_BLUE, hostname,                        // @hostname (синий)
             COLOR_YELLOW, current_dir,                   // :directory (желтый)
             COLOR_RESET,                                 // сброс цвета
             COLOR_RESET);                                // дополнительный сброс для безопасности
    
    return colored_prompt;
}

/**
 * @brief Получение пути к файлу истории
 * @return Указатель на строку с путем или NULL в случае ошибки
 */
char *get_history_file_path(void) {
    const char *home = getenv("HOME");
    if (!home) {
        return NULL;
    }
    
    size_t path_len = strlen(home) + strlen(HISTORY_FILE_NAME) + 1;
    char *path = malloc(path_len);
    if (!path) {
        return NULL;
    }
    
    strcpy(path, home);
    strcat(path, HISTORY_FILE_NAME);
    return path;
}

/**
 * @brief Загрузка истории команд из файла
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int load_history_from_file(shell_state_t *state) {
    if (!state) {
        return -1;
    }
    
    char *history_file = get_history_file_path();
    if (!history_file) {
        return -1;
    }
    
    FILE *file = fopen(history_file, "r");
    if (!file) {
        free(history_file);
        return 0; // Файл не существует - это нормально
    }
    
    // Проверяем размер файла
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size > MAX_HISTORY_FILE_SIZE) {
        fprintf(stderr, "Файл истории слишком большой, пропускаем загрузку\n");
        fclose(file);
        free(history_file);
        return -1;
    }
    fseek(file, 0, SEEK_SET);
    
    char line[MAX_HISTORY_LENGTH];
    int loaded_count = 0;
    
    while (fgets(line, sizeof(line), file) && loaded_count < MAX_HISTORY_SIZE) {
        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) > 0) {
            // Парсим строку: timestamp|exit_code|command
            char *token = strtok(line, "|");
            if (token) {
                time_t timestamp = (time_t)atol(token);
                
                token = strtok(NULL, "|");
                if (token) {
                    int exit_code = atoi(token);
                    
                    token = strtok(NULL, "|");
                    if (token) {
                        strncpy(state->history[loaded_count].command, token, MAX_HISTORY_LENGTH - 1);
                        state->history[loaded_count].command[MAX_HISTORY_LENGTH - 1] = '\0';
                        state->history[loaded_count].timestamp = timestamp;
                        state->history[loaded_count].exit_code = exit_code;
                        loaded_count++;
                    }
                }
            }
        }
    }
    
    state->history_count = loaded_count;
    state->history_index = loaded_count;
    
    fclose(file);
    free(history_file);
    
    if (loaded_count > 0) {
        printf("Загружено %d команд из истории\n", loaded_count);
    }
    
    return 0;
}

/**
 * @brief Сохранение истории команд в файл
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int save_history_to_file(shell_state_t *state) {
    if (!state || state->history_count == 0) {
        return 0;
    }
    
    char *history_file = get_history_file_path();
    if (!history_file) {
        return -1;
    }
    
    FILE *file = fopen(history_file, "w");
    if (!file) {
        free(history_file);
        return -1;
    }
    
    // Сохраняем только последние MAX_HISTORY_SIZE команд
    int start_index = 0;
    if (state->history_count > MAX_HISTORY_SIZE) {
        start_index = state->history_count - MAX_HISTORY_SIZE;
    }
    
    for (int i = start_index; i < state->history_count; i++) {
        fprintf(file, "%ld|%d|%s\n", 
                (long)state->history[i].timestamp,
                state->history[i].exit_code,
                state->history[i].command);
    }
    
    fclose(file);
    free(history_file);
    
    printf("Сохранено %d команд в историю\n", state->history_count - start_index);
    return 0;
}

/**
 * @brief Получение команды из истории по номеру
 * @param state Указатель на состояние оболочки
 * @param number Номер команды в истории
 * @return Указатель на команду или NULL если номер неверный
 */
const char *get_history_by_number(shell_state_t *state, int number) {
    if (!state || number < 1 || number > state->history_count) {
        return NULL;
    }
    return state->history[number - 1].command;
}

/**
 * @brief Поиск последней команды по префиксу
 * @param state Указатель на состояние оболочки
 * @param prefix Префикс для поиска
 * @return Указатель на команду или NULL если не найдено
 */
const char *get_last_command_by_prefix(shell_state_t *state, const char *prefix) {
    if (!state || !prefix) {
        return NULL;
    }
    
    for (int i = state->history_count - 1; i >= 0; i--) {
        if (strncmp(state->history[i].command, prefix, strlen(prefix)) == 0) {
            return state->history[i].command;
        }
    }
    
    return NULL;
}
