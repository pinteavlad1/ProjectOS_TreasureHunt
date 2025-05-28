#pragma once

void set_pipe_file_descriptors(int fd[2]);

void stop(int signum);

void list_hunts();

void get_treasures(char *hunt_id);

void get_treasure(char *hunt_id, char *treasure_id);

void get_score(char *hunt_id);

void link_handlers();