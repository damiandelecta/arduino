HardwareSerial MySerial(2);

#define RX_PIN 16
#define TX_PIN 17

void setup()
{
    // USB serial monitor
    Serial.begin(115200);

    // UART2
    MySerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop()
{
    if (MySerial.available())
    {
        char data = MySerial.read();

        Serial.print(data);
    }
}