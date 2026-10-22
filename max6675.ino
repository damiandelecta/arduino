#include <max6675.h>

int SO_PIN  = 12;
int CS_PIN  = 10;
int SCK_PIN = 13;

MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);

void setup()
{
  Serial.begin(9600);

  delay(500);

  Serial.println("MAX6675 Temperature");
}

void loop()
{
  float temperature = thermocouple.readCelsius();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  delay(1000);
}