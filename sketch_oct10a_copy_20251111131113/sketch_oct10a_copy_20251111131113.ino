#include "RTClib.h"
#include <U8glib.h>
#include <Servo.h>
#include <LedControl.h>
#include <math.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
RTC_DS3231 rtc;
Servo myservo;



LedControl lc=LedControl(12,11,10,1);

void setup () {
  Serial.begin(9600);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  myservo.attach(9);

  u8g.setFont(u8g_font_helvB10);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop () {
  DateTime now = rtc.now();
  int min = now.minute();
  int hr = now.hour();   
  int sec = now.second();

  int v = sec * 6;
  int m = min * 6;
  int h = (hr % 12) * 30;

  float temp = rtc.getTemperature();
  int pos = map((int)temp, 0, 40, 0, 180);
  myservo.write(pos);

  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Servo pos: "); Serial.println(pos);
  Serial.println(hr);

  showTime(temp, hr, min);
  draw(v, m, h);

  delay(1000);
}

void showTime(int temp, int h, int m){
  lc.setDigit(0,7,h/10,false);
  lc.setDigit(0,6,h%10,false);
  lc.setDigit(0,5,m/10,false);
  lc.setDigit(0,4,m%10,false);
  lc.setDigit(0,2,temp/10,false);
  lc.setDigit(0,1,temp%10,false);
  lc.setChar(0, 0, 'C', false);
}

void draw(int v, int m, int h){
  float radSec = (v - 90) * DEG_TO_RAD;
  float radMin = (m - 90) * DEG_TO_RAD;
  float radHr  = (h - 90) * DEG_TO_RAD;

  u8g.firstPage();
  do {
    u8g.drawCircle(64, 32, 24);
    u8g.drawLine(64, 32, 64 + 20 * cos(radSec), 32 + 20 * sin(radSec));
    u8g.drawLine(64, 32, 64 + 18 * cos(radMin), 32 + 18 * sin(radMin));
    u8g.drawLine(64, 32, 64 + 12 * cos(radHr),  32 + 12 * sin(radHr));
  } while (u8g.nextPage());
}

