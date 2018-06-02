#include <Arduino.h>

// Constants
const int PIN_FAN = 11;
const int PIN_PUMP = 10;
const int PIN_SHUTDOWN = 13;
const int PIN_BUTTON = 3;
const int PIN_FLOAT = 2;

// Types
typedef enum CoolerMode_t {
  COOLER_MODE_OFF,
  COOLER_MODE_LOW,
  COOLER_MODE_MED,
  COOLER_MODE_HIGH
} CoolerMode;

typedef enum PWMSpeed_t {
    PWM_SPEED_LOW = 128,
    PWM_SPEED_MED = 192,
    PWM_SPEED_HIGH = 230,
    PWM_SPEED_PUMP = PWM_SPEED_MED
} PWMSpeed;

// Global Variables
CoolerMode currentMode;
CoolerMode newMode;
boolean isShutdown;

// Function Definitions
void shutdownCooler();
void runCooler(PWMSpeed);
void changeModeIsr();

void setup() {
    newMode = currentMode = COOLER_MODE_OFF;
    isShutdown = true;
    shutdownCooler();
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_FLOAT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), changeModeIsr, FALLING);
}

void loop() {
    // if (digitalRead(PIN_FLOAT) == HIGH) {
    //     newMode = COOLER_MODE_OFF;
    // }

    if (newMode != currentMode) {
        switch(newMode) {
            case COOLER_MODE_OFF:
                shutdownCooler();
                break;
            case COOLER_MODE_LOW:
                runCooler(PWM_SPEED_LOW);
                break;
            case COOLER_MODE_MED:
                runCooler(PWM_SPEED_MED);
                break;
            case COOLER_MODE_HIGH:
                runCooler(PWM_SPEED_HIGH);
                break;
        }

        currentMode = newMode;
    }

    delay(5000);
}

void shutdownCooler() {
    digitalWrite(PIN_SHUTDOWN, LOW);
    analogWrite(PIN_FAN, 0);
    analogWrite(PIN_PUMP, 0);
    isShutdown = true;
}

void runCooler(PWMSpeed fanSpeed) {
    analogWrite(PIN_PUMP, PWM_SPEED_PUMP);
    analogWrite(PIN_FAN, fanSpeed);

    if (isShutdown) {
        digitalWrite(PIN_SHUTDOWN, HIGH);
        isShutdown = false;
    }
}

void changeModeIsr() {
    switch(currentMode) {
        case COOLER_MODE_OFF:
            newMode = COOLER_MODE_LOW;
            break;
        case COOLER_MODE_LOW:
            newMode = COOLER_MODE_MED;
            break;
        case COOLER_MODE_MED:
            newMode = COOLER_MODE_HIGH;
            break;
        case COOLER_MODE_HIGH:
            newMode = COOLER_MODE_OFF;
            break;
    }
}
