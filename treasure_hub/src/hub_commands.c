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

#include "hub_commands.h"
#include "monitor.h"


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
    printf("Child process exited with status: %d\n", WEXITSTATUS(signum));
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

void stop_monitor(Monitor *monitor)
{

    assert(monitor != NULL);
    assert(monitor->pid != 0);
    assert(monitor->status == 1);
    assert(monitor_stopping == 0);

    kill(monitor->pid, SIGTERM);

    monitor->status = 0;

    monitor_stopping = 1;
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

    //Asteapta SIGUSR1
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
   
    //Asteapta SIGUSR1
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

    //Asteapta SIGUSR1
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
