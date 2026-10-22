
void setup()
{
    pinMode(2, INPUT);       // IR sensor OUT → D2
    pinMode(13, OUTPUT);     // Built-in LED → D13

    Serial.begin(9600);
}

void loop()
{
    int irValue = digitalRead(2);

    Serial.print("IR = ");
    Serial.println(irValue);

    if (irValue == LOW)
    {
        digitalWrite(13, HIGH);   // Object detected → LED ON
    }
    else
    {
        digitalWrite(13, LOW);    // No object → LED OFF
    }

    delay(200);
}
