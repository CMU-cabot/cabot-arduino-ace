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
#ifndef uart_com_h
#define uart_com_h
#define CMD_BUF_MAX (128)
#define MAX_LEN  64

class uart_com{
  private:
  //for parsing
    char CmdBuf[CMD_BUF_MAX]; //command buffer
    int CmdBuf_wp=0; //Number of stored in command buffer
    char * words[MAX_LEN];
    int words_len = 0;
    char *delim = ",";
    bool CMD_PARSE_FLAG =false;
    bool StartFlg =false;
    
    unsigned long _last_update_millis = 0;
    bool _started = false;
    bool parse_mot();
    bool parse_thresh();
    bool parse_sensi();
    bool parse_dat();
    void StringCmdParse(char c);
  public:
    bool touch;
    int capacitance;
    int cap_thresh;
    int sensi = 2;
    int motor_r;
    int motor_c;
    int motor_l;
    int switch_up;
    int switch_down;
    int switch_left;
    int switch_right;
    int switch_center;
    uart_com();
    void begin(int baud_rate = 115200);
    void update();
    void start();
    void stop();
    bool is_started();
    bool is_alive();
    bool set_mot(int right, int center, int left);
    bool set_mot_r(int val);
    bool set_mot_c(int val);
    bool set_mot_l(int val);
    bool set_thresh(int thresh);
    bool set_sensi(int sensi);
};

#endif
