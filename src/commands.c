#include "commands.h"
#include "file_misc.h"
#include "treasure.h"
#include "interface.h"
#include "logger.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define TREASURE_READ_CHUNK 128

//TODO: Mesaje mai ok de log 

//TODO: Sa nu dau aici in fileu asta perror sa fac cu return value cred ca e mai ok

void add(int argc, char *argv[])
{

    assert(argc == 3);
    assert(argv[2] != NULL);

    if (!is_directory("hunts"))
    {
        if (mkdir("hunts", 0777) != 0)
        {
            perror("Error at mkdir");
            return;
        }
    }

    if (!is_directory(hunt_path(argv[2])))
    {
        if (mkdir(hunt_path(argv[2]), 0777) != 0)
        {
            perror("Error at mkdir");
            return;
        }
        start_logging(treasure_file_path(argv[2], "logged_hunt.txt"), symlink_file_path(argv[2]));
    }

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_WRONLY | O_CREAT | O_APPEND, 0777);

    if (file_descriptor < 0)
    {
        perror("Error at open");
        return;
    }

    Treasure treasure = treasure_from_input(argv[2]);

    // Aici trebe practic tot treasure sa nu fie NULL deci
    // probabil o sa ii fac in treasure.c o functie care zice daca un treasure ii NULL sau nu

    assert(treasure.id != NULL);

    if (write(file_descriptor, &treasure, sizeof(Treasure)) != sizeof(Treasure))
    {
        perror("Error at write");
        close(file_descriptor);
        return;
    }

    log_message(treasure_file_path(argv[2], "logged_hunt.txt"), "Added treasure\n");

    close(file_descriptor);
}

// TODO: Toate astea de sub vreau sa mearga pe mai multe fileuri

void list(int argc, char *argv[])
{
    assert(argc == 3);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_RDONLY);

    if (file_descriptor < 0)
    {
        perror("Error at open");
        return;
    }

    Treasure *treasures = malloc(TREASURE_READ_CHUNK * sizeof(Treasure));
    if (treasures == NULL)
    {
        perror("Error at malloc");
        close(file_descriptor);
        return;
    }

    int count = 0;

    while (read(file_descriptor, &treasures[count], sizeof(Treasure)) == sizeof(Treasure))
    {
        count++;
        if (count % TREASURE_READ_CHUNK == 0)
        {
            treasures = realloc(treasures, (count + TREASURE_READ_CHUNK) * sizeof(Treasure));
            if (treasures == NULL)
            {
                perror("Error at realloc");
                close(file_descriptor);
                return;
            }
        }
    }

    treasures = realloc(treasures, count * sizeof(Treasure));

    if (count == 0)
    {
        printf("No treasures found in hunt: %s\n", argv[2]);
    }
    else
    {
        print_treasures(treasures, count, argv[2]);
    }

    log_message(treasure_file_path(argv[2], "logged_hunt.txt"), "Listed hunt\n");

    free(treasures);
    close(file_descriptor);
}

void view(int argc, char *argv[])
{
    
    assert(argc == 4);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    int file_descriptor;

    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_RDONLY);

    if (file_descriptor < 0)
    {
        perror("Error at open");
        return;
    }

    Treasure treasure;

    while (read(file_descriptor, &treasure, sizeof(Treasure)) == sizeof(Treasure))
    {
        printf("Reading treasure with ID: %s\n", treasure.id);
        if (strcmp(treasure.id, argv[3]) == 0)
        {
            print_treasure(treasure);
            return;
        }
    }

    printf("Treasure with ID %s not found in hunt: %s\n", argv[3], argv[2]);

    log_message(treasure_file_path(argv[2], "logged_hunt.txt"), "Viewed treasure\n");


    close(file_descriptor);
}

void remove_treasure(int argc, char *argv[])
{

    assert(argc == 4);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));
    assert(argv[3] != NULL);
    assert(file_exists(treasure_file_path(argv[2], "treasure.bin")));

    int file_descriptor;
    file_descriptor = open(treasure_file_path(argv[2], "treasure.bin"), O_RDWR);

    if (file_descriptor < 0)
    {
        perror("Error at open");
        return;
    }

    Treasure treasure;
    int index = 0;

    while (read(file_descriptor, &treasure, sizeof(Treasure)) == sizeof(Treasure))
    {
        if (strcmp(treasure.id, argv[3]) == 0)
        {
            break;
        }   
        ++index;
    }

    close(file_descriptor);

    int file_descriptor_write = open(treasure_file_path(argv[2], "treasure.bin"), O_WRONLY, 0777);
    if (file_descriptor_write < 0)
    {
        perror("Error at open");
        return;
    }
    int file_descriptor_read = open(treasure_file_path(argv[2], "treasure.bin"), O_RDONLY, 0777);
    if (file_descriptor_read < 0)
    {
        perror("Error at open");
        return;
    }
    
    lseek(file_descriptor_read, (index + 1) * sizeof(Treasure), SEEK_SET);
    lseek(file_descriptor_write, index * sizeof(Treasure), SEEK_SET);

    while (read(file_descriptor_read, &treasure, sizeof(Treasure)) == sizeof(Treasure))
    {
        write(file_descriptor_write, &treasure, sizeof(Treasure));
        ++index;
    }

    close(file_descriptor_write);
    close(file_descriptor_read);

    if (truncate(treasure_file_path(argv[2], "treasure.bin"), (index) * sizeof(Treasure)) != 0)
    {
        perror("Error at truncate");
        return;
    }

    log_message(treasure_file_path(argv[2], "logged_hunt.txt"), "Removed treasure\n");

}

void remove_hunt(int argc, char *argv[])
{

    assert(argc == 3);
    assert(argv[2] != NULL);
    assert(is_directory(hunt_path(argv[2])));

    if (unlink(symlink_file_path(argv[2])) != 0)
    {
        perror("Error at unlink");
        return;
    }

    if (unlink(treasure_file_path(argv[2], "treasure.bin")) != 0)
    {
        perror("Error at unlink");
        return;
    }

    if (unlink(treasure_file_path(argv[2], "logged_hunt.txt")) != 0)
    {
        perror("Error at unlink");
        return;
    }

    if (rmdir(hunt_path(argv[2])) != 0)
    {
        perror("Error at rmdir");
        return;
    }

    printf("Hunt %s removed successfully\n", argv[2]);
}