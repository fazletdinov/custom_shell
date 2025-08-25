/**
 * @file utils.c
 * @brief Реализация утилитарных функций оболочки
 * @author Custom Shell Team
 * @version 1.0.0
 * @date 2024
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

/**
 * @brief Разделение строки по разделителю
 * @param str Строка для разделения
 * @param delim Разделитель
 * @param count Указатель для сохранения количества частей
 * @return Массив строк (нужно освободить память)
 */
char **split_string(const char *str, const char *delim, int *count) {
    if (!str || !delim || !count) {
        return NULL;
    }
    
    // Подсчет количества частей
    *count = 0;
    char *temp_str = strdup(str);
    if (!temp_str) {
        return NULL;
    }
    
    char *token = strtok(temp_str, delim);
    while (token) {
        (*count)++;
        token = strtok(NULL, delim);
    }
    
    free(temp_str);
    
    if (*count == 0) {
        return NULL;
    }
    
    // Выделение памяти для массива
    char **result = malloc((*count + 1) * sizeof(char *));
    if (!result) {
        return NULL;
    }
    
    // Разделение строки
    temp_str = strdup(str);
    if (!temp_str) {
        free(result);
        return NULL;
    }
    
    token = strtok(temp_str, delim);
    int i = 0;
    while (token && i < *count) {
        result[i] = strdup(token);
        if (!result[i]) {
            // Очистка в случае ошибки
            for (int j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            free(temp_str);
            return NULL;
        }
        i++;
        token = strtok(NULL, delim);
    }
    
    result[i] = NULL; // Завершающий NULL
    free(temp_str);
    
    return result;
}

/**
 * @brief Освобождение массива строк
 * @param strings Массив строк
 * @param count Количество строк
 */
void free_string_array(char **strings, int count) {
    if (!strings) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        if (strings[i]) {
            free(strings[i]);
        }
    }
    free(strings);
}

/**
 * @brief Удаление пробелов в начале и конце строки
 * @param str Строка для обработки
 * @return Указатель на обработанную строку
 */
char *trim_string(char *str) {
    if (!str) {
        return NULL;
    }
    
    // Удаление пробелов в начале
    while (isspace(*str)) {
        str++;
    }
    
    if (*str == '\0') {
        return str;
    }
    
    // Удаление пробелов в конце
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }
    
    *(end + 1) = '\0';
    
    return str;
}

/**
 * @brief Получение переменной окружения
 * @param name Имя переменной
 * @return Значение переменной или NULL если не найдена
 */
char *get_env_var(const char *name) {
    if (!name) {
        return NULL;
    }
    
    return getenv(name);
}

/**
 * @brief Установка переменной окружения
 * @param name Имя переменной
 * @param value Значение переменной
 * @return 0 в случае успеха, -1 в случае ошибки
 */
int set_env_var(const char *name, const char *value) {
    if (!name) {
        return -1;
    }
    
    if (setenv(name, value ? value : "", 1) != 0) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Расширение переменных в строке
 * @param str Строка с переменными
 * @return Новая строка с расширенными переменными
 */
char *expand_variables(const char *str) {
    if (!str) {
        return NULL;
    }
    
    // Простая реализация - замена $VAR на значение
    char *result = malloc(strlen(str) * 2); // Максимальное расширение
    if (!result) {
        return NULL;
    }
    
    int j = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '$' && str[i + 1] && isalpha(str[i + 1])) {
            // Найдена переменная
            char var_name[256] = {0};
            int k = 0;
            i++; // Пропустить $
            
            while (str[i] && (isalnum(str[i]) || str[i] == '_')) {
                var_name[k++] = str[i++];
            }
            var_name[k] = '\0';
            i--; // Вернуться к последнему символу переменной
            
            // Получить значение переменной
            char *var_value = get_env_var(var_name);
            if (var_value) {
                strcpy(result + j, var_value);
                j += strlen(var_value);
            }
        } else {
            result[j++] = str[i];
        }
    }
    
    result[j] = '\0';
    
    // Перевыделение памяти под точный размер
    char *final_result = realloc(result, strlen(result) + 1);
    return final_result ? final_result : result;
}

/**
 * @brief Проверка существования файла
 * @param path Путь к файлу
 * @return 1 если файл существует, 0 если нет
 */
int file_exists(const char *path) {
    if (!path) {
        return 0;
    }
    
    struct stat st;
    return (stat(path, &st) == 0);
}

/**
 * @brief Получение абсолютного пути
 * @param path Относительный путь
 * @return Абсолютный путь
 */
char *get_absolute_path(const char *path) {
    if (!path) {
        return NULL;
    }
    
    if (is_absolute_path(path)) {
        return strdup(path);
    }
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return NULL;
    }
    
    char *result = malloc(strlen(cwd) + strlen(path) + 2);
    if (!result) {
        return NULL;
    }
    
    sprintf(result, "%s/%s", cwd, path);
    return result;
}

/**
 * @brief Проверка является ли путь абсолютным
 * @param path Путь для проверки
 * @return 1 если абсолютный, 0 если относительный
 */
int is_absolute_path(const char *path) {
    if (!path) {
        return 0;
    }
    
    return (path[0] == '/');
}
