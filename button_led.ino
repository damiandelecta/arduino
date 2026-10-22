void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(10, OUTPUT);
}

void loop()
{
    int buttonState = digitalRead(2);

    if (buttonState == LOW)
    {
        digitalWrite(10, HIGH);
    }
    else
    {
        digitalWrite(10, LOW);
    }
}
