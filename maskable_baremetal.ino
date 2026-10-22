#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t buttonPressed = 0;

// INT0 ISR
ISR(INT0_vect)
{
    buttonPressed = 1;
}

// USART initialization
void USART_Init(void)
{
    // Baud rate = 9600 for 16 MHz clock
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B |= (1 << TXEN0);

    // 8-bit data, 1 stop bit, no parity
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

// Send one character
void USART_SendChar(char data)
{
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into transmit register
    UDR0 = data;
}

// Send a string
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
    // -------------------------
    // INT0 / Button setup
    // -------------------------

    // PD2 / D2 → INPUT
    DDRD &= ~(1 << PD2);

    // Enable internal pull-up
    PORTD |= (1 << PD2);

    // INT0 → Falling edge
    EICRA &= ~(1 << ISC00);
    EICRA |=  (1 << ISC01);

    // Enable INT0
    EIMSK |= (1 << INT0);

    // -------------------------
    // USART setup
    // -------------------------

    USART_Init();

    // Enable global interrupts
    sei();

    USART_SendString("System Started\r\n");

    while (1)
    {
        if (buttonPressed)
        {
            buttonPressed = 0;

            USART_SendString("Button Pressed!\r\n");
        }
    }
}
