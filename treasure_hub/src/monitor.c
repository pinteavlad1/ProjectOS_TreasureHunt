#define _POSIX_SOURCE
#define _DEFAULT_SOURCE //pt usleep

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include "monitor.h"

void stop(int signum) {

    assert(signum == SIGTERM);

    printf("Stopping monitor...\n");
    usleep(500);
    exit(0);
}

void handle_user_signal(int signum) {

    assert(signum == SIGUSR1);

    printf("Trebe sa fac ceva acuma!!\n");

}

void list_hunts() {

}

void list_treasures() {}

void view_treasure() {}

void link_handlers() {

    struct sigaction termination_handler;

    termination_handler.sa_handler = &stop;
    sigemptyset(&(termination_handler.sa_mask));
    termination_handler.sa_flags = 0;

    if (sigaction(SIGTERM, &termination_handler, NULL) == -1) {
        perror("sigaction");
    }

    struct sigaction user_signal_handler;
    user_signal_handler.sa_handler = &handle_user_signal;
    sigemptyset(&(user_signal_handler.sa_mask));
    user_signal_handler.sa_flags = 0;

    if (sigaction(SIGUSR1, &user_signal_handler, NULL) == -1) {
        perror("sigaction");
    }
}
