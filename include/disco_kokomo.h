#ifndef _DISCO_KOKOMO_H_
#define _DISCO_KOKOMO_H_

#include <Arduino.h>

// music featherwing includes
#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

#include <Adafruit_NeoPixel.h>
#include <AccelStepper.h>

// music maker featherwing + amp
#define VS1053_RESET -1 // VS1053 reset pin not used
// feather M0 config
#define VS1053_CS 6 // VS1053 chip select pin (output)
#define VS1053_DCS 10 // VS1053 data/command select pin (output)
#define CARDCS 5 // card chip select pin
#define VS1053_DREQ 9 // VS1053 data request, ideally an interrupt pin

// neopixel jewel
#define LED_COUNT 7
#define LED_DATA_PIN 7

// 28BYJ-48 stepper motor
#define STEPS_PER_REV 2038

constexpr float RpmToSpeed(float rpm)
{
    return STEPS_PER_REV * rpm / 60.0; // [steps/s]
}

#define DOOR_SENSOR_PIN 15

enum class DoorState
{
    kOpen = 0,
    kClosed,
    kUnknown,
};

DoorState GetDoorState(uint32_t pin)
{
    int door_sensor_val = digitalRead(pin);
    if(door_sensor_val == HIGH) return DoorState::kClosed;
    else if(door_sensor_val == LOW) return DoorState::kOpen;
    return DoorState::kUnknown;
}

#endif