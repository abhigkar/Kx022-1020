#include <Wire.h>
#include <KX022.h>
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#ifdef _VARIANT_ID107HR_
  #ifndef ACCEL_HWI2C
  #define USE_SOFTWAREI2C
  #endif
#else
//#define USE_SOFTWAREI2C
#endif

#ifdef USE_SOFTWAREI2C
#include <SoftwareI2C.h>
SoftwareI2C sWire(PIN_WIRE1_SDA, PIN_WIRE1_SCL);
KX022<SoftwareI2C> acc(sWire);
#else
KX022<> acc(Wire);
//KX022<TwoWire> acc(Wire); // TwoWire is the default class, so this is the same as above
#endif

#define OLED_WIDTH 64
#define OLED_HEIGHT 32
MicroOLED oled(OLED_RST, OLED_DC, OLED_CS);

float xyz[3];
uint32_t tPage;
bool button_wasPressed = false;
uint8_t page_num = 0;
const uint8_t page_count = 2;

void draw_page(uint8_t idx = 0);
void page_startup();
void page_active(uint8_t idx = 0);

bool Button_isPressed(uint8_t ButtonPin);
void float2chars(float &in, char (&out)[5]);

void setup()
{
  Serial.begin(9600);
  Serial.println(__FILE__);

  pinMode(PIN_BUTTON1, INPUT_PULLUP);

  acc.init();

  oled.setScreenSize(OLED_WIDTH, OLED_HEIGHT);
  oled.begin();
  oled.display();
  oled.setFontType(0);  // Set font to type 0
  page_startup();
  delay(2000); // show splash for 2s
  page_active();
  tPage = millis();
}

void loop()
{
  if (Button_isPressed(PIN_BUTTON1))
  {
    page_num = (page_num + 1 < page_count)?page_num+1:0;
  }
  
  if (millis() - tPage > 20) // 20ms = 50Hz
  {
    tPage = millis();
    draw_page(page_num);
  }
  yield();
}

// debounce the button
bool Button_isPressed(uint8_t ButtonPin)
{
  bool button_isPressed = (digitalRead(ButtonPin) == 0);

  bool result = (button_wasPressed == false) & (button_isPressed == true);
  button_wasPressed = button_isPressed;
  return result;
}

void float2chars(float &in, char (&out)[5])
{
  bool sign_bit = (in < 0);
  uint16_t tmp = sign_bit?(-in * 10):(in * 10);
  out[0] = (sign_bit)?'-':' ';
  out[1] = char('0' + (tmp / 10));
  out[2] = '.';
  out[3] = char('0' + (tmp % 10));
  out[4] = '\0';
}

void draw_page(uint8_t idx)
{
  page_active(idx);
}

void page_startup()
{
  oled.setCursor(0, 0);
  oled.print("github.com");
  oled.setCursor(0, 10);
  oled.print("/micooke");
  oled.setCursor(0, 20);
  oled.print(__TIME__);
  oled.display();
  oled.clear(SFEOLED_PAGE);
}

void page_active(uint8_t idx)
{
  if (idx == 0)
  {
    char fltBuf[5];
    
    acc.getAccelXYZ(xyz);
    
    float2chars(xyz[0],fltBuf);
    oled.setCursor(0, 0); oled.print("X:");
    oled.setCursor(10, 0); oled.print(fltBuf);
    float2chars(xyz[1],fltBuf);
    oled.setCursor(0, 10); oled.print("Y:");
    oled.setCursor(10, 10); oled.print(fltBuf);
    float2chars(xyz[2],fltBuf);
    oled.setCursor(0, 20); oled.print("Z:");
    oled.setCursor(10, 20); oled.print(fltBuf);

    #ifdef USE_SOFTWAREI2C
    oled.setCursor(50, 0); oled.print("SW");
    #else
    oled.setCursor(50, 0); oled.print("HW");
    #endif
    oled.setCursor(45, 10); oled.print("I2C");
    oled.display();
    oled.clear(SFEOLED_PAGE);
  }
  else
  {
    // freeze display
  }
}

