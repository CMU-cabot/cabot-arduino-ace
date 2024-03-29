/*******************************************************************************
 * Copyright (c) 2020, 2023  Carnegie Mellon University and Miraikan
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

#include "WiFiReader.hpp"  // NOLINT
#include <algorithm>

extern void restart();

WiFiReader::WiFiReader(cabot::Handle & ch)
: SensorReader(ch)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void WiFiReader::init(void (* callback)(char *))
{
  callback_ = callback;
  init();
}

void WiFiReader::init()
{
  verbose_int = verbose;
  if (!ch_.getParam("~verbose", &verbose_int, 1, PARAM_TIMEOUT)) {
    verbose = DEFAULT_VERBOSITY;
  }
  if (!ch_.getParam("~max_skip", &max_skip, 1, PARAM_TIMEOUT)) {
    max_skip = DEFAULT_MAX_SKIP;
  }
  if (!ch_.getParam("~n_channel", &n_channel, 1, PARAM_TIMEOUT)) {
    n_channel = DEFAULT_N_CHANNEL;
  }
  if (!ch_.getParam("~scan_duration", &scan_duration, 1, PARAM_TIMEOUT)) {
    scan_duration = DEFAULT_SCAN_DURATION;
  }
  if (!ch_.getParam("~scan_interval", &scan_interval, 1, PARAM_TIMEOUT)) {
    scan_interval = DEFAULT_SCAN_INTERVAL;
  }

  // init internal state
  scanningStart = millis();
  memset(skip, 0, sizeof(skip));
  memset(count, 0, sizeof(count));
  memset(aps, 0, sizeof(aps));
  memset(lastseen, 0, sizeof(lastseen));
}

void WiFiReader::showScanStatus()
{
  snprintf(buf, sizeof(buf), "%s", "WiFi: ");
  for (int i = 0; i < n_channel; i++) {
    snprintf(buf + strlen(buf), sizeof(buf + strlen(buf)), "%2d|", aps[i]);
  }
  snprintf(buf + strlen(buf), sizeof(buf + strlen(buf)), "%2d|", all_zero_count);

  callback_(buf);
}

void WiFiReader::update()
{
  handleScan();
}

/*
 * kick wifi scan if needed
 * when a scan is completed put message strings into the waiting queue
 * handle waiting queue while waiting scan
 */
void WiFiReader::handleScan()
{
  if (isScanning == false) {
    if (!ch_.connected()) {
      restart();
    }
    // TODO(where writer): not sure why, but when the serial is disconnected
    // sometimes it can be a strange state that ch.connected() == true and WiFi Scan does not work
    // restart the hardware if the WiFi scan returns no result for 10 consequtive cycles
    if (channel == 0) {
      checkZeroScan(10);
    }
    if (millis() <= scanningStart) {
      // during scan interval
      checkQueue();
      return;
    }

    if (count[channel] >= skip[channel]) {
      // start scan for the current channcel
      //
      // definition
      WiFi.scanNetworks(true, false, false, scan_duration, channel + 1);
      scanningStart = millis();
      count[channel] = 0;
      isScanning = true;
      // showScanStatus();
    } else {
      // skip until skip count
      count[channel] += 1;
      channel = (channel + 1) % n_channel;
    }
  } else {
    int n = 0;
    if ((n = WiFi.scanComplete()) >= 0) {
      // scan completed
      aps[channel] = n;
      showScanStatus();
      if (verbose) {
        snprintf(
          buf, sizeof(buf), "[ch:%2d][%3dAPs][skip:%2d/%2d]%3lums,%5lums",
          channel + 1, n, skip[channel], max_skip,
          millis() - scanningStart, millis() - lastseen[channel]);
        // ch_.loginfo(buf);
      }
      lastseen[channel] = millis();
      scanningStart = millis() + scan_interval;

      if (n == 0) {
        // increments skip count if no AP is found at the current channel
        skip[channel] = min(skip[channel] + 1, max_skip);
      } else {
        // if APs are found, put string into the queue
        skip[channel] = 0;
        for (int i = 0; i < n && waiting < MAX_WAITING; ++i) {
          String name = WiFi.SSID(i);
          name.replace(",", " ");
          snprintf(
            msg_buf[waiting], sizeof(msg_buf[waiting]), "%s,%s,%d,%d,%d,%d",
            WiFi.BSSIDstr(i).c_str(), name.c_str(), WiFi.channel(i),
            WiFi.RSSI(i), ch_.now().sec, ch_.now().nsec);
          waiting++;
        }
      }

      channel = (channel + 1) % n_channel;
      isScanning = false;
    } else {
      // waiting scan result
      checkQueue();
    }
  }
}

void WiFiReader::checkQueue()
{
  if (waiting > 0) {
    waiting--;
    size_t len = strlen(msg_buf[waiting]);
    ch_.publish(0x17, msg_buf[waiting], len);
  }
}

void WiFiReader::checkZeroScan(int maximum)
{
  bool all_zero = true;
  for (int i = 0; i < n_channel; i++) {
    all_zero = all_zero && aps[i] == 0;
  }
  if (all_zero) {
    all_zero_count++;
    if (all_zero_count > maximum) {
      restart();
    }
  } else {
    all_zero_count = 0;
  }
}
