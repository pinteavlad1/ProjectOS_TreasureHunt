#define _POSIX_SOURCE

#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ncurses.h>

#include "hub_commands.h"
#include "monitor.h"


//Asta e cam naspa implementarea pt functiile de signal trebe sa fie global
char *main_message;

void set_message(char *message)
{
    main_message = message;
}

int monitor_stopping = 0;
int is_monitor_stopping()
{
    return monitor_stopping;
}

void receive_data(int signum)
{
    assert(signum == SIGUSR2);
}

void monitor_stopped(int signum)
{

    assert(signum == SIGCHLD);

    monitor_stopping = 0;
    sprintf(main_message, "Monitor exited with status %d.", WEXITSTATUS(signum));

}

void copy_buffer(char *message, Monitor *monitor) {

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);

    char buffer[1024];

    int bytes_read = read(monitor->pipefd[0], buffer, sizeof(buffer));
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        strcpy(message, buffer);
    }
    else {
        perror("Failed to read from pipe");
    }
}

void start_monitor(char *message, Monitor *monitor)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 0);
    assert(monitor_stopping == 0);

    monitor->status = 1;

    int pfd[2];
    pid_t pid;

    if (pipe(pfd) < 0)
    {
        perror("Error creating pipe\n");
        return;
    }

    if ((pid = fork()) < 0)
    {
        perror("Failed to fork");
        return;
    }

    monitor->pid = pid;
    monitor->pipefd[0] = pfd[0];
    monitor->pipefd[1] = pfd[1];

    if (pid != 0)
    {
        strcpy(message, "Monitor started.");
        link_hub_handlers();
    }
    else
    {
        set_pipe_file_descriptors(pfd);
        link_handlers();
    }
}

void stop_monitor(char* message, Monitor *monitor)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    kill(monitor->pid, SIGTERM);

    monitor->status = 0;

    monitor_stopping = 1;

    strcpy(message, "Monitor stop requested.");
}

void list_all_hunts(char *message, Monitor *monitor)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    message[0] = 0;

    int file_descriptor = open("data/command.bin", O_WRONLY | O_CREAT, 0777);
    if (file_descriptor == -1)
    {
        perror("Failed to open command file");
        return;
    }
    Command *command;
    command = (Command *)malloc(sizeof(Command));
    command->id = 1;

    if (write(file_descriptor, command, sizeof(Command)) == -1)
    {
        perror("Failed to write command");
        close(file_descriptor);
        return;
    }
    close(file_descriptor);

    kill(monitor->pid, SIGUSR1);

    //Asteapta SIGUSR2
    pause();

    copy_buffer(message, monitor);
}

void list_treasures(char *message, Monitor *monitor, char *hunt_id)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    int file_descriptor = open("data/command.bin", O_WRONLY | O_CREAT, 0777);
    if (file_descriptor == -1)
    {
        perror("Failed to open command file");
        return;
    }
    Command *command;
    command = (Command *)malloc(sizeof(Command));
    command->id = 2;
    strcpy(command->data, hunt_id);

    if (write(file_descriptor, command, sizeof(Command)) == -1)
    {
        perror("Failed to write command");
        close(file_descriptor);
        return;
    }
    close(file_descriptor);

    kill(monitor->pid, SIGUSR1);
   
    //Asteapta SIGUSR2
    pause();

    copy_buffer(message, monitor);

}

void view_treasure(char *message, Monitor *monitor, char *hunt_id, char *treasure_id)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    int file_descriptor = open("data/command.bin", O_WRONLY | O_CREAT, 0777);
    if (file_descriptor == -1)
    {
        perror("Failed to open command file");
        return;
    }
    Command *command;
    command = (Command *)malloc(sizeof(Command));
    command->id = 3;
    strcpy(command->data, hunt_id);
    strcat(command->data, " ");
    strcat(command->data, treasure_id);

    if (write(file_descriptor, command, sizeof(Command)) == -1)
    {
        perror("Failed to write command");
        close(file_descriptor);
        return;
    }
    close(file_descriptor);

    kill(monitor->pid, SIGUSR1);

    //Asteapta SIGUSR2
    pause();
    
    copy_buffer(message, monitor);
}

void link_hub_handlers()
{

    struct sigaction child_handler;

    child_handler.sa_handler = &monitor_stopped;
    sigemptyset(&(child_handler.sa_mask));
    child_handler.sa_flags = 0;

    if (sigaction(SIGCHLD, &child_handler, NULL) == -1)
    {
        perror("sigaction");
    }

    struct sigaction usr2_handler;

    usr2_handler.sa_handler = &receive_data;
    sigemptyset(&(usr2_handler.sa_mask));
    usr2_handler.sa_flags = 0;
    
    if (sigaction(SIGUSR2, &usr2_handler, NULL) == -1)
    {
        perror("sigaction");
    }
}

void extract_hunt_names(const char *message_hunts, char hunt_names[][32], int *hunt_count) {

    *hunt_count = 0;
    char buffer[1024];
    strncpy(buffer, message_hunts, sizeof(buffer));
    buffer[sizeof(buffer)-1] = '\0';

    char *line = strtok(buffer, "\n");

    while (line && *hunt_count < 32) {
        char *colon = strchr(line, ':');
        if (colon) {
            size_t len = colon - line;
            if (len > 31) len = 31;
            strncpy(hunt_names[*hunt_count], line, len);
            hunt_names[*hunt_count][len] = '\0';
            (*hunt_count)++;
        }
        line = strtok(NULL, "\n");
    }
}

void calculate_score(char* message, Monitor* monitor) {

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);

    char *message_temp = (char*) malloc(1024);

    list_all_hunts(message_temp, monitor);
    if (strlen(message_temp) == 0) {
        strcpy(message, "No hunts available.");
        free(message_temp);
        return;
    }

    char hunt_names[32][32];
    int hunt_count = 0;
    extract_hunt_names(message_temp, hunt_names, &hunt_count);

    message[0] = 0;

    Command *command;
    command = (Command *)malloc(sizeof(Command));

    for (int i = 0; i < hunt_count; ++i) {

        int file_descriptor = open("data/command.bin", O_WRONLY | O_CREAT, 0777);
        if (file_descriptor == -1)
        {
            perror("Failed to open command file");
            return;
        }

        command->id = 4;
        strcpy(command->data, hunt_names[i]);
        if (write(file_descriptor, command, sizeof(Command)) == -1)
        {
            perror("Failed to write command");
            close(file_descriptor);
            free(command);
            free(message_temp);
            return;
        }

        close(file_descriptor);

        kill(monitor->pid, SIGUSR1);

        //Asteapta SIGUSR2
        pause();

        strcat(message, "Hunt: ");
        strcat(message, hunt_names[i]);
        strcat(message, "\n");
        copy_buffer(message_temp, monitor);
        strcat(message, message_temp);
        strcat(message, "\n");
    }
    
    free(command);
    free(message_temp);
}

