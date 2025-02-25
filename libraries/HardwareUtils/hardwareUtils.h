// HardwareUtils.h
#ifndef HARDWARE_UTILS_H
#define HARDWARE_UTILS_H

#include <Arduino.h>

class HardwareUtils {
  public:
    static void setHigh(int pin) {
      digitalWrite(pin, HIGH);
    }

    static void setLow(int pin) {
      digitalWrite(pin, LOW);
    }

    static void initPins(const uint8_t* pins, uint8_t count, uint8_t mode) {
      for (uint8_t i = 0; i < count; i++) {
        pinMode(pins[i], mode);
      }
    }

    virtual bool detectButton() = 0;
    virtual int8_t detectScroll() = 0;
};

#endif // HARDWARE_UTILS_H
