#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pins ----------
#define LED1_PIN 18
#define LED2_PIN 19
#define SDA_PIN  21
#define SCL_PIN  22

// ---------- PWM settings ----------
const uint32_t FREQ1 = 1000;           // LED1: 1 kHz
const uint32_t FREQ2 = 5000;           // LED2: 5 kHz
const uint8_t  RES   = 8;              // 8-bit => 0..255
const int MAX_DUTY   = (1 << RES) - 1; // 255

#if ESP_ARDUINO_VERSION_MAJOR < 3
  #define CH1 0   // LEDC timer 0
  #define CH2 2   // LEDC timer 1 (alag timer = alag frequency)
#endif

// ---------- OLED ----------
Adafruit_SSD1306 oled(128, 64, &Wire, -1);

// ---------- Fade state (OLED yehi values dikhati hai) ----------
int duty1 = 0,        dir1 = 1;
int duty2 = MAX_DUTY, dir2 = -1;

void writeDuty(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? CH1 : CH2, duty);
#endif
}

int toPercent(int duty) {
  return (duty * 100 + MAX_DUTY / 2) / MAX_DUTY;  // rounded %
}

void showOLED() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);

  oled.setCursor(0, 0);
  oled.println("PWM MONITOR");

  oled.setCursor(0, 24);
  oled.printf("LED1:%3d%% F:%luHz", toPercent(duty1), (unsigned long)FREQ1);

  oled.setCursor(0, 44);
  oled.printf("LED2:%3d%% F:%luHz", toPercent(duty2), (unsigned long)FREQ2);

  oled.display();
}

void setup() {
  Serial.begin(115200);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED1_PIN, FREQ1, RES);
  ledcAttach(LED2_PIN, FREQ2, RES);
#else
  ledcSetup(CH1, FREQ1, RES);
  ledcAttachPin(LED1_PIN, CH1);
  ledcSetup(CH2, FREQ2, RES);
  ledcAttachPin(LED2_PIN, CH2);
#endif

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found! Check wiring/address");
    while (true) delay(1000);
  }
  oled.clearDisplay();
  oled.display();
}

void loop() {
  duty1 += dir1 * 5;
  if (duty1 >= MAX_DUTY) { duty1 = MAX_DUTY; dir1 = -1; }
  if (duty1 <= 0)        { duty1 = 0;        dir1 =  1; }

  duty2 += dir2 * 5;
  if (duty2 >= MAX_DUTY) { duty2 = MAX_DUTY; dir2 = -1; }
  if (duty2 <= 0)        { duty2 = 0;        dir2 =  1; }

  writeDuty(1, duty1);   // LEDs ko yehi values mil rahi hain
  writeDuty(2, duty2);
  showOLED();            // OLED par wahi values dikh rahi hain
  delay(20);
}