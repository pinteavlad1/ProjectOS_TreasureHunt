#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "../treasure_manager/headers/treasure.h"

char* keys[256];
int values[393242];

uint32_t hash_djb2(const char *str) {

    uint32_t hash = 5381;

   for (int i = 0; i < strlen(str); i++) {
        hash = ((hash << 5) + hash) + str[i];
    }

    return hash;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <hunt_id>\n", argv[0]);
        return 1;
    }

    char treasure_path[256];
    snprintf(treasure_path, sizeof(treasure_path), "../treasure_manager/hunts/%s/treasure.bin", argv[1]);

    int fd = open(treasure_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open treasure.bin");
        return 1;
    }
    
    int k = 0;
    Treasure treasure;

    while (read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure))
    {
        uint32_t hash = hash_djb2(treasure.user_name);
        int index = hash % 393241;

        if (values[index] == 0) {
            keys[k] = malloc(strlen(treasure.user_name) + 1);
            if (keys[k] == NULL) {
                perror("Failed to allocate memory for key");
                close(fd);
                return 1;
            }
            strcpy(keys[k], treasure.user_name);
            values[index] = treasure.value;
            k++;
        } else {
            values[index] += treasure.value;
        }
    }

    for (int i = 0; i < k; i++) {
        if (keys[i] != NULL) {
            printf("ID: %s, Value: %d\n", keys[i], values[hash_djb2(keys[i]) % 393241]);
            free(keys[i]);
        }
    }

    close(fd);

    return 0;   
}