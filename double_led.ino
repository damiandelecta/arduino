void setup()
{
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    
}

void loop()     
{
    digitalWrite(10, HIGH);
    delay(500);
    digitalWrite(10, LOW);

    digitalWrite(11, HIGH);
    delay(500);
    digitalWrite(11, LOW);    
}
