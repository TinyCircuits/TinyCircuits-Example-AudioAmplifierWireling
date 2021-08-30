#include "sample.raw.h"
#include "SAMDTimerInterrupt.h"
#include <Wire.h>
#include <Wireling.h>

// DEFINES: DAC output pin and audio sample's sample rate (Hz)
#define DAC_PIN A0
#define AUDIO_SAMPLE_RATE_HZ 48000

// Related to getting sampels from converted raw file
volatile unsigned long int sampleIndex = 0;
volatile unsigned long int sampleCount = sizeof(sample_raw)/sizeof(sample_raw[0]);

// Timers for triggering ISR and playing samples
SAMDTimer ITimer0(TIMER_TC3);
const unsigned long int TIMER0_INTERVAL_US = (int)((1.0f / (float)AUDIO_SAMPLE_RATE_HZ) * 1000000.0f);

// Volume to play sample at: 0.0 = min, 1.0 = max
float volume = 1.0f;



// Play the samples on timer interrupt
void TimerHandler0(void){
  // Write sample value to DAC by scaling 8-bit 0-255 to 10-bit 0-1023 and incorporate volume
  analogWrite(DAC_PIN, (int)((sample_raw[sampleIndex] << 2) * volume));

  // Increase counter so next sample retrieved when ISR triggers again
  sampleIndex++;

  // Loop sample when reach end
  if(sampleIndex >= sampleCount){
    sampleIndex = 0;
  }
}


void setup(){
  // Start serial
  SerialUSB.begin(115200);

  // Start I2C and Wireling library
  Wire.begin();
  Wireling.begin();

  // Set analog out resolution to max, 10-bits (SAMD21 DAC is 10-bit)
  analogWriteResolution(10);

  // A small amount of delay, just in case
  delay(100);

  // Print timer adn processor information
  SerialUSB.print(F("\nStarting TimerInterruptTest on ")); Serial.println(BOARD_NAME);
  SerialUSB.println(SAMD_TIMER_INTERRUPT_VERSION);
  SerialUSB.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

  // Setup the timer and ISR, interval in microsecs (us)
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_US, TimerHandler0))
    Serial.println("Starting  ITimer0 OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer0. Select another freq. or timer");
}


// Loop doesn't do anything, all handled in TimerHandler0 ISR 
void loop() {
}
