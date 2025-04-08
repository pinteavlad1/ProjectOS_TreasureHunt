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

const char *hunt_path(const char *hunt_name) {
    static char file_path[128];
    snprintf(file_path, sizeof(file_path), "%s/%s", "hunts", hunt_name);
    return file_path;
}

const char *treasure_file_path(const char *path, const char *treasure_name)
{
    static char file_path[128];
    snprintf(file_path, sizeof(file_path), "hunts/%s/%s", path, treasure_name);
    return file_path;
}
