
#include <avr/io.h>

void UART_init(void)
{
    // 9600 baud at 16 MHz
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // 8 data bits, no parity, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_send_char(char data)
{
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }

    UDR0 = data;
}

void UART_send_string(const char *str)
{
    while (*str)
    {
        UART_send_char(*str);
        str++;
    }
}

int main(void)
{
    // D2 / PD2 → input
    DDRD &= ~(1 << PD2);

    // D13 / PB5 → output
    DDRB |= (1 << PB4);

    // Initialize USART
    UART_init();

    while (1)
    {
        // Read touch sensor
        if (PIND & (1 << PD2))
        {
            // Touch detected
            PORTB |= (1 << PB4);

            UART_send_string("Touch detected\r\n");
        }
        else
        {
            // No touch
            PORTB &= ~(1 << PB4);

            UART_send_string("No touch\r\n");
        }

        // Simple delay
        for (volatile uint32_t i = 0; i < 100000; i++)
        {
        }
    }
}
