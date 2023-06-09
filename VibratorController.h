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

#ifndef VIBRATORCONTROLLER_H_
#define VIBRATORCONTROLLER_H_

#include <Wire.h>
#include <std_msgs/UInt8.h>
#include "SensorReader.h"
#ifdef ESP32
#include <analogWrite.h>
#endif
#include "UartCom.h"

class VibratorController : public SensorReader
{
  ros::Subscriber<std_msgs::UInt8> vib1_sub_;
  ros::Subscriber<std_msgs::UInt8> vib2_sub_;
  ros::Subscriber<std_msgs::UInt8> vib3_sub_;
  ros::Subscriber<std_msgs::UInt8> vib4_sub_;
  UartCom & cm;

public:
  VibratorController(ros::NodeHandle & nh, UartCom & cm);
  void init();
  void update();
};

#endif  // VIBRATORCONTROLLER_H_
