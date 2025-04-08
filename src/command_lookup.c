#include "command_lookup.h"
#include "file_misc.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

CommandOption options[] = {
    {
        "--add",
        1,
        add,
        {HUNT}
    },
    {
        "--list",
        1,
        list,
        {HUNT_EXISTING}
    },
    {
        "--view",
        2,
        view,
        {HUNT, TREASURE}
    },
    {
        "--remove_treasure",
        2,
        remove_treasure,
        {HUNT, TREASURE}
    },
    {
        "--remove_hunt",
        1,
        remove_hunt,
        {HUNT_EXISTING}
    },
};


CommandFunction get_command(int argc, char *argv[]) {

    if (argc < 2) { 
        fprintf(stderr, "No command provided\n");
        return NULL;
    }

    int command_count = sizeof(options) / sizeof(CommandOption);
    for (int i = 0; i < command_count; ++i) {
        if (strcmp(argv[1], options[i].name) == 0) {
            if (argc != options[i].argument_count + 2) {
                fprintf(stderr, "%s requires %d arguments, but got %d\n", options[i].name, options[i].argument_count, argc - 2);
                return NULL;
            }
            for (int j = 0; j < options[i].argument_count; ++j) {
                if (options[i].argument_types[j] == HUNT_EXISTING && !is_directory(hunt_path(argv[j + 2]))) {
                    fprintf(stderr, "%s is not a valid hunt\n", argv[j + 2]);
                    return NULL;
                }
            }
            return options[i].command_func;
        }
    }
    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    return NULL;
}