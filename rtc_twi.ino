#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  if (!rtc.begin())
  {
    Serial.println("RTC not found!");
    while (1);
  }

  // Set RTC using the computer's date and time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("RTC started!");
}

void loop()
{
  DateTime now = rtc.now();

  Serial.print("Time: ");

  if (now.hour() < 10)
    Serial.print("0");
  Serial.print(now.hour());

  Serial.print(":");

  if (now.minute() < 10)
    Serial.print("0");
  Serial.print(now.minute());

  Serial.print(":");

  if (now.second() < 10)
    Serial.print("0");
  Serial.println(now.second());

  delay(1000);
}