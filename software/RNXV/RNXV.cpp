#include "RNXV.h"

// Use the readLine() function from IniFile
#include "IniFile.h"

const uint8_t RNXV::unconnectedPin = RNXV_UNCONNECTED_PIN;

RNXV::RNXV(Stream &uartStream) : uart(uartStream)
{
  // Mark input/output pins unconnected
  gpio4Pin = gpio5Pin = gpio6Pin = gpio8Pin = cmdPin = unconnectedPin;
  debug = false;
  
  console = NULL; // no console by default
}

bool RNXV::commandMode(void) const
{
  if (cmdPin == unconnectedPin)
    return uartCommandMode();
  else
    return gpioCommandMode();
}

bool RNXV::uartCommandMode(void) const
{
  delay(250);
  uart.print("$$$");
  delay(250);
  // Send <CR> in case already in command mode.
  uart.print('\r');
  if (console && debug)
    console->println("$$$");
  errno = exitOk; 
  return true; // Have no easy means to check
}

bool RNXV::gpioCommandMode() const
{
  if (cmdPin == unconnectedPin) {
    errno = errorCmdPinNotSet; 
    return false;
  }

  if (console && debug)
    console->println("cmd_GPIO");

  digitalWrite(cmdPin, HIGH);
  uart.print('\r');
  // Read in '?', with a timeout
  char c;
  int i = uart.readBytes(&c, 1);
  digitalWrite(cmdPin, LOW);
  if (i == 0) {
    errno = errorTimeout; 
    return false;
  }
  if (c == '?') {
    errno = exitOk; 
    return true;
  }
  errno = errorNotInCmdMode;
  return false;
}

void RNXV::setRtsPin(uint8_t pin)
{
  rtsPin = pin;
  if (rtsPin != unconnectedPin)
    pinMode(rtsPin, INPUT);
}

// High indicates receiver buffer full, so inver the return value
bool RNXV::rts(void) const
{
  if (rtsPin == unconnectedPin)
    return true; // Can't tell, assume it is
  return !digitalRead(rtsPin);
}

void RNXV::setCtsPin(uint8_t pin)
{
  ctsPin = pin;
  if (ctsPin != unconnectedPin)
    pinMode(ctsPin, OUTPUT);
}

// Low on RN-XV CTS pin enables transmitter so invert the state
bool RNXV::cts(bool a) const
{
  if (ctsPin == unconnectedPin)
    return false; 
  digitalWrite(ctsPin, !a);
  return true;
}

// Toggle the state of the 
bool RNXV::ctsWake(void) const
{
  if (ctsPin == unconnectedPin)
    return false; 
  uint8_t orig = digitalRead(ctsPin);
  digitalWrite(ctsPin, !orig);
  delayMicroseconds(1);
  digitalWrite(ctsPin, orig);
  return true;
}

void RNXV::setGpio4Pin(uint8_t pin)
{
  gpio4Pin = pin;
  if (gpio4Pin != unconnectedPin)
    pinMode(gpio4Pin, INPUT);
}

bool RNXV::isAssociated(void) const
{
  if (gpio4Pin == unconnectedPin)
    return false;
  return digitalRead(gpio4Pin);
}

// GPIO5
void RNXV::setGpio5Pin(uint8_t pin)
{
  gpio5Pin = pin;
  if (gpio5Pin != unconnectedPin)
    pinMode(gpio5Pin, OUTPUT);
}

bool RNXV::openConnection(void) const
{
  if (gpio5Pin == unconnectedPin)
    return false;
  digitalWrite(gpio5Pin, HIGH);
  return true;
}

bool RNXV::closeConnection(void) const
{
  if (gpio5Pin == unconnectedPin)
    return false;
  digitalWrite(gpio5Pin, LOW);
  return true;
}

// GPIO6
void RNXV::setGpio6Pin(uint8_t pin)
{
  gpio6Pin = pin;
  if (gpio6Pin != unconnectedPin) {
    pinMode(gpio6Pin, INPUT);
    digitalWrite(gpio6Pin, LOW);
  }
}

bool RNXV::isConnected(void) const
{
  if (gpio6Pin == unconnectedPin) {
    errno = errorGpio6PinNotSet;
    return false;
  }
  errno = exitOk;
  return digitalRead(gpio6Pin);
}

void RNXV::setGpio8Pin(uint8_t pin)
{ 
  gpio8Pin = pin;
  if (gpio8Pin != unconnectedPin) {
    pinMode(gpio8Pin, OUTPUT);
    digitalWrite(gpio8Pin, LOW);
  }
}

void RNXV::setCmdPin(uint8_t pin)
{ 
  cmdPin = pin;
  if (cmdPin != unconnectedPin) {
    // The command pin must change LOW-HIGH-LOW; initialise LOW
    pinMode(cmdPin, OUTPUT);
    digitalWrite(cmdPin, LOW);
  }
}

// Sleep using GPIO8 requires a rising edge, return to low level afterwards
bool RNXV::gpio8Sleep(void)
{
  if (gpio8Pin == unconnectedPin) {
    errno = errorGpio8PinNotSet;
    return false;
  }
  digitalWrite(gpio8Pin, HIGH);
  delayMicroseconds(1);
  digitalWrite(gpio8Pin, LOW);
  return true;
}


bool RNXV::sendCommand(const char* cmd) const
{
  uart.print(cmd);
  uart.print('\r');
  if (console && debug) {
    console->print(cmd);
    console->println();
    delay(50);
    while (uart.available())
      console->print(char(uart.read()));
    console->println();
  }
  errno = exitOk;
  return true;  
}

bool RNXV::sendCommandsFromFile(const char* filename, char* buffer,
				  int bufferLen) const
{
  File file = SD.open(filename);
  if (!file) {
    errno = errorCannotOpenFile;
    return false;
  }
  uint32_t pos = 0;
  while (IniFile::readLine(file, buffer, bufferLen, pos) == 0) {
    sendCommand(buffer);
  }
  file.close();
  errno = exitOk;
  return true;
}

bool RNXV::connect(const char* hostname, uint16_t port) const
{
  uart.print("open ");
  uart.print(hostname);
  uart.print(' ');
  uart.print(port, DEC);
  uart.print('\r');
  if (console && debug) {
    uart.print("open ");
    uart.print(hostname);
    uart.print(' ');
    uart.println(port, DEC);
    delay(50);
    while (uart.available())
      console->print(char(uart.read()));
    console->println();
  }
  errno = exitOk;
  return true;  
}

bool RNXV::connect(const IPAddress& ip, uint16_t port) const
{
  uart.print("open ");
  uart.print(ip);
  uart.print(' ');
  uart.print(port, DEC);
  uart.print('\r');
  if (console && debug) {
    uart.print("open ");
    uart.print(ip);
    uart.print(' ');
    uart.println(port, DEC);
    delay(50);
    while (uart.available())
      console->print(char(uart.read()));
    console->println();
  }
  errno = exitOk;
  return true;  
}

bool RNXV::stop(void) const
{
  return sendCommand("close");
}


void RNXV::showPinStatus(void) const
{
  if (console == NULL)
    return;

  console->print("RTS: ");
  if (rtsPin == unconnectedPin) 
    console->print('?');
  else
    console->print(digitalRead(rtsPin));

  console->print(",  CTS: ");
  if (ctsPin == unconnectedPin) 
    console->print('?');
  else
    console->print(digitalRead(ctsPin));

  console->print(",  is associated: ");
  if (gpio4Pin == unconnectedPin) 
    console->print('?');
  else
    console->print(isAssociated());
  
  console->print(",  connect: ");
  if (gpio5Pin == unconnectedPin) 
    console->print('?');
  else
    console->print(digitalRead(gpio5Pin));
  
  console->print(",  is connected: ");
  if (gpio6Pin == unconnectedPin) 
    console->println('?');
  else
    console->println(isConnected());
}
