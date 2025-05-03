#pragma once

#include <sys/types.h>

typedef struct {
    char *name;
    pid_t pid;
    int status;
} Monitor;


void monitor_stopped(int signum);

void start_monitor(Monitor* monitor);

void stop_monitor(Monitor* monitor);

void list_all_hunts(Monitor* monitor);

void link_hub_handlers();

int is_monitor_stopping();









