/**
 * @file builtins.h
 * @brief Заголовочный файл для встроенных команд оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит функции для встроенных команд оболочки,
 * таких как cd, pwd, echo, exit и других.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Встроенная команда cd (смена директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_cd(char **args, int argc);

/**
 * @brief Встроенная команда pwd (текущая директория)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_pwd(char **args, int argc);

/**
 * @brief Встроенная команда echo (вывод текста)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_echo(char **args, int argc);

/**
 * @brief Встроенная команда exit (выход из оболочки)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return Код выхода
 */
int builtin_exit(char **args, int argc);

/**
 * @brief Встроенная команда help (справка)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_help(char **args, int argc);

/**
 * @brief Встроенная команда clear (очистка экрана)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_clear(char **args, int argc);

/**
 * @brief Встроенная команда history (история команд)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_history(char **args, int argc);

/**
 * @brief Встроенная команда touch (создание файла)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_touch(char **args, int argc);

/**
 * @brief Встроенная команда rm (удаление файла)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_rm(char **args, int argc);

/**
 * @brief Встроенная команда mkdir (создание директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_mkdir(char **args, int argc);

/**
 * @brief Встроенная команда rmdir (удаление директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_rmdir(char **args, int argc);

/**
 * @brief Встроенная команда ls (просмотр содержимого директории)
 * @param args Аргументы команды
 * @param argc Количество аргументов
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int builtin_ls(char **args, int argc);

#ifdef __cplusplus
}
#endif

#endif /* BUILTINS_H */
