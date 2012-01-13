#include "RNXV.h"


RNXV::RNXV() : uart(Serial)
{
  // Mark input/output pins unused
  isAssociatedPin = openConnectionPin = isConnectedPin = hardwareSleepPin = -1;
}

//RNXV::RNXV(HardwareSerial &s) : uart(s)

RNXV::RNXV(Stream &stream) : uart(stream)
{
  // Mark input/output pins unused
  isAssociatedPin = openConnectionPin = isConnectedPin = hardwareSleepPin = -1;
}

/*
void RNXV::print(const char* s) const
{
  uart.print(s);
}
 
void println(const char* s) const
{
  uart.println(s);
}
*/

void RNXV::commandMode(void) const
{
  delay(250);
  uart.print("$$$");
  delay(250);
  // Send <CR> in case already in command mode.
  uart.print('\r');
}

void RNXV::setUart(Stream &stream)
{
  uart = stream;
}

void RNXV::setAssociatedPin(int8_t pin)
{
  isAssociatedPin = pin;
  if (isAssociatedPin != -1)
    pinMode(isAssociatedPin, INPUT);
}

boolean RNXV::isAssociated(void) const
{
  if (isAssociatedPin == -1)
    return false;
  return digitalRead(isAssociatedPin);
}

// GPIO5
void RNXV::setOpenConnectionPin(int8_t pin)
{
  openConnectionPin = pin;
  if (openConnectionPin != -1)
    pinMode(openConnectionPin, OUTPUT);
}

boolean RNXV::openConnection(void) const
{
  if (openConnectionPin == -1)
    return false;
  digitalWrite(openConnectionPin, HIGH);
  return true;
}

boolean RNXV::closeConnection(void) const
{
  if (openConnectionPin == -1)
    return false;
  digitalWrite(openConnectionPin, LOW);
  return true;
}

// GPIO6
void RNXV::setIsConnectedPin(int8_t pin)
{
  isConnectedPin = pin;
  if (isConnectedPin != -1) {
    pinMode(isConnectedPin, INPUT);
    digitalWrite(isConnectedPin, LOW);
  }
}

boolean RNXV::isConnected(void) const
{
  if (isConnectedPin == -1)
    return false;
  return digitalRead(isConnectedPin);
}

void RNXV::setHardwareSleepPin(int8_t pin)
{ 
  hardwareSleepPin = pin;
  if (hardwareSleepPin != -1) {
    pinMode(hardwareSleepPin, OUTPUT);
    digitalWrite(hardwareSleepPin, LOW);
  }
}

boolean RNXV::hardwareSleep(void)
{
  if (hardwareSleepPin == -1)
    return false;
  digitalWrite(hardwareSleepPin, HIGH);
  return true;
}

boolean RNXV::hardwareWake(void)
{
  if (hardwareSleepPin == -1)
    return false;
  digitalWrite(hardwareSleepPin, LOW);
  return true;
}


