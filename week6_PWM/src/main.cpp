#include <Arduino.h>

const int ledPin = 2;  // GPIO 2

// ESP32 PWM Properties
const int freq = 5000;      // PWM frequency in Hz
const int ledChannel = 0;   // Choose a PWM channel (0-15)
const int resolution = 8;   // 8-bit resolution (0-255)

void setup() {
  // Configure the PWM channel, frequency, and resolution
  ledcSetup(ledChannel, freq, resolution);
  
  // Attach the configured channel to the LED pin
  ledcAttachPin(ledPin, ledChannel);
}

void loop() {
  // Fade in
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledChannel, dutyCycle);
    delay(10);
  }

  // Fade out
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledChannel, dutyCycle);
    delay(10);
  }
}