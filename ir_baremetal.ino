#include <avr/io.h>

void UART_init(void)
{
    // 9600 baud at 16 MHz
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // 8-bit data, 1 stop bit, no parity (8N1)
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

void UART_send_number(uint8_t num)
{
    if (num >= 100)
    {
        UART_send_char((num / 100) + '0');
        num %= 100;
        UART_send_char((num / 10) + '0');
        UART_send_char((num % 10) + '0');
    }
    else if (num >= 10)
    {
        UART_send_char((num / 10) + '0');
        UART_send_char((num % 10) + '0');
    }
    else
    {
        UART_send_char(num + '0');
    }
}

int main(void)
{
    // D2 / PD2 → input
    DDRD &= ~(1 << PD2);

    // D13 / PB5 → output
    DDRB |= (1 << PB5);

    UART_init();

    while (1)
    {
        if ((PIND & (1 << PD2)) == 0)
        {
            // Object detected
            PORTB |= (1 << PB5);

            UART_send_string("Object detected\r\n");
        }
        else
        {
            // No object
            PORTB &= ~(1 << PB5);

            UART_send_string("No object\r\n");
        }

        // Simple delay
        for (volatile uint32_t i = 0; i < 100000; i++)
        {
        }
    }
}
