#include <avr/io.h>

#define TRIG_PIN   PB1       // D9
#define ECHO_PIN   PB0       // D8

void delay_us(uint16_t us)
{
    while (us--)
    {
        _delay_loop_1(4);
    }
}

uint16_t measure_echo_time(void)
{
    uint16_t count = 0;

    // Wait until ECHO becomes HIGH
    while (!(PINB & (1 << ECHO_PIN)));

    // Count while ECHO remains HIGH
    while (PINB & (1 << ECHO_PIN))
    {
        count++;
        delay_us(1);
    }

    return count;
}

void USART_Init(void)
{
    UBRR0H = 0;
    UBRR0L = 103;                         // 9600 baud @ 16 MHz

    UCSR0B = (1 << TXEN0);                // Enable transmitter

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
}

void USART_SendChar(char data)
{
    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = data;
}

void USART_SendString(const char *str)
{
    while (*str)
    {
        USART_SendChar(*str);
        str++;
    }
}

void USART_SendNumber(uint16_t number)
{
    char digits[5];
    uint8_t i = 0;

    if (number == 0)
    {
        USART_SendChar('0');
        return;
    }

    while (number > 0)
    {
        digits[i++] = (number % 10) + '0';
        number /= 10;
    }

    while (i > 0)
    {
        USART_SendChar(digits[--i]);
    }
}

int main(void)
{
    uint16_t echo_time;
    uint16_t distance;

    // D9 / PB1 → output
    DDRB |= (1 << TRIG_PIN);

    // D8 / PB0 → input
    DDRB &= ~(1 << ECHO_PIN);

    USART_Init();

    while (1)
    {
        // Make TRIG LOW
        PORTB &= ~(1 << TRIG_PIN);

        delay_us(2);

        // Send 10 µs trigger pulse
        PORTB |= (1 << TRIG_PIN);

        delay_us(10);

        PORTB &= ~(1 << TRIG_PIN);

        // Measure ECHO HIGH duration
        echo_time = measure_echo_time();

        // Distance = time × speed of sound / 2
        distance = (echo_time * 343) / 20000;

        USART_SendString("Distance = ");

        USART_SendNumber(distance);

        USART_SendString(" cm\r\n");

        // Small delay before next measurement
        delay_us(50000);
    }
}
