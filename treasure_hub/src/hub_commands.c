#define _POSIX_SOURCE

#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>

#include "hub_commands.h"
#include "monitor.h"

int monitor_stopping = 0;
int is_monitor_stopping() {
    return monitor_stopping;
}

void start_monitor(Monitor* monitor) {

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 0);
    assert(monitor_stopping == 0);

    if (monitor->status == 1) {
        printf("Monitor is already running.\n");
        return;
    }
    monitor->status = 1;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Failed to fork");
        return;
    }

    monitor->pid = pid;

    if (pid != 0) {
        printf("Monitor started.\n");
    }
    else {
        link_handlers();
    }
}

void stop_monitor(Monitor* monitor) {

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    kill(monitor->pid, SIGTERM);

    monitor->status = 0;

    monitor_stopping = 1;

}

void list_all_hunts(Monitor* monitor) {

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    kill(monitor->pid, SIGUSR1);
    printf("Listing all hunts...\n");
}


void monitor_stopped(int signum) {

    assert(signum == SIGCHLD);

    monitor_stopping = 0;
    printf("Monitor stopped.\n");
}

void link_hub_handlers() {

    struct sigaction child_handler;

    child_handler.sa_handler = &monitor_stopped;
    sigemptyset(&(child_handler.sa_mask));
    child_handler.sa_flags = 0;

    if (sigaction(SIGCHLD, &child_handler, NULL) == -1) {
        perror("sigaction");
    }
    
}
