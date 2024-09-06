#include <stdint.h>
#include <math.h>

// Define the Sabertooth range and SBUS range
#define SBUS_MIN 172
#define SBUS_MAX 1811
#define SABERTOOTH_MIN 1
#define SABERTOOTH_NEUTRAL 64
#define SABERTOOTH_MAX 127

// Interpolation function: converts SBUS channel values to Sabertooth-compatible PWM signals
int interpolation(uint16_t channel) {
    // Clamp the channel value within SBUS range
    if (channel < SBUS_MIN) channel = SBUS_MIN;
    if (channel > SBUS_MAX) channel = SBUS_MAX;

    int pwm;
    if (channel < 992) {
        // Map SBUS values below 992 to reverse PWM (1-63)
        pwm = (int)((channel - SBUS_MIN) * (SABERTOOTH_NEUTRAL - SABERTOOTH_MIN) / (992 - SBUS_MIN) + SABERTOOTH_MIN);
    } else {
        // Map SBUS values above 992 to forward PWM (65-127)
        pwm = (int)((channel - 992) * (SABERTOOTH_MAX - SABERTOOTH_NEUTRAL) / (SBUS_MAX - 992) + SABERTOOTH_NEUTRAL);
    }

    // Ensure PWM is within Sabertooth range
    if (pwm < SABERTOOTH_MIN) pwm = SABERTOOTH_MIN;
    if (pwm > SABERTOOTH_MAX) pwm = SABERTOOTH_MAX;

    return pwm;
}

// Function to parse SBUS buffer data into 16 channels
uint16_t *parse_buffer(uint8_t buff[]) {
    static uint16_t channel[16]; // To store the 16 channels
    uint16_t mask = 0x7FF; // Mask to extract 11 bits

    // Parse SBUS data to extract 11-bit channel values
    channel[0]  = ((buff[1]  | (buff[2] << 8)) & mask);
    channel[1]  = ((buff[2] >> 3 | (buff[3] << 5)) & mask);
    channel[2]  = ((buff[3] >> 6 | (buff[4] << 2) | (buff[5] << 10)) & mask);
    channel[3]  = ((buff[5] >> 1 | (buff[6] << 7)) & mask);
    channel[4]  = ((buff[6] >> 4 | (buff[7] << 4)) & mask);
    channel[5]  = ((buff[7] >> 7 | (buff[8] << 1) | (buff[9] << 9)) & mask);
    channel[6]  = ((buff[9] >> 2 | (buff[10] << 6)) & mask);
    channel[7]  = ((buff[10] >> 5 | (buff[11] << 3)) & mask);
    channel[8]  = ((buff[12] | (buff[13] << 8)) & mask);
    channel[9]  = ((buff[13] >> 3 | (buff[14] << 5)) & mask);
    channel[10] = ((buff[14] >> 6 | (buff[15] << 2) | (buff[16] << 10)) & mask);
    channel[11] = ((buff[16] >> 1 | (buff[17] << 7)) & mask);
    channel[12] = ((buff[17] >> 4 | (buff[18] << 4)) & mask);
    channel[13] = ((buff[18] >> 7 | (buff[19] << 1) | (buff[20] << 9)) & mask);
    channel[14] = ((buff[20] >> 2 | (buff[21] << 6)) & mask);
    channel[15] = ((buff[21] >> 5 | (buff[22] << 3)) & mask);

    return channel;
}


