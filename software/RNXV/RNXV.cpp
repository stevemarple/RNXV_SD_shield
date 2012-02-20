#include "RNXV.h"

// Use the readLine() function from IniFile
#include "SD.h"
#include "IniFile.h"

const uint8_t RNXV::unconnectedPin = RNXV_UNCONNECTED_PIN;

RNXV::RNXV(Stream &uartStream) : uart(uartStream)
{
  // Mark input/output pins unconnected
  gpio4Pin = gpio5Pin = gpio6Pin = gpio8Pin = cmdPin = unconnectedPin;
  debug = false;
  errno = exitOk;
  console = NULL; // no console by default
}

bool RNXV::commandMode(void) const
{
  return uartCommandMode();
  
  if (cmdPin == unconnectedPin)
    return uartCommandMode();
  else {
    if (gpioCommandMode())
      return true;
    else
    return uartCommandMode();
  }
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

  uint8_t ctsPinOrigState;
  if (ctsPin != unconnectedPin) {
    ctsPinOrigState = digitalRead(ctsPin);
    digitalWrite(ctsPin, HIGH); // Don't allow new data to be sent
  }
  digitalWrite(cmdPin, HIGH);
  uart.flush();
  if (ctsPin != unconnectedPin)
    digitalWrite(ctsPin, LOW); // Allow sending of data
  
  uart.print('\r');
  // Read in '?', with a timeout
  char c;
  int i = uart.readBytes(&c, 1);
  digitalWrite(cmdPin, LOW);
  if (ctsPin != unconnectedPin)
    digitalWrite(ctsPin, ctsPinOrigState);

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
  if (gpio4Pin == unconnectedPin) {
    errno = errorGpio4PinNotSet;
    return true;
  }
  errno = exitOk;
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
    return true;
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
bool RNXV::gpio8Sleep(void) const
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
    console->println(cmd);
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
    if (console && debug) {
      console->print("sendCommandsFromFile(): cannot open ");
      console->println(filename);
    }
    errno = errorCannotOpenFile;
    return false;
  }

  bool ret = true;
  uint32_t pos = 0;
  while (1) {
    int8_t r = IniFile::readLine(file, buffer, bufferLen, pos);
    if (r < 0) {
      ret = false;
      break;
    }
    uart.flush();
    sendCommand(buffer);
    if (r)
      break;
  }
  file.close();
  uart.flush();
  errno = exitOk;
  return ret;
}

bool RNXV::connect(const char* hostname, uint16_t port, uint16_t timeout_ms) const
{
  uart.print("open ");
  uart.print(hostname);
  uart.print(' ');
  uart.print(port, DEC);
  uart.print('\r');
  if (console && debug) {
    console->print("open ");
    console->print(hostname);
    console->print(' ');
    console->println(port, DEC);
    // delay(50);
    // while (uart.available())
    //   console->print(char(uart.read()));
    // console->println();
  }
  unsigned long t = millis();
  while (!isConnected())
    if (millis() - t > timeout_ms) {
      errno = errorTimeout;
      return false;
    }

  errno = exitOk;
  return true;  
}

bool RNXV::connect(const IPAddress& ip, uint16_t port, uint16_t timeout_ms) const
{
  if (!isAssociated())
    return false; // errno already set
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
  unsigned long t = millis();
  while (!isConnected())
    if (millis() - t > timeout_ms) {
      errno = errorTimeout;
      return false;
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


void RNXV::consoleDebugger(void)
{
  if (console == NULL)
    return;

  const int consoleBufferLen = 80;
  char consoleBuffer[consoleBufferLen];
  const int uartBufferLen = 80;
  char uartBuffer[uartBufferLen];

  // Read from console
  if (console->available()) {
    int len = console->readBytesUntil('\n', consoleBuffer, consoleBufferLen-1);
    if (len) {
      consoleBuffer[len] = '\0';
      trimInput(&consoleBuffer[1]);
      if (consoleBuffer[0] == '$') {
	// Handle RNXV commands. These are sent with '\r' terminator
	if (consoleBuffer[1] == '$' && consoleBuffer[2] == '$') {
	  console->flush();
	  commandMode();
	}
	else { 
	  // Send some other command
	  sendCommand(&consoleBuffer[1]); // Omit leading $
	}
      }
      // Handle commands to read/modify pin status
      else if (consoleBuffer[0] == '!') {
	console->println(&consoleBuffer[0]); // Echo command
	console->print("!=> ");
	if (consoleBuffer[1] == '\0') {
	  showPinStatus();
	}
	else if (strcmp(consoleBuffer, "!gpio8Sleep") == 0) {
	  gpio8Sleep();
	}
	else if (strcmp(consoleBuffer, "!ctsWake") == 0) {
	  ctsWake();
	}
	else if (strcmp(consoleBuffer, "!cts 0") == 0) {
	  cts(0);
	}
	else if (strcmp(consoleBuffer, "!cts 1") == 0) {
	  cts(1);
	}
	else if (strcmp(consoleBuffer, "!openConnection") == 0) {
	  openConnection();
	}
	else if (strcmp(consoleBuffer, "!closeConnection") == 0) {
	  closeConnection();
	}
	else if (strcmp(consoleBuffer, "!commandMode") == 0) {
	  commandMode();
	}
	else if (strcmp(consoleBuffer, "!uartCommandMode") == 0) {
	  int8_t r = uartCommandMode();
	  console->print("!uartCommandMode -> ");
	  console->println(r, DEC);
	}
	else if (strcmp(consoleBuffer, "!gpioCommandMode") == 0) {
	  int8_t r = gpioCommandMode();
	  console->print("!gpioCommandMode -> ");
	  console->println(r, DEC);
	}
	else if (strcmp(consoleBuffer, "!isConnected") == 0) {
	  int8_t r = isConnected();
	  console->print("!isConnected -> ");
	  console->println(r, DEC);
	}
	else if (strcmp(consoleBuffer, "!setDebug 0") == 0) {
	  setDebug(0);
	}
	else if (strcmp(consoleBuffer, "!setDebug 1") == 0) {
	  setDebug(1);
	}
	else if (strncmp(consoleBuffer, "!sendCommandsFromFile ", 22) == 0) {
	  const int bufferLen = 100;
	  char buffer[bufferLen];
	  int8_t r = sendCommandsFromFile(&consoleBuffer[22],
					  buffer, bufferLen); 
	  console->print("!sendCommandsFromFile -> ");
	  console->print(r, DEC);
	  if (r)
	    console->println();
	  else {
	    console->print(" errno=");
	    console->println(errno);
	  }
	}
	else if (strcmp(consoleBuffer, "!stop") == 0) {
	  int8_t r = stop();
	  console->print("!stop -> ");
	  console->println(r, DEC);
	}
	else {
	  console->println("Unknown command");
	}
      }
      else
	uart.write((uint8_t*)consoleBuffer, len);
    }
  }

  // Read from uart
  if (uart.available()) {
    int len = uart.readBytes(uartBuffer, uartBufferLen-1);
    uartBuffer[len] = '\0';
    console->write((uint8_t*)uartBuffer, len);
  }
  return;
}

void RNXV::trimInput(char* str)
{
  for (int i = strlen(str) - 1; i >= 0; --i)
    if (isspace(str[i]))
      str[i] = '\0';
    else
      break;
}

