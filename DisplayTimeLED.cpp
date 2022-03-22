/** @file DisplayTimeLED
 *  @brief Arduino LED Backpack time display
 */

#include <Wire.h>
#include <DS1307RTC.h>
#include <TimeLib.h>
#include "Adafruit_LEDBackpack.h"
#define SERIALOUT 1
// Object Instantiation
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
// Global Data

// Data Time Data
const char *monthName[12] =
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;
char displaybuffer[10] = {0};

/** @brief Arduino main setup
 *  @author Robert R Miller
 *  @date 21MAR22
 *  @param None
 *  @return void
 */
void setup() {
  // setup rtc
  bool parse=false;
  bool config=false;
    if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }
  alpha4.begin(0x70);
  alpha4.clear();
  alpha4.writeDigitAscii(0, 'a');
  alpha4.writeDisplay();
}

/** @brief Arduino main control loop
 *  @author Robert R Miller
 *  @date 21MAR22
 *  @param None
 *  @return void
 */
void loop() {
  alpha4.clear();
  RTC.read(tm);
  sprintf(displaybuffer,"%2d%02d%02d",tm.Hour,tm.Minute,tm.Second);
  
#if SERIALOUT 
  Serial.println();
  Serial.print(displaybuffer);
#endif
  alpha4.writeDigitAscii(0,displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);  
  // write it out!
  alpha4.writeDisplay();
  delay(1000);
}

/** @brief parse time from string & add it to struct tm
 *  @author Robert R Miller
 *  @date 21MAR22
 *  @param const char *str
 *  @return bool
 */
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

/** @brief parse date from string & add it to struct tm
 *  @author Robert R Miller
 *  @date 21MAR22
 *  @param const char *str
 *  @return bool
 */
bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;
  
  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3){ return false; }
  for (monthIndex = 0; monthIndex < 12; monthIndex++) 
  {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
