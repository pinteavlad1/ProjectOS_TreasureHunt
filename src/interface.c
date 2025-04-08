#include "interface.h"

#include <stdio.h>

Treasure treasure_from_input(const char *hunt_name)
{

    //TODO: INPUT VALIDATION

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