#ifndef CONFIG_H
#define CONFIG_H

#define ENCODER_CLK 22
#define ENCODER_DT 23
#define ENCODER_SW 3

const uint8_t MESA_OUT[] = {23, 24, 25};
const uint8_t MESA_IN[] = {29, 30, 31, 32};
const uint8_t RELAIS[] = {26, 27, 28};

const uint8_t NUM_STEPPERS = 3;
const uint8_t DRV_ENABLE[] = {12, 9, 6}; // X, Y, Z, E0, E1
const uint8_t ENDSTOP[] = {2, 18, 19};   // X, Y, Z, E0 ----- E0 = Y?
const uint8_t DRV_DIR[] = {10, 7, 4};  // X, Y, Z, E0, E1
const uint8_t DRV_STEP[] = {11, 8, 5}; // X, Y, Z, E0, E1

const uint8_t DELAY_READ_ENCODER = 2;

#define SCROLL_STEP 1

#endif