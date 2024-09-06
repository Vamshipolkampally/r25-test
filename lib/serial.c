#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h> // For strerror
#include <stdlib.h> // For exit

// Function to open the serial port
FILE* open_file(const char* portname, const char* mode) {
    FILE* file_ptr = fopen(portname, mode);
    if (file_ptr == NULL) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return NULL;
    }
    return file_ptr;
}

// Function to read data from SBUS
void read_SBUS(uint8_t* buffer, size_t size, size_t n, FILE* file_ptr) {
    if (file_ptr == NULL) {
        printf("File pointer is NULL, cannot read.\n");
        return;
    }
    
    size_t rc = fread(buffer, size, n, file_ptr);
    if (rc != n) {
        printf("Error reading file: %s\n", strerror(errno));
        exit(EXIT_FAILURE); // Exit if critical read failure
    }
}

// Function to write data to Sabertooth
void write_to_SB(FILE* file_ptr, const char* fmt, ...) {
    if (file_ptr == NULL) {
        printf("File pointer is NULL, cannot write.\n");
        return;
    }

    va_list args;
    va_start(args, fmt);
    int rc = vfprintf(file_ptr, fmt, args);
    va_end(args);

    if (rc < 0) {
        printf("Error writing to file: %s\n", strerror(errno));
        exit(EXIT_FAILURE); // Exit if critical write failure
    }

    fflush(file_ptr); // Ensure data is written immediately
}

// Function to close the serial port
void close_file(FILE* file_ptr) {
    if (file_ptr != NULL) {
        fclose(file_ptr);
        printf("File closed successfully.\n");
    } else {
        printf("File pointer is NULL, cannot close.\n");
    }
}

int main() {
    // Example of how to use the functions
    const char* portname = "/dev/serial0"; // Example port, adjust as needed
    FILE* serial_port = open_file(portname, "r+");
    if (serial_port == NULL) {
        return EXIT_FAILURE; // Exit if file could not be opened
    }

    // Example of reading SBUS data (assuming 25-byte buffer for SBUS protocol)
    uint8_t sbus_buffer[25];
    read_SBUS(sbus_buffer, sizeof(uint8_t), 25, serial_port);

    // Example of writing data to Sabertooth (in the format Sabertooth expects)
    write_to_SB(serial_port, "%c", 64); // Example of sending neutral command

    // Close the serial port after communication
    close_file(serial_port);

    return EXIT_SUCCESS;
}
