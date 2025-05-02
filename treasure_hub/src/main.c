#include <stdio.h>

int main() {
    char command[21];
    printf("Welcome to the Treasure Hub!\n");
    while (1) {
        printf("> ");
        scanf("%20s", command);
    }
    return 0;
}
