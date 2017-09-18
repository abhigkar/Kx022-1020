/*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*  KX022 Accelerometer library
*
*  @author  Goran Mahovlić
*  @version 1.0
*  @date    26-May-2017
*
*  @author  Mark Cooke
*  @version 1.1
*  @date    18-Sep-2017
*  @brief   Remove separate r and w addressing (didnt work for me)
*           Now allows change in accelerometer range and accel data output rate. 
*           New functions : getXYZ()
* 
*  Library for "KX022 Accelerometer library" from Kionix a Rohm group
*    http://www.kionix.com/product/KX022-1020
*/

#include "KX022.h"

void KX022::init(uint8_t range, uint8_t rate)
{
   _range = range;

   writeTwoBytes(KX022_CNTL1_REG,
                 (KX022_CNTL1_VALUE_STANDBY & ~KX022_ACC_OUTPUT_RATE_MASK) |
                     (range & KX022_ACC_OUTPUT_RATE_MASK));
   writeTwoBytes(KX022_ODCNTL_REG,
                 (KX022_ODCNTL_VALUE & ~KX022_ACC_OUTPUT_RATE_MASK) |
                     (rate & KX022_ACC_OUTPUT_RATE_MASK));
   writeTwoBytes(KX022_CNTL3_REG, KX022_CNTL3_VALUE);
   writeTwoBytes(KX022_TILT_TIMER_REG, KX022_TILT_TIMER_VALUE);

   // wait for registers to be set prior to coming out of standby = 1.2/ODR
   // (pg39 of the datasheet)
   delay((uint32_t)(1200.0f / KX022_ODR[rate]));
   writeTwoBytes(KX022_CNTL1_REG,
                 (KX022_CNTL1_VALUE_OPERATE & ~KX022_ACC_OUTPUT_RATE_MASK) |
                     (range & KX022_ACC_OUTPUT_RATE_MASK));
}

void KX022::writeTwoBytes(uint8_t one, uint8_t two)
{
   Wire.beginTransmission(_i2c_address);
   Wire.write(one);
   Wire.write(two);
   Wire.endTransmission();
}

uint8_t KX022::getByte(uint8_t reg_address)
{
   uint8_t value_ = 0;
   Wire.beginTransmission(_i2c_address);
   Wire.write(reg_address);
   Wire.endTransmission();
   Wire.requestFrom(_i2c_address, 1);
   if (Wire.available())
   {
      value_ = Wire.read();
   }
   return value_;
}

void KX022::getRawXYZ(int16_t xyz[3])
{
   Wire.beginTransmission(_i2c_address);
   Wire.write(DATA_OUT_BASE);
   Wire.endTransmission();
   Wire.requestFrom(_i2c_address, 6);
   if (Wire.available())
   {
      xyz[0] = static_cast<int16_t>(Wire.read() | (Wire.read() << 8));
      xyz[1] = static_cast<int16_t>(Wire.read() | (Wire.read() << 8));
      xyz[2] = static_cast<int16_t>(Wire.read() | (Wire.read() << 8));
   }
}

float KX022::getAccel(uint8_t channelNum)
{
   int16_t measurement =
       static_cast<int16_t>((getByte(DATA_OUT_BASE + 1 + 2 * channelNum) << 8) |
                            getByte(DATA_OUT_BASE + 2 * channelNum));
   return static_cast<float>(measurement) / KX022_ACC_SENSITIVITY[_range >> 3];
}

void KX022::getXYZ(float (&xyz)[3])
{
   int16_t xyz_[3] = {0};
   getRawXYZ(xyz_);
   xyz[0] = static_cast<float>(xyz_[0])  / KX022_ACC_SENSITIVITY[_range >> 3];
   xyz[1] = static_cast<float>(xyz_[1])  / KX022_ACC_SENSITIVITY[_range >> 3];
   xyz[2] = static_cast<float>(xyz_[2])  / KX022_ACC_SENSITIVITY[_range >> 3];
}