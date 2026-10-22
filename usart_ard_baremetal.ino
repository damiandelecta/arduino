#include <avr/io.h>

void USART_Init(void)
{
    // 9600 baud at 16 MHz
    UBRR0H = 0;
    UBRR0L = 8;

    // Enable transmitter
    UCSR0B |= (1 << TXEN0);

    // 8-bit data
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_SendChar(char data)
{
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into transmit register
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
    USART_Init();

    while (1)
    {
        USART_SendString("Hello Boss\r\n");

        // Simple delay
        for (volatile long i = 0; i < 500000; i++);
    }
}