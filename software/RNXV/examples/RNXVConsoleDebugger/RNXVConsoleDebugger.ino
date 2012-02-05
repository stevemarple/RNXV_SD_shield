/*
 * RNXVConsoleDebugger.ino
 * Author: Steve Marple
 * License: GNU Lesser General Public License, version 2.1
 *
 * RNXVConsoleDebugger. A more user-friendly approach to explore and
 * debug RNXV behaviour than the RNXVLoopbackTest sketch. Use "$$$" to
 * enter command mode, any newline/carriage return characters are
 * automatically stripped. Begin RNXV commands with "$"; the leading
 * "$" is stripped and trailing whitespace (including newlines and
 * carriage returns) are removed,the command is automatically
 * terminated with carriage return to make sending commands more
 * convenient.
 *
 * Some of the member functions of the RNXV class can be called by
 * sending commands which begin with "!":
 *
 *   !
 *      Print the status of the GPIO pins.
 *   !gpio8Sleep
 *      Send the RNXV to sleep using the GPIO8 pin.
 *   !ctsWake
 *      Wake the RNXV by toggling the CTS pin.
 *   !cts 0
 *   !cts 1
 *      Clear/set the CTS pin.
 *   !openConnection
 *      Call openConnection() to open a stored connection using GPIO5. See
 *      "alternate GPIO functions" in the RN-XV manual. 
 *   !closeConnection
 *      Call closeConnection() to terminate a connection using GPIO5. See
 *      "alternate GPIO functions" in the RN-XV manual. 
 *   !commandMode()
 *      Call commandMode().
 *   !uartCommandMode()
 *      Call usartCommandMode().
 *   !gpioCommandMode()
 *      Call gpioCommandMode().
 *   !isConnected()
 *      Call isConnected() and print the result.
 *   !stop
 *      Call stop() to close a connection by sending the "close"
 *      command to the UART.
 *
 * Requirements:
 *   Arduino Mega(2560) or Calunium.
 *   RNXV library, see https://github.com/stevemarple/RNXV_SD_shield
 */

#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <IniFile.h>
#include <RNXV.h>

// RNXV UART is connected to Serial1. Adjust to suit your configuration.
RNXV rnxv(Serial1);

void setup(void)
{
  pinMode(SS, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);

  rnxv.setRtsPin();
  rnxv.setCtsPin();
  rnxv.setGpio4Pin();
  rnxv.setGpio5Pin(RNXV::unconnectedPin);
  rnxv.setGpio6Pin();
  rnxv.setGpio8Pin();

  // Use GPIO5 to enter command mode
  //rnxv.setCmdPin(RNXV_DEFAULT_GPIO5);
  rnxv.setCmdPin(5);
  rnxv.setConsole(Serial);
  
  // Reduce the timeout on Serial to 500ms
  Serial.setTimeout(500);
}

void loop(void)
{
  consoleDebugger();
}

