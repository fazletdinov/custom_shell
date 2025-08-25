#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    printf("Тест базовой функциональности\n");
    
    // Простой тест парсинга
    char input[] = "help";
    char *cmd = strtok(input, " ");
    
    if (cmd && strcmp(cmd, "help") == 0) {
        printf("Парсинг работает: %s\n", cmd);
    } else {
        printf("Ошибка парсинга\n");
    }
    
    return 0;
}
