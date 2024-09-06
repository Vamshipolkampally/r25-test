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
        return 1;
    }

    char *port_name_1 = argv[1]; // SBUS 
    char *port_name_2 = argv[2]; // Sabertooth1

    FILE *sbus; 
    FILE *sabertooth;

    // to store SBUS packets
    uint8_t sbus_packet[25];

    // to store value of individual RC channel
    uint16_t *channel;

    // PWM value after interpolation
    int pwm;

    // Opening serial ports for communication with SBUS and Sabertooth
    sbus = open_file(port_name_1, "rb");
    if (!sbus) {
        perror("Failed to open SBUS port");
        return 1;
    }

    sabertooth = open_file(port_name_2, "w");
    if (!sabertooth) {
        perror("Failed to open Sabertooth port");
        close_file(sbus);
        return 1;
    }

    // Read data from RC transmitter using SBUS
    if (read_SBUS(sbus_packet, sizeof(sbus_packet), sbus) == 0) {
        fprintf(stderr, "Failed to read SBUS data\n");
        close_file(sbus);
        close_file(sabertooth);
        return 1;
    }

    // Parsing SBUS packet
    channel = parse_buffer(sbus_packet);

    // Get PWM range for Sabertooth 1
    pwm = interpolation(channel[0]);

    // Write data to Sabertooth 1
    write_to_SB(sabertooth, "%d\n", pwm);

    // Closing all serial ports
    close_file(sbus);
    close_file(sabertooth);

    return 0;
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

// Function to read SBUS data (mock implementation)
size_t read_SBUS(uint8_t *buffer, size_t size, FILE *file) {
    return fread(buffer, 1, size, file);
}

// Function to parse SBUS data into channels (mock implementation)
uint16_t *parse_buffer(uint8_t buff[]) {
    static uint16_t channel[16];
    uint16_t mask = 0x7ff;

    channel[0] = ((buff[1] | buff[2] << 8) & mask);
    channel[1] = ((buff[2] >> 3 | buff[3] << 5) & mask);
    // Add more channels as needed

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
