// Adafruit IO temp_DHT & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temp_DHT-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.


// Both DHT22 and the MPL115A2 can measure temperature,
// This sketch uses DHT22 and the MPL115A2 to sense and compare temperature 
// readings from the two sensors and outputs the result to OLED and Adafruit IO
// https://io.adafruit.com/willayang/dashboards/ice3
/************************** Configuration ***********************************/

#include "config.h"

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MPL115A2.h>

// initialize mpl115a2 sensor
Adafruit_MPL115A2 mpl115a2;

// oled display
Adafruit_SSD1306 oled = Adafruit_SSD1306(128, 32, &Wire);

// pin connected to DH22 data line
#define DATA_PIN 12

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT22);

// set up the 'temp_DHT' and 'humidity' feeds
AdafruitIO_Feed *temp_DHT = io.feed("temp_DHT");
AdafruitIO_Feed *temp_MPL = io.feed("temp_MPL");

void setup() {
  
  mpl115a2.begin(); //  starts sensing pressure
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display(); // make the screen display

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // text display tests
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  
  // Get temp_DHT event and print its value
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;

  Serial.print("Reading from DHT: ");
  Serial.print(celsius);
  Serial.println(" *C");

  // save fahrenheit (or celsius) to Adafruit IO
  temp_DHT->save(celsius);
  
  float celsius_MPL = 0;    

  celsius_MPL = mpl115a2.getTemperature();  
  Serial.print("Reading from MPL: "); Serial.print(celsius_MPL, 1); Serial.println(" *C");

  // save the reading from MPL to Adafruit IO
  temp_MPL->save(celsius_MPL);

  // print them to the OLED
  oled.clearDisplay(); // clear display
  oled.setCursor(0,0); // set cursor

  // print temperature reading from DHT
  oled.print("DHT Temp: "); oled.print(celsius,0); oled.println(" *C ");

  // print temperature reading from MPL, which is a little different
  oled.print("MPL Temp: "); oled.print(celsius_MPL,0); oled.println(" *C ");
  oled.display();

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(2000);
}
