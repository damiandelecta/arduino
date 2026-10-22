
void setup()
{
    pinMode(2, INPUT);       // Touch sensor OUT → D2
    pinMode(13, OUTPUT);     // Built-in LED → D13

    Serial.begin(9600);
}

void loop()
{
    int touchValue = digitalRead(2);

    Serial.print("Touch = ");
    Serial.println(touchValue);

    if (touchValue == HIGH)
    {
        digitalWrite(13, HIGH);   // Touched → LED ON
    }
    else
    {
        digitalWrite(13, LOW);    // Not touched → LED OFF
    }

    delay(200);
}
