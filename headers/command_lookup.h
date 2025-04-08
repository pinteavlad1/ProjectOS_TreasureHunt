#pragma once

#include "commands.h"

typedef void (*CommandFunction)(int argc, char *argv[]);

typedef enum {
    HUNT,
    HUNT_EXISTING,
    TREASURE,
} ArgumentType;

typedef struct {
    char name[21];
    int argument_count;
    CommandFunction command_func;
    ArgumentType argument_types[10];
} CommandOption;

CommandFunction get_command(int argc, char *argv[]);
