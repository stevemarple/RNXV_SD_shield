#ifndef RNXV_H
#define RNXV_H
#include "Arduino.h"
#include "IPAddress.h"

#define RNXV_UNCONNECTED_PIN 0xff

#ifdef CALUNIUM
#define RNXV_DEFAULT_RTS 23
#define RNXV_DEFAULT_CTS 22
#define RNXV_DEFAULT_GPIO4 A1
#define RNXV_DEFAULT_GPIO5 5
#define RNXV_DEFAULT_GPIO6 8
#define RNXV_DEFAULT_GPIO8 9
#define RNXV_DEFAULT_CMD_PIN RNXV_UNCONNECTED_PIN

#elif defined(__AVR_ATmega1280__) || defined( __AVR_ATmega2560__)
#define RNXV_DEFAULT_RTS 15
#define RNXV_DEFAULT_CTS 14
#define RNXV_DEFAULT_GPIO4 A1
#define RNXV_DEFAULT_GPIO5 5
#define RNXV_DEFAULT_GPIO6 8
#define RNXV_DEFAULT_GPIO8 9
#define RNXV_DEFAULT_CMD_PIN RNXV_UNCONNECTED_PIN

#else
// Assume normal Arduino
#define RNXV_DEFAULT_RTS RNXV_UNCONNECTED_PIN
#define RNXV_DEFAULT_CTS RNXV_UNCONNECTED_PIN
#define RNXV_DEFAULT_GPIO4 A1
#define RNXV_DEFAULT_GPIO5 5
#define RNXV_DEFAULT_GPIO6 8
#define RNXV_DEFAULT_GPIO8 9
#define RNXV_DEFAULT_CMD_PIN RNXV_UNCONNECTED_PIN
#endif

#define RNXV_TIMEOUT_MS 4000

class RNXV {
 public:
  enum returnValues {
    exitOk = 0,
    errorCannotOpenFile,
    errorRtsPinNotSet,
    errorCtsPinNotSet,
    errorGpio4PinNotSet,
    errorGpio5PinNotSet,
    errorGpio6PinNotSet,
    errorGpio8PinNotSet,
    errorCmdPinNotSet,
    errorNotInCmdMode,
    errorTimeout,
    errorNotAssociated,
    errorConnectionFailed,
  };
    
  static const uint8_t unconnectedPin;
  RNXV(Stream& uartStream);

  inline returnValues getErrno(void) const {
    return errno;
  }

  inline void clearErrno(void) const {
    errno = exitOk;
  }
  
  inline bool getDebug(void) const {
    return debug;
  }

  inline void setDebug(bool myDebug = true) {
    debug = myDebug;
  }

  inline Stream& getUart(void) const {
    return uart;
  }

  inline Stream* getConsolePtr(void) const {
    return console;
  }

  inline void setConsole(Stream &stream) {
    console = &stream;
  }

  template<class T> inline Print& operator<<(T arg) const {
    uart.print(arg);
    return uart; 
  }
  
  template<class T> inline void print(T arg) const {
      uart.print(arg);
  }

  template<class T> inline void println(T arg) const {
      uart.println(arg);
  }

  inline void flush(void) const {
    uart.flush();
  }
  
  bool commandMode(void) const;
  bool uartCommandMode(void) const;
  bool gpioCommandMode(void) const;
  
  // RTS
  void setRtsPin(uint8_t p = RNXV_DEFAULT_RTS);
  bool rts(void) const;
  inline uint8_t getRtsPin(void) const {
    return rtsPin;
  }
  
  // CTS
  void setCtsPin(uint8_t p = RNXV_DEFAULT_CTS);
  bool cts(bool a) const;
  bool ctsWake(void) const;
  inline uint8_t getCtsPin(void) const {
    return ctsPin;
  }
  
  // GPIO4
  void setGpio4Pin(uint8_t p = RNXV_DEFAULT_GPIO4);
  bool isAssociated(void) const;
  inline uint8_t getGpio4Pin(void) const {
    return gpio4Pin;
  }

  // GPIO5
  void setGpio5Pin(uint8_t p = RNXV_DEFAULT_GPIO5);
  bool openConnection(void) const;
  bool closeConnection(void) const;
  inline uint8_t getGpio5Pin(void) const {
    return gpio5Pin;
  }
  
  // GPIO6
  void setGpio6Pin(uint8_t p = RNXV_DEFAULT_GPIO6);
  bool isConnected(void) const;
  inline uint8_t getGpio6Pin(void) const {
    return gpio6Pin;
  }

  // GPIO8
  void setGpio8Pin(uint8_t p = RNXV_DEFAULT_GPIO8);
  bool gpio8Sleep(void) const;
  inline uint8_t getGpio8Pin(void) const {
    return gpio8Pin;
  }
  
  bool sendCommand(const char* cmd) const;
  bool sendCommandsFromFile(const char* filename, char* buffer,
			      int bufferLen) const;
  bool connect(const char* hostname, uint16_t port,
	       uint16_t timeout_ms = RNXV_TIMEOUT_MS) const;
  bool connect(const IPAddress& ip, uint16_t port,
	       uint16_t timeout_ms = RNXV_TIMEOUT_MS) const;
  bool stop(void) const;

  // For Ethernet compatibility
  inline bool connected(void) const { return isConnected(); }

  // Command-mode pin. This can be any available GPIO that isn't being
  // used for something else
  void setCmdPin(uint8_t p = RNXV_DEFAULT_CMD_PIN);
  inline uint8_t getCmdPin(void) const {
    return cmdPin;
  }
  
  void showPinStatus(void) const;
  void consoleDebugger() const;
  
 private:
  mutable returnValues errno; // Error number
  uint8_t rtsPin;
  uint8_t ctsPin;
  uint8_t gpio4Pin;
  uint8_t gpio5Pin;
  uint8_t gpio6Pin;
  uint8_t gpio8Pin;
  uint8_t cmdPin;
  bool debug;
  Stream& uart;
  Stream* console; // use a pointer, might not have a console

  // Remove trailing CR, NL and other whitespace
  static void trimInput(char* str);
};

#endif
