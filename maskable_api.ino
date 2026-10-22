volatile bool buttonPressed = false;

void setup()
{
    pinMode(13, OUTPUT);

    pinMode(2, INPUT_PULLUP);

    Serial.begin(9600);

    attachInterrupt(
        digitalPinToInterrupt(2),
        buttonISR,
        FALLING
    );
}

void loop()
{
    if (buttonPressed)
    {
        buttonPressed = false;

        digitalWrite(13, !digitalRead(13));

        Serial.println("Interrupt occurred - Button pressed");
    }
}

void buttonISR()
{
    buttonPressed = true;
}
