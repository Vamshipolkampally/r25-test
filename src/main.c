#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

// Constants for SBUS and Sabertooth ranges
#define SBUS_MIN 172
#define SBUS_MAX 1811
#define SABERTOOTH_MIN 1
#define SABERTOOTH_NEUTRAL 64
#define SABERTOOTH_MAX 127

// Function prototypes
FILE *open_file(const char *port_name, const char *mode);
void close_file(FILE *file);
size_t read_SBUS(uint8_t *buffer, size_t size, FILE *file);
uint16_t *parse_buffer(uint8_t buff[]);
int interpolation(uint16_t channel);
void write_to_SB(FILE *file, const char *format, int value);

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <SBUS_port> <Sabertooth_port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *port_name_1 = argv[1]; // SBUS 
    char *port_name_2 = argv[2]; // Sabertooth

    FILE *sbus = NULL;
    FILE *sabertooth = NULL;

    // To store SBUS packets
    uint8_t sbus_packet[25] = {0};

    // To store value of individual RC channel
    uint16_t *channel = NULL;

    // PWM value after interpolation
    int pwm = 0;

    // Opening serial ports for communication with SBUS and Sabertooth
    sbus = open_file(port_name_1, "rb");
    if (!sbus) {
        fprintf(stderr, "Failed to open SBUS port %s\n", port_name_1);
        return EXIT_FAILURE;
    }

    sabertooth = open_file(port_name_2, "w");
    if (!sabertooth) {
        fprintf(stderr, "Failed to open Sabertooth port %s\n", port_name_2);
        close_file(sbus);
        return EXIT_FAILURE;
    }

    // Read data from RC transmitter using SBUS
    if (read_SBUS(sbus_packet, sizeof(sbus_packet), sbus) != sizeof(sbus_packet)) {
        fprintf(stderr, "Failed to read SBUS data\n");
        close_file(sbus);
        close_file(sabertooth);
        return EXIT_FAILURE;
    }

    // Parsing SBUS packet
    channel = parse_buffer(sbus_packet);

    // Get PWM range for Sabertooth 1
    pwm = interpolation(channel[0]);

    // Write data to Sabertooth 1
    write_to_SB(sabertooth, "%d\n", pwm);
    fflush(sabertooth); // Ensure the data is sent immediately

    // Closing all serial ports
    close_file(sbus);
    close_file(sabertooth);

    return EXIT_SUCCESS;
}

// Function to open a file or serial port
FILE *open_file(const char *port_name, const char *mode) {
    FILE *file = fopen(port_name, mode);
    if (!file) {
        perror("fopen");
        return NULL;
    }
    return file;
}

// Function to close a file or serial port
void close_file(FILE *file) {
    if (file) {
        fclose(file);
    }
}

// Function to read SBUS data (actual size of SBUS packet is 25 bytes)
size_t read_SBUS(uint8_t *buffer, size_t size, FILE *file) {
    return fread(buffer, 1, size, file);
}

// Function to parse SBUS data into channels
uint16_t *parse_buffer(uint8_t buff[]) {
    static uint16_t channel[16];
    uint16_t mask = 0x7ff;

    channel[0]  = ((buff[1] | buff[2] << 8)                 & mask);
    channel[1]  = ((buff[2] >> 3 | buff[3] << 5)            & mask);
    channel[2]  = ((buff[3] >> 6 | buff[4] << 2 | buff[5] << 10) & mask);
    channel[3]  = ((buff[5] >> 1 | buff[6] << 7)            & mask);
    channel[4]  = ((buff[6] >> 4 | buff[7] << 4)            & mask);
    channel[5]  = ((buff[7] >> 7 | buff[8] << 1 | buff[9] << 9)  & mask);
    channel[6]  = ((buff[9] >> 2 | buff[10] << 6)           & mask);
    channel[7]  = ((buff[10] >> 5 | buff[11] << 3)          & mask);
    // Add remaining channels as necessary

    return channel;
}

// Function to interpolate SBUS value to Sabertooth PWM value
int interpolation(uint16_t channel) {
    if (channel < SBUS_MIN) channel = SBUS_MIN;
    if (channel > SBUS_MAX) channel = SBUS_MAX;

    int pwm;
    if (channel < 992) {
        // Reverse direction
        pwm = (int)((channel - SBUS_MIN) * (SABERTOOTH_NEUTRAL - SABERTOOTH_MIN) / (992 - SBUS_MIN) + SABERTOOTH_MIN);
    } else {
        // Forward direction
        pwm = (int)((channel - 992) * (SABERTOOTH_MAX - SABERTOOTH_NEUTRAL) / (SBUS_MAX - 992) + SABERTOOTH_NEUTRAL);
    }

    // Ensure PWM is within Sabertooth bounds
    if (pwm < SABERTOOTH_MIN) pwm = SABERTOOTH_MIN;
    if (pwm > SABERTOOTH_MAX) pwm = SABERTOOTH_MAX;

    return pwm;
}

// Function to write data to Sabertooth
void write_to_SB(FILE *file, const char *format, int value) {
    fprintf(file, format, value);
}
