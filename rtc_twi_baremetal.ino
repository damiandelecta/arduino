#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define DS3231_ADDRESS 0x68

// =====================================================
// I2C / TWI
// =====================================================

void I2C_init(void)
{
    // SCL = 100 kHz
    TWSR = 0x00;
    TWBR = 72;

    // Enable TWI
    TWCR = (1 << TWEN);
}


// Generate START condition
void I2C_start(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));
}


// Generate STOP condition
void I2C_stop(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTO) |
           (1 << TWEN);
}


// Send one byte
void I2C_write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));
}


// Read one byte and send ACK
uint8_t I2C_read_ACK(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN) |
           (1 << TWEA);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}


// Read one byte and send NACK
uint8_t I2C_read_NACK(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}


// =====================================================
// BCD to Decimal
// =====================================================

uint8_t BCD_to_DEC(uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}


// =====================================================
// USART
// =====================================================

void USART_init(void)
{
    // 9600 baud at 16 MHz
    UBRR0H = 0;
    UBRR0L = 103;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // 8-bit data
    UCSR0C = (1 << UCSZ01) |
             (1 << UCSZ00);
}


void USART_sendChar(char data)
{
    // Wait until transmit buffer is empty
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


// =====================================================
// DS3231 RTC READ
// =====================================================

void RTC_readTime(uint8_t *hour,
                  uint8_t *minute,
                  uint8_t *second)
{
    // -------------------------------
    // START
    // -------------------------------
    I2C_start();


    // -------------------------------
    // DS3231 address + WRITE
    // 0x68 << 1 = 0xD0
    // 0xD0 | 0 = 0xD0
    // -------------------------------
    I2C_write((DS3231_ADDRESS << 1) | 0);


    // -------------------------------
    // Select seconds register
    // -------------------------------
    I2C_write(0x00);


    // -------------------------------
    // REPEATED START
    // -------------------------------
    I2C_start();


    // -------------------------------
    // DS3231 address + READ
    // 0x68 << 1 = 0xD0
    // 0xD0 | 1 = 0xD1
    // -------------------------------
    I2C_write((DS3231_ADDRESS << 1) | 1);


    // -------------------------------
    // Read seconds
    // Send ACK because more data needed
    // -------------------------------
    *second = I2C_read_ACK();


    // -------------------------------
    // Read minutes
    // Send ACK because more data needed
    // -------------------------------
    *minute = I2C_read_ACK();


    // -------------------------------
    // Read hours
    // Send NACK because this is last byte
    // -------------------------------
    *hour = I2C_read_NACK();


    // -------------------------------
    // STOP
    // -------------------------------
    I2C_stop();


    // -------------------------------
    // Convert BCD to decimal
    // -------------------------------
    *second = BCD_to_DEC(*second);
    *minute = BCD_to_DEC(*minute);
    *hour   = BCD_to_DEC(*hour);
}


// =====================================================
// MAIN
// =====================================================

int main(void)
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    // Initialize I2C
    I2C_init();

    // Initialize USART
    USART_init();

    USART_sendString("DS3231 RTC\r\n");

    while (1)
    {
        // Read RTC time
        RTC_readTime(&hour, &minute, &second);


        USART_sendString("Time: ");


        // -------------------------------
        // HOURS
        // -------------------------------

        if (hour < 10)
            USART_sendChar('0');

        USART_sendChar((hour / 10) + '0');
        USART_sendChar((hour % 10) + '0');


        USART_sendChar(':');


        // -------------------------------
        // MINUTES
        // -------------------------------

        if (minute < 10)
            USART_sendChar('0');

        USART_sendChar((minute / 10) + '0');
        USART_sendChar((minute % 10) + '0');


        USART_sendChar(':');


        // -------------------------------
        // SECONDS
        // -------------------------------

        if (second < 10)
            USART_sendChar('0');

        USART_sendChar((second / 10) + '0');
        USART_sendChar((second % 10) + '0');


        USART_sendString("\r\n");


        // Wait 1 second
        _delay_ms(1000);
    }
}
