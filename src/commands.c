#include "commands.h"
#include "file_misc.h"
#include "treasure.h"
#include "interface.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void add(int argc, char *argv[]) {

    assert(argc == 3);
    assert(argv[2] != NULL);

    if (!is_directory("hunts")) {
        if (mkdir("hunts", 0777) != 0) {
            perror("Error at mkdir");
            return;
        }
    }

    if (!is_directory(hunt_path(argv[2]))) {
        if (mkdir(hunt_path(argv[2]), 0777) != 0) {
            perror("Error at mkdir");
            return;
        }
    }

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_WRONLY | O_CREAT | O_APPEND, 0777);

    if (file_descriptor < 0) {
        perror("Error at open");
        return;
    }

    Treasure treasure = treasure_from_input(argv[2]);

    //Aici trebe practic tot treasure sa nu fie NULL deci 
    //probabil o sa ii fac in treasure.c o functie care zice daca un treasure ii NULL sau nu
    
    assert(treasure.id != NULL);

    if (write(file_descriptor, &treasure, sizeof(Treasure)) != sizeof(Treasure)) {
        perror("Error at write");
        close(file_descriptor);
        return;
    }

    close(file_descriptor);
    
}

//TODO: Toate astea de sub vreau sa mearga pe mai multe fileuri

void list(int argc, char *argv[]) {
    assert(argc == 3);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_RDONLY);

    if (file_descriptor < 0) {
        perror("Error at open");
        return;
    }

    //TODO: De refacut aici sa mearga pe mai mult de 100 de treasures

    Treasure treasures[100];
    int count = 0;

    while (read(file_descriptor, &treasures[count], sizeof(Treasure)) == sizeof(Treasure)) {
        count++;
    }

    if (count == 0) {
        printf("No treasures found in hunt: %s\n", argv[2]);
    } else {
        print_treasures(treasures, count, argv[2]);
    }

    close(file_descriptor);

}

void view(int argc, char *argv[]) {

    assert(argc == 4);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_RDONLY);

    if (file_descriptor < 0) {
        perror("Error at open");
        return;
    }

    Treasure treasure;

    while (read(file_descriptor, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("Reading treasure with ID: %s\n", treasure.id);
        if (strcmp(treasure.id, argv[3]) == 0) {
            print_treasure(treasure);
            return;
        }
    }

    printf("Treasure with ID %s not found in hunt: %s\n", argv[3], argv[2]);

    close(file_descriptor);
}

void remove_treasure(int argc, char *argv[]) {

}

void remove_hunt(int argc, char *argv[]) {
    assert(argc == 3);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    if (unlink(treasure_file_path(argv[2], "treasure.bin")) != 0) {
        perror("Error at unlink");
        return;
    }

    if (rmdir(hunt_path(argv[2])) != 0) {
        perror("Error at rmdir");
        return;
    }

    printf("Hunt %s removed successfully\n", argv[2]);
}