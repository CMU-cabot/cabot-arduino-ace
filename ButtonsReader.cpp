/*******************************************************************************
 * Copyright (c) 2020, 2022 Carnegie Mellon University, IBM Corporation, and others
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

#include "ButtonsReader.h"

ButtonsReader::ButtonsReader(ros::NodeHandle & nh, UartCom & cm)
: SensorReader(nh),
  b_pub_("pushed", &b_msg_),
  cm(cm)
{
  nh.advertise(b_pub_);
}

void ButtonsReader::init()
{
}

void ButtonsReader::update()
{
  bool reading_1 = cm.switch_up;
  bool reading_2 = cm.switch_down;
  bool reading_3 = cm.switch_left;
  bool reading_4 = cm.switch_right;
  bool reading_5 = cm.switch_center;

  b_msg_.data = reading_1 ? 0x01 : 0x00 |
    reading_2 ? 0x02 : 0x00 |
    reading_3 ? 0x04 : 0x00 |
    reading_4 ? 0x08 : 0x00 |
    reading_5 ? 0x10 : 0x00;

  b_pub_.publish(&b_msg_);
}
