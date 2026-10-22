#define TRIG_PIN 9
#define ECHO_PIN 8

long duration;
float distance;

void setup()
{
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    Serial.begin(9600);
}

void loop()
{
    // Make sure TRIG starts LOW
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Send 10 µs trigger pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure how long ECHO stays HIGH
    duration = pulseIn(ECHO_PIN, HIGH);

    // Calculate distance in cm
    distance = duration * 0.0343 / 2;

    // Display distance
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(500);
}
