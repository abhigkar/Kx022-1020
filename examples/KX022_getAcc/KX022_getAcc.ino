/*
 * Simple 
 */
#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <KX022.h>
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#if (PIN_WIRE_SDA == 14)
KX022<> acc(Wire);
//KX022<TwoWire> acc(Wire); // TwoWire is the default class, so this is the same as above
#else
#include <SoftwareI2C.h>
SoftwareI2C sWire(14, 16);
KX022<SoftwareI2C> acc(sWire);
#endif

#define OLED_WIDTH 64
#define OLED_HEIGHT 32
MicroOLED my_oled(OLED_RST, OLED_DC, OLED_CS); // (pin_rst, pin_dc, pin_cs)

float xyz[3];

void setup() {
  Serial.begin(9600);
  Serial.println(__FILE__);

  // put your setup code here, to run once:
  acc.init();
  
  my_oled.setScreenSize(OLED_WIDTH, OLED_HEIGHT);
  my_oled.begin();
  my_oled.clear(PAGE);
  my_oled.setCursor(0,0);
  my_oled.print("github.com");
  my_oled.setCursor(0,10);
  my_oled.print("/micooke");
  my_oled.display();
  delay(3000);
}

void loop() {
  acc.getAccelXYZ(xyz);

  Serial.print(xyz[0]); Serial.print(",");
  Serial.print(xyz[1]); Serial.print(",");
  Serial.println(xyz[2]);
  
  my_oled.clear(PAGE);
  my_oled.setCursor(0,0);
  my_oled.printf("X:%f",xyz[0]);
  my_oled.setCursor(0,10);
  my_oled.printf("Y:%0.1f",xyz[1]);
  my_oled.setCursor(0,20);
  my_oled.printf("Z:%0.1f",xyz[2]);
  my_oled.display();

  yield();
}