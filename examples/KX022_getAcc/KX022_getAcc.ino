/*
 * Simple 
 */

#include <KX022.h>
#include <SmartOled.h>

KX022 acc;
SmartOled my_oled(OLED_RST, OLED_DC, OLED_CS); // (pin_rst, pin_dc, pin_cs)

float xyz[3];

void setup() {
  Serial.begin(9600);
  Serial.println(__FILE__);

  // put your setup code here, to run once:
  acc.init();

  my_oled.init(0, 8000000);
  my_oled.clear(PAGE);
  my_oled.setCursor(0,0);
  my_oled.puts("github.com");
  my_oled.setCursor(0,10);
  my_oled.puts("/micooke");
  my_oled.display();
  delay(3000);
}

void loop() {
  acc.getXYZ(xyz);

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
