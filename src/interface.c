#include "interface.h"
#include "file_misc.h"

#include <stdio.h>
#include <time.h>

Treasure treasure_from_input(const char *hunt_name)
{

    //TODO: INPUT VALIDATION
    //TODO: sa mearga clue cu mai multe cuvinte
    
    Treasure treasure;
    printf("Adding a new treasure to hunt: %s\n", hunt_name);
    printf("Enter the ID of the treasure: ");
    scanf("%s", treasure.id);
    printf("Enter username: ");
    scanf("%s", treasure.user_name);
    printf("Enter the coordinates (latitude longitude): ");
    scanf("%lf %lf", &treasure.coordinates.latitude, &treasure.coordinates.longitude);
    printf("Enter the clue text: ");
    scanf("%s", treasure.clue_text);
    printf("Enter the value of the treasure: ");
    scanf("%hu", &treasure.value);

    return treasure;
}

void print_treasure(const Treasure treasure)
{
    printf("ID: %s\n", treasure.id);
    printf("User name: %s\n", treasure.user_name);
    printf("Coordinates: (%lf, %lf)\n", treasure.coordinates.latitude, treasure.coordinates.longitude);
    printf("Clue text: %s\n", treasure.clue_text);
    printf("Value: %hu\n", treasure.value);
}

void print_treasures(const Treasure treasures[], int count, const char *hunt_name)
{
    printf("Hunt: %s\n", hunt_name);
    //Si aici trebe facut asta sa mearga cu mai multe fisiere
    printf("Hunt file size: %d\n", file_size(treasure_file_path(hunt_name, "treasure.bin")));

    time_t time_modified = last_modification_time(hunt_path(hunt_name));

    printf("Last modification time: %s\n", ctime(&time_modified));

    for (int i = 0; i < count; i++)
    {
        printf("Treasure %d:\n", i + 1);
        print_treasure(treasures[i]);
        printf("\n");
    }
}