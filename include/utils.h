/**
 * @file utils.h
 * @brief Заголовочный файл для утилитарных функций оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 * 
 * @details
 * Этот файл содержит вспомогательные функции для работы с оболочкой,
 * включая работу со строками, переменными окружения и другими утилитами.
 */

#ifndef UTILS_H
#define UTILS_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Разделение строки по разделителю
 * @param str Строка для разделения
 * @param delim Разделитель
 * @param count Указатель для сохранения количества частей
 * @return Массив строк (нужно освободить память)
 */
char **split_string(const char *str, const char *delim, int *count);

/**
 * @brief Освобождение массива строк
 * @param strings Массив строк
 * @param count Количество строк
 */
void free_string_array(char **strings, int count);

/**
 * @brief Удаление пробелов в начале и конце строки
 * @param str Строка для обработки
 * @return Указатель на обработанную строку
 */
char *trim_string(char *str);

/**
 * @brief Получение переменной окружения
 * @param name Имя переменной
 * @return Значение переменной или NULL если не найдена
 */
char *get_env_var(const char *name);

/**
 * @brief Установка переменной окружения
 * @param name Имя переменной
 * @param value Значение переменной
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int set_env_var(const char *name, const char *value);

/**
 * @brief Расширение переменных в строке
 * @param str Строка с переменными
 * @return Новая строка с расширенными переменными
 */
char *expand_variables(const char *str);

/**
 * @brief Проверка существования файла
 * @param path Путь к файлу
 * @return 1 если файл существует, 0 если нет
 */
int file_exists(const char *path);

/**
 * @brief Получение абсолютного пути
 * @param path Относительный путь
 * @return Абсолютный путь
 */
char *get_absolute_path(const char *path);

/**
 * @brief Проверка является ли путь абсолютным
 * @param path Путь для проверки
 * @return 1 если абсолютный, 0 если относительный
 */
int is_absolute_path(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
