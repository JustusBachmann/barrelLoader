#ifndef CONFIG_H
#define CONFIG_H

const uint8_t ENCODER_CLK = 49;
const uint8_t ENCODER_DT = 51;
const uint8_t ENCODER_SW = 53;

const uint8_t DRV_DIR[] = {10, 8, 6, 28, 34}; // X, Y, Z, E0, E1
const uint8_t DRV_STEP[] = {11, 9, 7, 26, 36}; // X, Y, Z, E0, E1
const uint8_t DRV_ENABLE[] = {4, 2, 3, 24, 30}; // X, Y, Z, E0, E1
const uint8_t ENDSTOP[] = {14, 15, 16, 15}; // X, Y, Z, E0                                  ----- E0 = Y? 

const uint8_t MESA_OUT[] = {22, 23, 24};
const uint8_t MESA_IN[] = {29, 30, 31, 32};

const uint8_t RELAIS[] = {26, 27, 28};

const uint16_t HOMING_SPEED = 2500;
const uint16_t PROGRAM_SPEED = 1000;
const uint16_t ACCELERATION = 3000;

const uint8_t STEP_SIZE = 50; //200 steps = 360°
const uint16_t DELAY_BETWEEN_STEPS = 1000; //1000 = 1s
const uint8_t DELAY_READ_ENCODER = 2;

#define SCROLL_STEP 1

#endif