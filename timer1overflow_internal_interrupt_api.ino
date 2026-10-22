volatile bool timerFlag = false;

ISR(TIMER1_OVF_vect)
{
    timerFlag = true;
}

void setup()
{
    pinMode(13, OUTPUT);

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << CS12) | (1 << CS10);

    TIMSK1 |= (1 << TOIE1);

    sei();
}

void loop()
{
    if (timerFlag)
    {
        timerFlag = false;

        digitalWrite(13, !digitalRead(13));
    }
}
