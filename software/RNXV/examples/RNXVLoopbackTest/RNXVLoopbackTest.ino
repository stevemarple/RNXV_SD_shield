/*
 * RNXVLoopbackTest.ino
 * Author: Steve Marple
 * License: GNU Lesser General Public License, version 2.1
 *
 * Loopback test. Have the Arduino send serial data received from the
 * console to the RN-XV, and send any data received from the RN-XV to
 * the console.
 *
 * To enter command mode on the RN-XV send $$$ without no line
 * ending. Once command mode is enabled send commands with a carriage
 * return. The Arduino IDE console provides a line ending option.
 *
 * To use this sketch with a ATmega168 or ATmega324 Arduino you must
 * use NewSoftSerial to emulate a second UART.
 *
 * Requirements:
 *  Arduino Mega(2560) or Calunium.
 *
 */

#define SD_SELECT 4
#define MICROMAG3_SELECT A0


void setup()
{
    // Configure all of the SPI select pins as outputs and make SPI
  // devices inactive, otherwise the earlier init routines may fail
  // for devices which have not yet been configured.
  pinMode(SS, OUTPUT);
  digitalWrite(SS, 1);

  pinMode(SD_SELECT, OUTPUT);
  digitalWrite(SD_SELECT, 1);

  pinMode(MICROMAG3_SELECT, OUTPUT);
  digitalWrite(MICROMAG3_SELECT, 1);
  
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop()
{
  while (Serial.available()) {
    char c = Serial.read();
    Serial1.write(c);
  }

  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);
  }
}
