#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t timerFlag = 0;


// Timer1 Overflow ISR
ISR(TIMER1_OVF_vect)
{
    // Reload Timer1 for approximately 1 second
    TCNT1 = 49911;

    timerFlag = 1;
}


void USART_Init(void)
{
    // 9600 baud, 16 MHz clock
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B |= (1 << TXEN0);

    // 8-bit data, 1 stop bit, no parity
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
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


int main(void)
{
    // D13 / PB5 → OUTPUT
    DDRB |= (1 << PB5);

    // Timer1 configuration
    TCCR1A = 0;
    TCCR1B = 0;

    // Load initial value
    TCNT1 = 49911;

    // Start Timer1 with 1024 prescaler
    TCCR1B |= (1 << CS12) | (1 << CS10);

    // Enable Timer1 overflow interrupt
    TIMSK1 |= (1 << TOIE1);

    // USART
    USART_Init();

    // Enable global interrupts
    sei();

    USART_SendString("System Started\r\n");

    while (1)
    {
        if (timerFlag)
        {
            timerFlag = 0;

            // Toggle D13
            PORTB ^= (1 << PB5);

            USART_SendString("Timer1 Overflow\r\n");
        }
    }
}
