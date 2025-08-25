/**
 * @file shell.h
 * @brief Основной заголовочный файл для пользовательской оболочки bash
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит основные определения и структуры для пользовательской оболочки.
 * Оболочка поддерживает базовые команды, переменные окружения и выполнение внешних программ.
 */

#ifndef CUSTOM_SHELL_H
#define CUSTOM_SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <time.h> // Added for time_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def MAX_INPUT_SIZE
 * @brief Максимальный размер входной строки
 */
#define MAX_INPUT_SIZE 1024

/**
 * @def MAX_ARGS
 * @brief Максимальное количество аргументов команды
 */
#define MAX_ARGS 64

/**
 * @def MAX_PATH
 * @brief Максимальная длина пути
 */
#define MAX_PATH 256

/**
 * @def MAX_HISTORY_SIZE
 * @brief Максимальное количество команд в истории
 */
#define MAX_HISTORY_SIZE 100

/**
 * @def MAX_HISTORY_LENGTH
 * @brief Максимальная длина одной команды в истории
 */
#define MAX_HISTORY_LENGTH 1024

/**
 * @def HISTORY_FILE_NAME
 * @brief Имя файла для сохранения истории команд
 */
#define HISTORY_FILE_NAME "/.custom_shell_history"

/**
 * @def MAX_HISTORY_FILE_SIZE
 * @brief Максимальный размер файла истории в байтах
 */
#define MAX_HISTORY_FILE_SIZE (1024 * 1024) // 1MB

/**
 * @def COLOR_RESET
 * @brief ANSI escape-код для сброса цвета
 */
#define COLOR_RESET "\033[0m"

/**
 * @def COLOR_GREEN
 * @brief ANSI escape-код для зеленого цвета
 */
#define COLOR_GREEN "\033[32m"

/**
 * @def COLOR_RED
 * @brief ANSI escape-код для красного цвета
 */
#define COLOR_RED "\033[31m"

/**
 * @def COLOR_BLUE
 * @brief ANSI escape-код для синего цвета
 */
#define COLOR_BLUE "\033[34m"

/**
 * @def COLOR_YELLOW
 * @brief ANSI escape-код для желтого цвета
 */
#define COLOR_YELLOW "\033[33m"

/**
 * @def COLOR_CYAN
 * @brief ANSI escape-код для голубого цвета
 */
#define COLOR_CYAN "\033[36m"

/**
 * @def COLOR_BOLD
 * @brief ANSI escape-код для жирного шрифта
 */
#define COLOR_BOLD "\033[1m"

/**
 * @struct command_t
 * @brief Структура для хранения информации о команде
 */
typedef struct {
    char *name;           /**< Имя команды */
    char **args;          /**< Массив аргументов */
    int argc;             /**< Количество аргументов */
    char *input_file;     /**< Файл для перенаправления ввода */
    char *output_file;    /**< Файл для перенаправления вывода */
    int background;       /**< Флаг фонового выполнения */
} command_t;

/**
 * @struct history_entry_t
 * @brief Структура для хранения записи в истории команд
 */
typedef struct {
    char command[MAX_HISTORY_LENGTH];  /**< Текст команды */
    time_t timestamp;                  /**< Время выполнения */
    int exit_code;                     /**< Код выхода команды */
} history_entry_t;

/**
 * @struct shell_state_t
 * @brief Структура для хранения состояния оболочки
 */
typedef struct {
    char *prompt;         /**< Строка приглашения */
    char *current_dir;    /**< Текущая директория */
    int exit_code;        /**< Код выхода последней команды */
    int should_exit;      /**< Флаг для выхода из оболочки */
    history_entry_t history[MAX_HISTORY_SIZE];  /**< История команд */
    int history_count;    /**< Количество команд в истории */
    int history_index;    /**< Индекс текущей позиции в истории */
} shell_state_t;

// Глобальная переменная для выхода из оболочки
extern int g_should_exit;

/**
 * @brief Инициализация оболочки
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int shell_init(shell_state_t *state);

/**
 * @brief Основной цикл оболочки
 * @param state Указатель на состояние оболочки
 * @return Код выхода оболочки
 */
int shell_run(shell_state_t *state);

/**
 * @brief Очистка ресурсов оболочки
 * @param state Указатель на состояние оболочки
 */
void shell_cleanup(shell_state_t *state);

/**
 * @brief Обработка сигналов
 * @param sig Номер сигнала
 */
void signal_handler(int sig);

/**
 * @brief Добавление команды в историю
 * @param state Указатель на состояние оболочки
 * @param command Текст команды
 * @param exit_code Код выхода команды
 */
void add_to_history(shell_state_t *state, const char *command, int exit_code);

/**
 * @brief Получение команды из истории по индексу
 * @param state Указатель на состояние оболочки
 * @param index Индекс команды в истории
 * @return Указатель на команду или NULL если индекс неверный
 */
const char *get_history_command(shell_state_t *state, int index);

/**
 * @brief Поиск команды в истории по началу строки
 * @param state Указатель на состояние оболочки
 * @param prefix Префикс для поиска
 * @return Индекс найденной команды или -1 если не найдено
 */
int search_history(shell_state_t *state, const char *prefix);

/**
 * @brief Очистка истории команд
 * @param state Указатель на состояние оболочки
 */
void clear_history(shell_state_t *state);

/**
 * @brief Проверка поддержки цветов терминалом
 * @return 1 если цвета поддерживаются, 0 если нет
 */
int supports_colors(void);

/**
 * @brief Установка цвета для вывода
 * @param color ANSI escape-код цвета
 */
void set_color(const char *color);

/**
 * @brief Сброс цвета вывода
 */
void reset_color(void);

/**
 * @brief Создание цветной строки приглашения
 * @param username Имя пользователя
 * @param hostname Имя хоста
 * @param current_dir Текущая директория
 * @return Цветная строка приглашения
 */
char *create_colored_prompt(const char *username, const char *hostname, const char *current_dir);

/**
 * @brief Загрузка истории команд из файла
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int load_history_from_file(shell_state_t *state);

/**
 * @brief Сохранение истории команд в файл
 * @param state Указатель на состояние оболочки
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int save_history_to_file(shell_state_t *state);

/**
 * @brief Получение пути к файлу истории
 * @return Указатель на строку с путем или NULL в случае ошибки
 */
char *get_history_file_path(void);

/**
 * @brief Поиск команды в истории по номеру (например, !5)
 * @param state Указатель на состояние оболочки
 * @param number Номер команды в истории
 * @return Указатель на команду или NULL если номер неверный
 */
const char *get_history_by_number(shell_state_t *state, int number);

/**
 * @brief Поиск последней команды по префиксу (например, !ls)
 * @param state Указатель на состояние оболочки
 * @param prefix Префикс для поиска
 * @return Указатель на команду или NULL если не найдено
 */
const char *get_last_command_by_prefix(shell_state_t *state, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_SHELL_H */
