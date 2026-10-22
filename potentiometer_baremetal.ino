#include <avr/io.h>

void UART_init(void)
{
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0B = (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) |
             (1 << UCSZ00);
}

void UART_send_char(char data)
{
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

void UART_send_number(uint16_t num)
{
    char buffer[6];
    uint8_t i = 0;

    if (num == 0)
    {
        UART_send_char('0');
        return;
    }

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num = num / 10;
    }

    while (i > 0)
    {
        UART_send_char(buffer[--i]);
    }
}

void ADC_init(void)
{
    ADMUX = (1 << REFS0);

    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);
}

uint16_t ADC_read(void)
{
    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC))
    {
    }

    return ADC;
}

int main(void)
{
    UART_init();
    ADC_init();

    while (1)
    {
        uint16_t adcValue = ADC_read();

        UART_send_string("ADC = ");
        UART_send_number(adcValue);
        UART_send_string("\r\n");

        for (volatile uint32_t i = 0; i < 80000; i++)
        {
        }
    }
}
