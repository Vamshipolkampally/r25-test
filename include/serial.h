#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>

// Function to open the serial port
FILE* open_file(const char* portname, char* mode) {
    FILE* fd = fopen(portname, mode);
    if (fd == NULL) {
        perror("Error opening file");
        return NULL;
    }
    return fd;
}

// Function to read data from SBUS
void read_SBUS(uint8_t* buffer, size_t size, size_t n, FILE* fd) {
    if (fd == NULL) {
        fprintf(stderr, "File descriptor is NULL\n");
        return;
    }

    size_t total_read = 0;
    while (total_read < n) {
        ssize_t bytes_read = fread(buffer + total_read, 1, size, fd);
        if (bytes_read < 0) {
            perror("Error reading from file");
            return;
        }
        total_read += bytes_read;
    }
}

// Function to write data to Sabertooth
void write_to_SB(FILE* fd, char* fmt, ...) {
    if (fd == NULL) {
        fprintf(stderr, "File descriptor is NULL\n");
        return;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(fd, fmt, args);
    va_end(args);

    fflush(fd); // Ensure data is written immediately
}

// Function to close the serial port
void close_file(FILE* fd) {
    if (fd != NULL) {
        fclose(fd);
    }
}

