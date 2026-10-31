#include <Arduino.h>

// ---------- Pins ----------
#define LED1_PIN 25
#define LED2_PIN 26

// ---------- PWM settings ----------
const uint32_t FREQ1 = 1000;           // LED1: 1 kHz
const uint32_t FREQ2 = 5000;           // LED2: 5 kHz
const uint8_t  RES   = 8;              // 8-bit => 0..255
const int MAX_DUTY   = (1 << RES) - 1; // 255

#if ESP_ARDUINO_VERSION_MAJOR < 3
  #define CH1 0   // LEDC timer 0
  #define CH2 2   // LEDC timer 1 (alag timer = alag frequency)
#endif

// ---------- Fade state ----------
int duty1 = 0,        dir1 = 1;    // LED1 0 se upar jaye
int duty2 = MAX_DUTY, dir2 = -1;   // LED2 255 se neeche jaye

void writeDuty(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? CH1 : CH2, duty);
#endif
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
}

void loop() {
  // LED1 fade
  duty1 += dir1 * 5;
  if (duty1 >= MAX_DUTY) { duty1 = MAX_DUTY; dir1 = -1; }
  if (duty1 <= 0)        { duty1 = 0;        dir1 =  1; }

  // LED2 fade
  duty2 += dir2 * 5;
  if (duty2 >= MAX_DUTY) { duty2 = MAX_DUTY; dir2 = -1; }
  if (duty2 <= 0)        { duty2 = 0;        dir2 =  1; }

  writeDuty(1, duty1);
  writeDuty(2, duty2);
  delay(20);
}
