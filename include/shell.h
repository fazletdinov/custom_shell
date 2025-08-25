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
 * @struct shell_state_t
 * @brief Структура для хранения состояния оболочки
 */
typedef struct {
    char *prompt;         /**< Строка приглашения */
    char *current_dir;    /**< Текущая директория */
    int exit_code;        /**< Код выхода последней команды */
    int should_exit;      /**< Флаг для выхода из оболочки */
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

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_SHELL_H */
