#include <stdint.h>

// Define constants for SBUS and Sabertooth ranges
#define SBUS_MIN 172
#define SBUS_MAX 1811
#define SABERTOOTH_MIN 1
#define SABERTOOTH_NEUTRAL 64
#define SABERTOOTH_MAX 127

// Function to parse SBUS data buffer into channel values
uint16_t *parse_buffer(uint8_t buff[]) {
    static uint16_t channel[16]; // Array to store 16 channel values
    uint16_t mask = 0x7FF; // Mask for 11-bit data

    // Extracting 11-bit channels from the buffer
    channel[0]  = ((buff[1] | (buff[2] << 8)) & mask);
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

// Function to interpolate SBUS channel value to Sabertooth PWM value
int interpolation(uint16_t ch) {
    // Clamp channel value within SBUS range
    if (ch < SBUS_MIN) ch = SBUS_MIN;
    if (ch > SBUS_MAX) ch = SBUS_MAX;

    int pwm;
    if (ch < 992) {
        // Map SBUS value below 992 to reverse direction (1-63)
        pwm = (int)((ch - SBUS_MIN) * (SABERTOOTH_NEUTRAL - SABERTOOTH_MIN) / (992 - SBUS_MIN) + SABERTOOTH_MIN);
    } else {
        // Map SBUS value above 992 to forward direction (65-127)
        pwm = (int)((ch - 992) * (SABERTOOTH_MAX - SABERTOOTH_NEUTRAL) / (SBUS_MAX - 992) + SABERTOOTH_NEUTRAL);
    }

    // Ensure PWM value is within Sabertooth bounds
    if (pwm < SABERTOOTH_MIN) pwm = SABERTOOTH_MIN;
    if (pwm > SABERTOOTH_MAX) pwm = SABERTOOTH_MAX;

    return pwm;
}
