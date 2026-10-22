#include <avr/io.h>
#include <util/delay.h>

#define CS      PB2       // Arduino D10
#define MOSI    PB3       // Arduino D11
#define MISO    PB4       // Arduino D12
#define SCK     PB5       // Arduino D13

// --------------------------------------------------
// SPI INITIALIZATION
// --------------------------------------------------

void SPI_Init(void)
{
    // MOSI, SCK and CS as OUTPUT
    DDRB |= (1 << MOSI) | (1 << SCK) | (1 << CS);

    // MISO as INPUT
    DDRB &= ~(1 << MISO);

    // CS HIGH initially
    PORTB |= (1 << CS);

    // Enable SPI
    // Master mode
    // SPI clock = F_CPU / 128
    SPCR = (1 << SPE) |
           (1 << MSTR) |
           (1 << SPR1) |
           (1 << SPR0);
}

// --------------------------------------------------
// SPI TRANSFER
// --------------------------------------------------

uint8_t SPI_Transfer(uint8_t data)
{
    // Put data into SPI Data Register
    SPDR = data;

    // Wait until transfer is complete
    while (!(SPSR & (1 << SPIF)));

    // Return received data
    return SPDR;
}

// --------------------------------------------------
// SEND SD COMMAND
// --------------------------------------------------

void SD_SendCommand(uint8_t cmd,
                    uint32_t arg,
                    uint8_t crc)
{
    SPI_Transfer(cmd | 0x40);

    SPI_Transfer((arg >> 24) & 0xFF);
    SPI_Transfer((arg >> 16) & 0xFF);
    SPI_Transfer((arg >> 8) & 0xFF);
    SPI_Transfer(arg & 0xFF);

    SPI_Transfer(crc);
}

// --------------------------------------------------
// MAIN
// --------------------------------------------------

int main(void)
{
    uint8_t response;

    SPI_Init();

    _delay_ms(100);

    // Send at least 74 clock pulses with CS HIGH
    for (uint8_t i = 0; i < 10; i++)
    {
        SPI_Transfer(0xFF);
    }

    // Select SD card
    PORTB &= ~(1 << CS);

    // CMD0 - GO_IDLE_STATE
    SD_SendCommand(0, 0x00000000, 0x95);

    // Read response
    response = SPI_Transfer(0xFF);

    // Deselect SD card
    PORTB |= (1 << CS);

    while (1)
    {
    }
}