#pragma once

#include <time.h>

int is_directory(const char *path);
int file_exists(const char *path);
int file_size(const char *path);
time_t last_modification_time(const char *path);
const char *hunt_path(const char *hunt_name);
const char *treasure_file_path(const char *path, const char *treasure_name);