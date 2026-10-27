#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_PIN 18
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const unsigned long DEBOUNCE_MS = 50;

int lastRawState = HIGH;        // last raw reading
int stableState = HIGH;         // debounced (accepted) state
unsigned long lastChangeTime = 0;
int pressCount = 0;

void updateDisplay()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("ESP32 Button Counter");

    display.setCursor(0, 20);
    display.print("Button: ");
    display.println(stableState == LOW ? "PRESSED" : "RELEASED");

    display.setTextSize(2);
    display.setCursor(0, 40);
    display.print("Count: ");
    display.println(pressCount);

    display.display();
}

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Wire.begin(21, 22);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
    {
        Serial.println("OLED not found!");
        while (true);
    }
    updateDisplay();
    Serial.println("System ready");
}

void loop()
{
    int rawState = digitalRead(BUTTON_PIN);

    // Raw reading badli to debounce timer reset karo
    if (rawState != lastRawState)
    {
        lastChangeTime = millis();
        lastRawState = rawState;
    }

    // Reading 50 ms stable rahi aur accepted state se different hai
    if ((millis() - lastChangeTime) >= DEBOUNCE_MS && rawState != stableState)
    {
        stableState = rawState;

        if (stableState == LOW)      // valid press
        {
            pressCount++;
            Serial.print("Accepted press count: ");
            Serial.println(pressCount);
        }
        updateDisplay();
    }
}