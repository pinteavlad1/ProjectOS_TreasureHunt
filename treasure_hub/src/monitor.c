#define _POSIX_SOURCE
#define _DEFAULT_SOURCE //pt usleep

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "monitor.h"
#include "hub_commands.h"

int pipefd[2];

void set_pipe_file_descriptors(int fd[2]) {
    pipefd[0] = fd[0];
    pipefd[1] = fd[1];

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
        perror("Failed to redirect stdout");
        return;
    }
    if (dup2(pipefd[1], STDERR_FILENO) == -1) {
        perror("Failed to redirect stderr");
        return;
    }

    close(pipefd[0]);
}

void stop(int signum) {

    assert(signum == SIGTERM);
    
    printf("Stopping monitor...\n");
    close(pipefd[1]);

    usleep(1000000);
    exit(0);
}

//Am file paths relative in celalalt program
//TODO: Sa schimb creca e mai ok
void list_hunts() {

    int pid = fork();

    if (pid == 0) {

        if (chdir("../treasure_manager") != 0) {
            perror("chdir to new directory failed");
            return;
        }

        execl("./treasure_manager", "treasure_manager", "--list_hunts", NULL);
    }
    else {
        waitpid(pid, NULL, 0);
        kill(getppid(), SIGUSR1);
    }

}

void get_treasures(char *hunt_id) {

    int pid = fork();

    if (pid == 0) {

        if (chdir("../treasure_manager") != 0) {
            perror("chdir to new directory failed");
            return;
        }

        execl("./treasure_manager", "treasure_manager", "--list", hunt_id, NULL);

    }
    
}

void get_treasure(char *hunt_id, char *treasure_id) {

    int pid = fork();

    if (pid == 0) {

        if (chdir("../treasure_manager") != 0) {
            perror("chdir to new directory failed");
            return;
        }

        execl("./treasure_manager", "treasure_manager", "--view", hunt_id, treasure_id, NULL);

    }

}

void handle_user_signal(int signum) {

    assert(signum == SIGUSR1);

    int file_descriptor = open("data/command.bin", O_RDONLY);

    if (file_descriptor == -1) {
        perror("Failed to open command file");
        return;
    }

    Command command = {};

    if (read(file_descriptor, &command, sizeof(Command)) != sizeof(Command)) {
        perror("Failed to read command");
        close(file_descriptor);
        return;
    }

    if (command.id == 1) {
        list_hunts();
    }

    if (command.id == 2) {
        get_treasures(command.data);
    }

    if (command.id == 3) {
        char treasure_id[21] = "";
        char hunt_id[21] = "";

        int read_toggle = 0, k = 0;

        for (size_t i = 0; i < strlen(command.data); i++) {
            if (command.data[i] == ' ') {
                read_toggle = 1;

            }
            else {
                if (read_toggle == 0) {
                    hunt_id[i] = command.data[i];
                }
                else {
                    treasure_id[k++] = command.data[i];
                }
            }
        }
        hunt_id[strlen(hunt_id)] = '\0';
        treasure_id[strlen(treasure_id)] = '\0';
        get_treasure(hunt_id, treasure_id);   
    }


    close(file_descriptor);
}

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