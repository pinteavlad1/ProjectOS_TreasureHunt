#pragma once

#include <sys/types.h>

typedef struct {
    pid_t pid;
    int status;
    int pipefd[2];
} Monitor;

typedef struct {
    int id;
    char data[64];
} Command;


void set_message(char *message);

void monitor_stopped(int signum);

void start_monitor(char* message, Monitor* monitor);

void stop_monitor(char* message, Monitor* monitor);

void list_treasures(char* message, Monitor* monitor, char* hunt_id);

void view_treasure(char* message, Monitor* monitor, char* hunt_id, char* treasure_id);

void list_all_hunts(char* message, Monitor* monitor);

void link_hub_handlers();

void calculate_score(char* message, Monitor* monitor);

int is_monitor_stopping();





