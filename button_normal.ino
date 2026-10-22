int ledState = LOW;
int lastButtonState = HIGH;

void setup()
{
    pinMode(2, INPUT_PULLUP);
    pinMode(10, OUTPUT);
}

void loop()
{
    int buttonState = digitalRead(2);

    if (lastButtonState == HIGH && buttonState == LOW)
    {
        ledState = !ledState;

        digitalWrite(10, ledState);

        delay(50);
    }

    lastButtonState = buttonState;
}
