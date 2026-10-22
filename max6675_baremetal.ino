#include <avr/io.h>
#include <util/delay.h>

#define CS   PB2
#define SCK  PB5
#define SO   PB4

void SPI_init(void)
{
    // CS and SCK as outputs
    DDRB |= (1 << CS);
    DDRB |= (1 << SCK);

    // SO as input
    DDRB &= ~(1 << SO);

    // CS initially HIGH
    PORTB |= (1 << CS);

    // SCK initially LOW
    PORTB &= ~(1 << SCK);
}

uint16_t MAX6675_read(void)
{
    uint16_t data = 0;

    // Select MAX6675
    PORTB &= ~(1 << CS);

    _delay_us(1);

    // Read 16 bits
    for (int i = 15; i >= 0; i--)
    {
        // Clock HIGH
        PORTB |= (1 << SCK);

        _delay_us(1);

        // Read SO
        if (PINB & (1 << SO))
        {
            data |= (1 << i);
        }

        // Clock LOW
        PORTB &= ~(1 << SCK);

        _delay_us(1);
    }

    // Deselect MAX6675
    PORTB |= (1 << CS);

    return data;
}

void USART_init(void)
{
    // 9600 baud
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // 8-bit data, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_sendChar(char data)
{
    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = data;
}

void USART_sendString(const char *str)
{
    while (*str)
    {
        USART_sendChar(*str);
        str++;
    }
}

void USART_sendNumber(uint16_t num)
{
    char buffer[6];
    int i = 0;

    if (num == 0)
    {
        USART_sendChar('0');
        return;
    }

    while (num > 0)
    {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0)
    {
        USART_sendChar(buffer[--i]);
    }
}

int main(void)
{
    uint16_t rawData;
    uint16_t temperature;

    SPI_init();
    USART_init();

    USART_sendString("MAX6675 Temperature\r\n");

    while (1)
    {
        rawData = MAX6675_read();

        // Check thermocouple open condition
        if (rawData & 0x0004)
        {
            USART_sendString("Thermocouple Error\r\n");
        }
        else
        {
            // Bits D14-D3 contain temperature
            temperature = (rawData >> 3) & 0x0FFF;

            // Each bit represents 0.25 degree Celsius
            temperature = temperature / 4;

            USART_sendString("Temperature: ");
            USART_sendNumber(temperature);
            USART_sendString(" C\r\n");
        }

        _delay_ms(1000);
    }
}