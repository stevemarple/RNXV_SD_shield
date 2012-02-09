# RN-XV and SD shield

The RN-XV and SD shield is designed for the Arduino. It provides an
interface to the Roving Networks RN-XV Wifi module, which uses the
pseudo-standard XBee footprint. XBee modules will probably work 
although the circuitry is designed for the UART interface and GPIOs
of the RN-XV. The shield also provides an interface to an SD card and
the MicroMag3 magnetometer module by PNI. The shield can operate at
either 3.3V or 5V; logic-gate level shifters ensure that the inputs
*and* outputs of the RN-XV and SD card are only exposed to 3.3V signals.
For 5V operation the MicroMag module is powered at 5V.

The shield is fully functional on a standard Arduino (Duemilanove, 
Uno etc). Optional features such as flow control and the GPIO features
of the RN-XV require an Arduino Mega(2560) or Calunium
(https://github.com/stevemarple/Calunium). For more information see
http://blog.stevemarple.co.uk/2011/12/rn-xvsd-shield.html.

## Pin mapping

*   D4: SD select (if jumper JP3 fitted in LH position)
*   D5: RN-XV GPIO5 (if jumper JP5 fitted)
*   D8:  RN-XV GPIO6  (if jumper JP8 fitted)
*   D9:  RN-XV GPIO8 (if jumper JP6 fitted)
*   D11: MOSI (if jumper JP7 fitted)
*   D12: MISO (if jumper JP9 fitted)
*   D13: SCK (if jumper JP10 fitted)
*   D14 (Arduino Mega) or D22 (Calunium): RN-XV CTS (if jumper JP4 fitted)
*   D15 (Arduino Mega) or D23 (Calunium): RN-XV RTS (if jumper JP12 fitted)
*   A0: MicroMag SS
*   A1: RN-XV GPIO4 (if jumper JP11 fitted)
*   A2: LM61 temperature sensor
*   A3: MicroMag reset, LM61 power and heartbeat LED
*   A6: SD select (if jumper JP3 fitted in RH position)
*   A8 (Arduino Mega) or D14 (Calunium): MicroMag DRDY

## Jumper settings

*   JP1: Shield power supply; voltage regulator or 3.3V supply
*   JP2: Voltage regulator input; from +5V or Vin
*   JP3: SD chip select; D4 or A6
*   JP4: CTS flow control; D14 (Arduino Mega) or D22 (Calunium)
*   JP5: GPIO5; D5
*   JP6: GPIO8; D9
*   JP7: MOSI; link MOSI to D11
*   JP8: GPIO6; D8
*   JP9: MISO; link MISO to D12
*   JP10: SCK; link SCK to D13
*   JP11: GPIO4; A1
*   JP12: RTS flow control; D15 (Arduino Mega) or D23 (Calunium)
*   X2: TX/RX selection, any of D0,D1,D2,D3. Also D19/D18 (RX1/TX1) for
        Arduino Mega or D2/D3 (RX1/TX1) for Calunium


## Arduino library 

An Arduino library to take advantage of the various features is
provided.

## See also

*   Calunium: https://github.com/stevemarple/Calunium
*   MicroMag Arduino library: https://github.com/stevemarple/MicroMag

