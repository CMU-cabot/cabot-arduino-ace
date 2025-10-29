/*******************************************************************************
 * Copyright (c) 2020, 2023  Carnegie Mellon University, IBM Corporation, and others
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#include "TouchReader.hpp"  // NOLINT

TouchReader::TouchReader(cabot::Handle & ch, uart_com & cm)
: SensorReader(ch), cm(cm) {}

void TouchReader::init()
{
  if (!cap_.begin(0x5A)) {
    ch_.loginfo("Ooops, no MPR121 detected ... Check your wiring or I2C ADDR!");
    return;
  }
  set_mode(128);

  initialized_ = true;
  is_continuous_ = false;
  diag_status_ = 0;
  diag_message_ = "working";
  count_ = 0;
}

void TouchReader::init(
  uint8_t touch_baseline, uint8_t touch_threshold,
  uint8_t release_threshold)
{

  if (!cap_.begin(0x5A, &Wire, touch_threshold, release_threshold)) {
    ch_.loginfo("Ooops, no MPR121 detected ... Check your wiring or I2C ADDR!");
    return;
  }
  set_mode(touch_baseline);

  ch_.loginfo("Touch initialized");
  initialized_ = true;
  is_continuous_ = false;
  diag_status_ = 0;
  diag_message_ = "working";
  count_ = 0;
}

void TouchReader::set_mode(uint8_t touch_baseline)
{
    // stop mode
  cap_.writeRegister(MPR121_ECR, 0b00000000);
  // set baseline to 128 ( do not remove bit shift)
  cap_.writeRegister(MPR121_BASELINE_0, touch_baseline >> 2);
  // use only pin 0
  cap_.writeRegister(MPR121_ECR, 0b01000001);

  ch_.loginfo("Touch ready");
}

void TouchReader::update()
{
  if (!initialized_) {
    return;
  }
  int touched = cap_.touched();

  int16_t touch_raw = (int16_t)cap_.filteredData(0);

  check_touch_raw(touch_raw);

  // touch
  ch_.publish(0x10, (int16_t)touched);
  // TBR
  ch_.publish(0x11, touch_raw);
  // vel
  // ch_.publish(0x01, (int16_t)(touched & 0x01) ? 2.0 : 0);
}

void TouchReader::diag_pub()
{
  size_t len = diag_message_.length() + 1;
  uint8_t combinedData[len];
  int index = 0;

  combinedData[index++] = diag_status_;

  for(char c : diag_message_) {
    if (index < len) {
      combinedData[index++] = static_cast<uint8_t>(c);
    }
  }
  ch_.publish(0x30, combinedData, len);
}

void TouchReader::check_touch_raw(int16_t touch_raw)
{
  if (touch_raw == 0) {
    count_++;
      if (count_ > 100) {
        diag_status_ = 2; //error
        diag_message_ = "touch_raw is constant at 0";
      }
  } else {
      diag_status_ = 0;
      count_ = 0;
      diag_message_ = "working";
  }
}