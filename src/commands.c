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

void add(int argc, char *argv[]) {

    assert(argc == 3);
    assert(argv[2] != NULL);

    if (!is_directory("hunts")) {
        if (mkdir("hunts", 0777) != 0) {
            perror("Error at mkdir: ");
            return;
        }
    }

    if (!is_directory(hunt_path(argv[2]))) {
        if (mkdir(hunt_path(argv[2]), 0777) != 0) {
            perror("Error at mkdir: ");
            return;
        }
    }

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_WRONLY | O_CREAT, 0777);

    if (file_descriptor < 0) {
        perror("Error at open: ");
        return;
    }

    Treasure treasure = treasure_from_input(argv[2]);

    //Aici trebe practic tot treasure sa nu fie NULL deci 
    //probabil o sa ii fac in treasure.c o functie care zice daca un treasure ii NULL sau nu
    
    assert(treasure.id != NULL);

    if (write(file_descriptor, &treasure, sizeof(Treasure)) != sizeof(Treasure)) {
        perror("Error at write: ");
        close(file_descriptor);
        return;
    }

    close(file_descriptor);
    
}

void list(int argc, char *argv[]) {

}

void view(int argc, char *argv[]) {

}

void remove_treasure(int argc, char *argv[]) {

}

void remove_hunt(int argc, char *argv[]) {

}