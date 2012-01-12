#include "Arduino.h"


class RNXV {
 public: 
  RNXV();
  RNXV(Stream &stream);

  template<class T> inline Print& operator<<(T arg) const {
    uart.print(arg); return uart; 
  }
  
  template<class T> inline void print(T arg) const {
    uart.print(arg);
  }

  template<class T> inline void println(T arg) const {
    uart.println(arg);
  }

  inline Stream& getUart(void) const {
    return uart;
  }

  void commandMode(void) const;

  boolean configureFromFile(const char* filename) const;
  boolean initialiseFromFile(const char* filename) const;

  void setUart(Stream &stream);
    
  // GPIO4
  void setAssociatedPin(int8_t p = A1);
  boolean isAssociated(void) const;

  // GPIO5
  void setOpenConnectionPin(int8_t p = 5);
  boolean openConnection(void) const;
  boolean closeConnection(void) const;
  
  // GPIO6
  void setIsConnectedPin(int8_t p = 8);
  boolean isConnected(void) const;

  // setGPIO8pin();
  void setHardwareSleepPin(int8_t p = 9);
  boolean hardwareSleep(void);
  boolean hardwareWake(void);

 private:
  int8_t isAssociatedPin;
  int8_t openConnectionPin;
  int8_t isConnectedPin;
  int8_t hardwareSleepPin;
  // HardwareSerial &uart;
  Stream &uart;
};

