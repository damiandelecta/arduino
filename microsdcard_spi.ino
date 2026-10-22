#include <SPI.h>
#include <SD.h>

#define CS_PIN 10

void setup()
{
    Serial.begin(9600);

    Serial.println("Program started");

    Serial.println("Initializing SD card...");

    if (!SD.begin(CS_PIN))
    {
        Serial.println("SD initialization FAILED!");
        return;
    }

    Serial.println("SD initialization SUCCESS!");

    File file = SD.open("data.txt", FILE_WRITE);

    if (file)
    {
        Serial.println("File opened for writing");

        file.println("Hello from Arduino!");
        file.println("SPI microSD card test");

        file.close();

        Serial.println("Data written successfully");
    }
    else
    {
        Serial.println("File opening FAILED!");
    }

    file = SD.open("data.txt");

    if (file)
    {
        Serial.println("Reading file:");

        while (file.available())
        {
            Serial.write(file.read());
        }

        file.close();

        Serial.println();
        Serial.println("Reading completed");
    }
    else
    {
        Serial.println("File opening for reading FAILED!");
    }
}

void loop()
{
}