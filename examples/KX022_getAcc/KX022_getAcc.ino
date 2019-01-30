#include <Wire.h>
#include <KX022.h>

//#define USE_SOFTWAREI2C
#ifdef USE_SOFTWAREI2C 
#include <SoftwareI2C.h>
SoftwareI2C sWire(14, 16);
KX022<SoftwareI2C> acc(sWire);
#else
KX022<> acc(Wire);
//KX022<TwoWire> acc(Wire); // TwoWire is the default class, so this is the same as above
#endif

uint32_t tPage;

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

void setup()
{
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println(__FILE__);

  pinMode(PIN_BUTTON1, INPUT_PULLUP);

  acc.init();

  tPage = millis();
}

void loop()
{
  if (millis() - tPage > 1000)
  {
    tPage = millis();
    char fltBuf[5];
    float xyz[3];
    
    acc.getAccelXYZ(xyz);
    
    float2chars(xyz[0],fltBuf);
    Serial.print("X:"); Serial.println(fltBuf);
    float2chars(xyz[1],fltBuf);
    Serial.print("Y:"); Serial.println(fltBuf);
    float2chars(xyz[2],fltBuf);
    Serial.print("Z:"); Serial.println(fltBuf);
  }
  yield();
}
