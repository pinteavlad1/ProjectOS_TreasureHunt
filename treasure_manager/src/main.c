#include "command_lookup.h"
#include "commands.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    if (chdir("../treasure_manager") != 0)
    {
        perror("Failed to change directory to treasure_manager");
        return 1;
    }

    CommandFunction command = get_command(argc, argv);
    if (command == NULL)
    {
        return 1;
    }
    command(argc, argv);

    return 0;
}