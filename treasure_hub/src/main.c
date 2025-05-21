#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "hub_commands.h"

int main() {
    char command[21];
	Monitor monitor = {};
	monitor.name = "Treasure Monitor";
	monitor.pid = -1;
	monitor.status = 0;
	monitor.pipefd[0] = -1;
	monitor.pipefd[1] = -1;

	char *message = (char*) malloc(1024);

    printf("Welcome to the Treasure Hub!\n");
    while (1) {

		// Monitor not started or we are the main process
		if (monitor.pid != 0) {

			command[0] = 0; //Daca monitor da printf in timp ce asteptam input, trece peste scanf si ruleaza ultima comanda

			printf("> ");
			scanf("%20s", command);

			if (command[0] != '\0') {
				if (is_monitor_stopping()) {
					printf("Cannot execute commands: monitor is stopping.\n");
				}
				else if (strcmp(command, "start_monitor") == 0) {
					if (monitor.status == 1) {
						printf("Monitor is already running.\n");
					} 
					else {
						start_monitor(message, &monitor);
						printf("%s\n", message);
					}
				} 
				else if (strcmp(command, "stop_monitor") == 0) {
					if (monitor.status == 0) {
						printf("Monitor is not running.\n");
					} 
					else {
						stop_monitor(&monitor);
					}
				} 
				else if (strcmp(command, "list_hunts") == 0) {
					if (monitor.status == 0) {
						printf("Monitor is not running.\n");
					} 
					else {
						list_all_hunts(message, &monitor);
						printf("%s\n", message);
					}
				} 
				else if (strcmp(command, "list_treasures") == 0) {
					if (monitor.status == 0) {
						printf("Monitor is not running.\n");
					} 
					else {
						char hunt_id[21];
						printf("Enter hunt ID: ");
						scanf("%20s", hunt_id);
						list_treasures(message, &monitor, hunt_id);
						printf("%s\n", message);
					}
				} 
				else if (strcmp(command, "view_treasure") == 0) {
					char hunt_id[21], treasure_id[21];
					printf("Enter hunt ID: ");
					scanf("%20s", hunt_id);
					printf("Enter treasure ID: ");
					scanf("%20s", treasure_id);
					view_treasure(message, &monitor, hunt_id, treasure_id);
					printf("%s\n", message);
				} 
				else if (strcmp(command, "exit") == 0) {
					break;
				} 
				else if (strcmp(command, "clear") == 0) {
					printf("\e[1;1H\e[2J"); //ANSI clear screen
				}
				else if (strcmp(command, "exit") == 0) {
					break;
				} 
				else
				{
					printf("Unknown command: %s\n", command);
				}
			}
		}
    }
	free(message);
    return 0;
}