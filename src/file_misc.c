#include "file_misc.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int file_exists(const char *path)
{
    struct stat statbuf;
    return (stat(path, &statbuf) == 0);
}

int file_size(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return -1;
    return statbuf.st_size;
}

time_t last_modification_time(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return -1;
    return statbuf.st_mtime;
}

const char *hunt_path(const char *hunt_name) {
    static char file_path[128];
    snprintf(file_path, sizeof(file_path), "%s/%s", "hunts", hunt_name);
    return file_path;
}

const char *treasure_file_path(const char *hunt_name, const char *treasure_name)
{
    static char file_path[128];
    snprintf(file_path, sizeof(file_path), "hunts/%s/%s", hunt_name, treasure_name);
    return file_path;
}
