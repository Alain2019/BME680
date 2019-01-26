/*******************************************************************************************************************
** Example program for using the Bosch BME680 sensor. The sensor measures temperature, pressure and humidity and  **
** is described at https://www.bosch-sensortec.com/bst/products/all_products/BME680. The datasheet is available   **
** from Bosch at https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME680_DS001-11.pdf           **
**                                                                                                                **
** The most recent version of the BME680 library is available at https://github.com/SV-Zanshin/BME680 and the     **
** documentation of the library as well as example programs are described in the project's wiki pages located at  **
** https://github.com/SV-Zanshin/BME680/wiki.                                                                     **
**                                                                                                                **
** The BME680 is a very small package so it is unlikely for an Arduino hobbyist to play around with directly, the **
** hardware used to develop this library is a breakout board from AdaFruit which is well-documented at            **
** https://learn.adafruit.com/adafruit-BME680-humidity-barometric-pressure-temperature-sensor-breakout.           **
**                                                                                                                **
** This example program initializes the BME680 to use hardware SPI for communications. The library does not use   **
** floating point mathematics to save on computation space and time, the values for Temperature, Pressure and     **
** Humidity are returned in deci-units, e.g. a Temperature reading of "2731" means "27.31" degrees Celsius. The   **
** display in the example program uses floating point for demonstration purposes only.  Note that the temperature **
** reading is generally higher than the ambient temperature due to die and PCB temperature and self-heating of    **
** the element.                                                                                                   **
**                                                                                                                **
** The pressure reading needs to be adjusted for altitude to get the adjusted pressure reading. There are numerous**
** sources on the internet for formula converting from standard sea-level pressure to altitude, see the data sheet**
** for the BME180 on page 16 of http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf. Rather than put a     **
** floating-point function in the library which may not be used but which would use space, an example altitude    **
** computation function has been added to this example program to show how it might be done.                      **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.0  2017-07-01 https://github.com/SV-Zanshin Cloned from original SPIDemo.ino program for BME280            **
**                                                                                                                **
*******************************************************************************************************************/
#include "Zanshin_BME680.h"                                                  // Include the BME680 Sensor library //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED            = 115200;                              // Set the baud rate for Serial I/O //
                                                                              // The pin used for slave-select can//
                                                                              // be freely chosen from the digital//
                                                                              // pins available. This is default  //
                                                                              // pin used on an Arduino MEGA2560  //
const uint8_t  SPI_CS_PIN              = 53;                                  // Pin for slave-select of BME680   //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
BME680_Class   BME680;                                                        // Create an instance of the BME680 //
/*******************************************************************************************************************
** Method altitude(). This converts a pressure measurement into a height in meters. The corrected sea-level       **
** pressure can be passed into the function if it is know, otherwise the standard atmosphereic pressure of        **
** 1013.25hPa is used (see https://en.wikipedia.org/wiki/Atmospheric_pressure                                     **
*******************************************************************************************************************/
float altitude(const float seaLevel=1013.25) {                                // Return computed altitude         //
  int32_t temp,hum,press,gas;                                                 // Temp variables to store a reading//
  BME680.getSensorData(temp,hum,press,gas);                                   // Get the most recent values       //
  float Altitude = 44330.0*(1.0-pow(((float)press/100.0)/seaLevel,0.1903));   // Convert into altitude in meters  //
  return(Altitude);                                                           // return computation result        //
} // of method altitude()                                                     //                                  //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    delay(3000);                                                              // wait 3 seconds for the serial    //
  #endif                                                                      // interface to initialize          //
  Serial.println(F("Starting Hardware SPIDemo example program for BME680"));  //                                  //
  Serial.print(F("- Initializing BME680 sensor\n"));                          //                                  //
                                                                              //==================================//
  while (!BME680.begin(SPI_CS_PIN)) {                                         // Start using hardware SPI protocol//
                                                                              //==================================//
    Serial.println(F("-  Unable to find BME680. Waiting 3 seconds."));        // Show error text                  //
    delay(3000);                                                              // wait three seconds               //
  } // of loop until device is located                                        //                                  //
  Serial.println(F("- Setting 16x oversampling for all sensors"));            //                                  //
  BME680.setOversampling(TemperatureSensor,Oversample16);                     // Use enumerated type values       //
  BME680.setOversampling(HumiditySensor,   Oversample16);                     //                                  //
  BME680.setOversampling(PressureSensor,   Oversample16);                     //                                  //
  Serial.println(F("- Setting IIR filter to maximum value of 16 samples"));   //                                  //
  BME680.setIIRFilter(IIR16);                                                 // Use enumerated type values       //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
  static int32_t temperature, humidity, pressure, gas;                        // Store readings                   //
  BME680.getSensorData(temperature,humidity,pressure,gas);                    // Get most recent readings         //
  Serial.print(temperature/100.0,2);                                          // Temperature in deci-degrees      //
#ifdef ESP32
  Serial.print(F(" ")); // Esp32 compiler doesn't liked escaped string
#else
  Serial.print(F("\xC2\xB0\C "));                          // Representation of the � symbol
#endif
  Serial.print(humidity/1000.0,2);                                            // Humidity in milli-percent        //
  Serial.print(F("%Hum "));                                                   //                                  //
  Serial.print(pressure/100.0,2);                                             // Pressure in Pascals              //
  Serial.print(F("hPa "));                                                    //                                  //
  Serial.print(altitude(),2);                                                 //                                  //
  Serial.print(F("m "));                                                      //                                  //
  Serial.print(gas/100.0,2);                                                  //                                  //
  Serial.println(F("mOhm"));                                                  //                                  //
  delay(5000);                                                                //                                  //
} // of method loop()                                                         //----------------------------------//
