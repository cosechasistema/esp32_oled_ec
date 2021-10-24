/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
 * Copyright (c) 2018 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ThingPulse invests considerable time and money to develop these open source libraries.
 * Please support us by buying our products (and not the clones) from
 * https://thingpulse.com
 * ---- > https://github.com/ThingPulse/esp8266-oled-ssd1306
 */

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include <JC_Button.h>
#include "Relay.h"

#define DS18B20PIN 16
#define RELAYUNOPIN 21
Relay relayuno(2, RELAYUNOPIN); // constructor receives (pin, isNormallyOpen) true = Normally Open, false = Normally Closed

// para el oled como usarlo https://github.com/ThingPulse/esp8266-oled-ssd1306

// NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// defines variables
long duration;
int distance;
float temperatureC;

// ejemplo https://github.com/DevCorTec/OLED_I2C_ARDUINO_ONEBUTTON/blob/master/OLED_128x64_MENU-EXAMPLE_20170919_2210.ino

int pic = 0;
int maxPics_L1 = 5;
int maxPics_L2 = 3;
long lastmillis_pic = 0;
long maxtime_pic = 30000;

// #ifdef BOARDwemosbat
// Pin definition

#define led_Pin 23
#define pin_neopixel 17
#define led_neopixel_count 1
#define BUTTON_PIN 22
// Para genearar un build-->  platformio run -e wemosbat
// Para Pasarlo a la Placa --> platformio run -e wemosbat --target upload
// Para Limpiar todo --> platformio run --target clean

// #endif

// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);
// SCL - Pin 4
// SDA - Pin 5
// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, 5, 4);
// SH1106 display(0x3c, D3, D5);

/* Create an instance of OneWire */
OneWire oneWire(DS18B20PIN);

DallasTemperature sensor(&oneWire);
// *********************************
// * Manejo del Boton
// *********************************

Button myBtn(BUTTON_PIN); // define the button

// *********************************
// * Manejo del neopixel
// *********************************
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
Adafruit_NeoPixel strip = Adafruit_NeoPixel(led_neopixel_count, pin_neopixel, NEO_GRB + NEO_KHZ800);

//************************
//** F U N C I O N E S ***
//************************
void TodoUnColor(int xr, int xg, int xb);

// manejo del menu solo pasa por las patallas no hay enter solo ve

void select_pic(int xpic);
void back_to_principal_menu();
void click_menu();

void setup()
{
  display.init();
  Serial.begin(115200);
  // display.flipScreenVertically();

  display.setContrast(255);

  // display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  sensor.begin();
  strip.begin();
  strip.show();     // Initialize all pixels to 'off'
  select_pic(0);    // para mostrar el logo
  relayuno.begin(); // inicializes the pin
}

void loop()
{

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  sensor.requestTemperatures();
  temperatureC = sensor.getTempCByIndex(0);

  back_to_principal_menu();

  //  display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.drawString(64, 22, "Center aligned (64,22)");

  Serial.print("Ping: ");
  // Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");

  static bool ledState; // a variable that keeps the current LED status
  myBtn.read();         // read the button

  if (myBtn.wasReleased()) // if the button was released, change the LED state
  {
    ledState = !ledState;
    display.drawString(0, 0, "boton " + String(ledState) + " <---");
    click_menu();
    if (ledState)
    {
      relayuno.turnon();
    }
    else
    {
      relayuno.turnoff();
    }
  }
  if (ledState)
  {
    TodoUnColor(240, 248, 255);
  }
  else
  {
    TodoUnColor(0, 150, 0);
  }

  // delay(5000);
}

void TodoUnColor(int xr, int xg, int xb)
{
  strip.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for (int i = 0; i < led_neopixel_count; i++)
  { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:// g 155
    strip.setPixelColor(i, strip.Color(xr, xg, xb));

    strip.show();
  }
}

void select_pic(int xpic)
{

  if (xpic == 0)
  {
    display.clear(); // limpio la pantalla
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 10, "Imagen o algo lindo ");
    display.drawString(0, 20, "Vuelve a esta pantalla los 30 segundos ");
    display.display(); // muestro la pantalla
  }

  if (xpic == 1)
  {
    display.clear(); // limpio la pantalla
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Muestra los pines Utilizados: " + String(millis()));
    display.drawString(0, 10, "PIN neo: " + String(pin_neopixel) + "PIN boton: " + String(BUTTON_PIN));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 22, "--> C " + String(temperatureC) + " <---");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 30, "pin " + String(DS18B20PIN) + " <---");
    display.display(); // muestro la pantalla
  }

  if (xpic == 2)
  {
    display.clear(); // limpio la pantalla
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 10, "PARAMETERS ");
    display.drawString(0, 20, ">DATA RECORD");
    display.drawString(0, 30, " DISPLAY ");
    display.drawString(0, 40, " HELP ");
    display.drawString(0, 50, " PROJECT INFO");
    display.display(); // muestro la pantalla
  }

  if (xpic == 3)
  {
    display.clear(); // limpio la pantalla
    display.drawString(0, 10, " PARAMETERS ");
    display.drawString(0, 20, " DATA RECORD");
    display.drawString(0, 30, ">DISPLAY ");
    display.drawString(0, 40, " HELP ");
    display.drawString(0, 50, " PROJECT INFO");
    display.display(); // muestro la pantalla
  }
  if (xpic == 4)
  {
    display.clear(); // limpio la pantalla
    display.drawString(0, 10, " PARAMETERS ");
    display.drawString(0, 20, " DATA RECORD");
    display.drawString(0, 30, " DISPLAY ");
    display.drawString(0, 40, ">HELP ");
    display.drawString(0, 50, " PROJECT INFO");
    display.display(); // muestro la pantalla
  }

  if (xpic == 5)
  {
    display.clear(); // limpio la pantalla
    display.drawString(0, 10, " PARAMETERS ");
    display.drawString(0, 20, " DATA RECORD");
    display.drawString(0, 30, " DISPLAY ");
    display.drawString(0, 40, " HELP ");
    display.drawString(0, 50, ">PROJECT INFO");
    display.display(); // muestro la pantalla
  }
}

void back_to_principal_menu()
{
  // vuelvo amenu principal si para determinado tiempo
  // JUMP TO DEFAULT IF NO CLICK IS DETECTED
  if (millis() >= (lastmillis_pic + maxtime_pic))
  {
    select_pic(1);
  }
}
void click_menu()
{
  // se hizo click en el boto de menues
  lastmillis_pic = millis();
  if (pic >= 0 && pic < 10)
  {
    if (pic >= maxPics_L1)
    {
      pic = 1;
      select_pic(pic);
    }
    else if (pic < maxPics_L1)
    {
      pic++;
      select_pic(pic);
    }
  }
}