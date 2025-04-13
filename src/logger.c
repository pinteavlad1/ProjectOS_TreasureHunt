#include "logger.h"
#include "file_misc.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void start_logging(const char* log_file_path, const char* link_file_path) {

    assert(log_file_path != NULL);
    assert(link_file_path != NULL);

    if (file_exists(log_file_path)) {
        return;
    }
    if (open(log_file_path, O_WRONLY | O_APPEND | O_CREAT, 0777) < 0) {
        perror("Error opening log file");
        return;
    }
    if (symlink(log_file_path, link_file_path) < 0) {
        perror("Error creating symlink");
        return;
    }
}

void log_message(const char* log_file_path, const char *message) {
    assert(file_exists(log_file_path));
    int fd = open(log_file_path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("Error opening log file");
        return;
    }
    if (write(fd, message, strlen(message)) != (int)strlen(message)) {
        perror("Error writing to log file");
        close(fd);
        return;
    }
    close(fd);
}