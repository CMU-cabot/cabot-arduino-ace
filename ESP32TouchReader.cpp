/*******************************************************************************
 * Copyright (c) 2020, 2024  Carnegie Mellon University, IBM Corporation, and others
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

#include "ESP32TouchReader.hpp"  // NOLINT

ESP32TouchReader::ESP32TouchReader(cabot::Handle & ch): SensorReader(ch){}

void ESP32TouchReader::init()
{
  initialized_ = true;
  diag_status_ = 0;
  diag_message_ = "working";
  count_ = 0;
}

void ESP32TouchReader::init(
  uint8_t touch_baseline, uint8_t touch_threshold,
  uint8_t release_threshold)
{
  ch_.loginfo("Touch initialized");
  initialized_ = true;
  touched_ = 0;
  touch_baseline_ = touch_baseline;
  touch_threshold_ = touch_threshold;
  release_threshold_ = release_threshold;
  diag_status_ = 0;
  diag_message_ = "working";
  count_ = 0;
}

void ESP32TouchReader::set_mode(uint8_t touch_baseline)
{
  ch_.loginfo("Touch ready");
}

void ESP32TouchReader::update()
{
  if (!initialized_) {
    return;
  }

  int16_t touch_raw = touchRead(T0);

  if (touched_ == 0){
    if ( touch_raw < touch_baseline_ - touch_threshold_ ){
      touched_ = 1;
    }
  }else{
    if ( touch_baseline_ - release_threshold_ < touch_raw ){
      touched_ = 0;
    }
  }

  check_touch_raw(touch_raw);

  // touch
  ch_.publish(0x10, (int16_t)touched_);
  // touch raw
  ch_.publish(0x11, touch_raw);
}

void ESP32TouchReader::diag_pub()
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

void ESP32TouchReader::check_touch_raw(int16_t touch_raw)
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
