#include "command_lookup.h"
#include "commands.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    CommandFunction command = get_command(argc, argv);
    if (command == NULL)
    {
        return 1;
    }
    command(argc, argv);
    return 0;
}