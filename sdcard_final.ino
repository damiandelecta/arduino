#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* ================= USART ================= */

void USART_Init(void)
{
    UBRR0H = 0;
    UBRR0L = 103;                         // 9600 baud

    UCSR0B = (1 << TXEN0);               // Enable transmitter

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

void USART_SendHex(uint8_t data)
{
    const char hex[] = "0123456789ABCDEF";

    USART_SendChar(hex[(data >> 4) & 0x0F]);
    USART_SendChar(hex[data & 0x0F]);
}

/* ================= SPI ================= */

void SPI_Init(void)
{
    /*
       Arduino UNO:

       PB2 = D10 = CS
       PB3 = D11 = MOSI
       PB4 = D12 = MISO
       PB5 = D13 = SCK
    */

    DDRB |= (1 << PB2);       // CS    -> Output
    DDRB |= (1 << PB3);       // MOSI  -> Output
    DDRB |= (1 << PB5);       // SCK   -> Output

    DDRB &= ~(1 << PB4);     // MISO  -> Input

    PORTB |= (1 << PB2);      // CS HIGH

    /*
       SPI enabled
       Master mode
       Clock = F_CPU / 128 = 125 kHz
    */

    SPCR = (1 << SPE) |
           (1 << MSTR) |
           (1 << SPR1) |
           (1 << SPR0);
}

uint8_t SPI_Transfer(uint8_t data)
{
    SPDR = data;

    while (!(SPSR & (1 << SPIF)));

    return SPDR;
}

/* ================= SD CARD ================= */

void SD_Select(void)
{
    PORTB &= ~(1 << PB2);      // CS LOW
}

void SD_Deselect(void)
{
    PORTB |= (1 << PB2);       // CS HIGH

    SPI_Transfer(0xFF);        // Extra clock
}

/* Send SD command */

uint8_t SD_SendCommand(uint8_t cmd,
                       uint32_t arg,
                       uint8_t crc)
{
    uint8_t response;
    uint16_t timeout;

    SPI_Transfer(0x40 | cmd);

    SPI_Transfer((uint8_t)(arg >> 24));
    SPI_Transfer((uint8_t)(arg >> 16));
    SPI_Transfer((uint8_t)(arg >> 8));
    SPI_Transfer((uint8_t)arg);

    SPI_Transfer(crc);

    /* Wait for R1 response */

    for (timeout = 0; timeout < 1000; timeout++)
    {
        response = SPI_Transfer(0xFF);

        if (response != 0xFF)
            return response;
    }

    return 0xFF;
}

/* ================= SD INITIALIZATION ================= */

uint8_t SD_Init(void)
{
    uint8_t response;
    uint8_t i;
    uint8_t r7[4];
    uint8_t ocr[4];

    USART_SendString("SD Initialization\r\n");

    /*
       CS HIGH
       Send at least 80 clock pulses
       to enter SPI mode
    */

    SD_Deselect();

    for (i = 0; i < 10; i++)
        SPI_Transfer(0xFF);

    /* ---------- CMD0 ---------- */

    SD_Select();

    response = SD_SendCommand(0, 0x00000000, 0x95);

    USART_SendString("CMD0 R1 = 0x");
    USART_SendHex(response);
    USART_SendString("\r\n");

    SD_Deselect();

    if (response != 0x01)
        return 0;

    /* ---------- CMD8 ---------- */

    SD_Select();

    response = SD_SendCommand(8, 0x000001AA, 0x87);

    USART_SendString("CMD8 R1 = 0x");
    USART_SendHex(response);
    USART_SendString("\r\n");

    for (i = 0; i < 4; i++)
        r7[i] = SPI_Transfer(0xFF);

    USART_SendString("R7 = ");

    for (i = 0; i < 4; i++)
    {
        USART_SendHex(r7[i]);
        USART_SendChar(' ');
    }

    USART_SendString("\r\n");

    SD_Deselect();

    /* ---------- ACMD41 ---------- */

    do
    {
        /* CMD55 */

        SD_Select();

        response = SD_SendCommand(55, 0x00000000, 0x01);

        SD_Deselect();

        /* ACMD41 */

        SD_Select();

        response = SD_SendCommand(
                        41,
                        0x40000000,       // HCS = 1
                        0x01);

        SD_Deselect();

    } while (response == 0x01);

    USART_SendString("ACMD41 R1 = 0x");
    USART_SendHex(response);
    USART_SendString("\r\n");

    if (response != 0x00)
        return 0;

    /* ---------- CMD58 ---------- */

    SD_Select();

    response = SD_SendCommand(
                    58,
                    0x00000000,
                    0x01);

    USART_SendString("CMD58 R1 = 0x");
    USART_SendHex(response);
    USART_SendString("\r\n");

    for (i = 0; i < 4; i++)
        ocr[i] = SPI_Transfer(0xFF);

    USART_SendString("OCR = ");

    for (i = 0; i < 4; i++)
    {
        USART_SendHex(ocr[i]);
        USART_SendChar(' ');
    }

    USART_SendString("\r\n");

    SD_Deselect();

    USART_SendString("SD CARD READY!\r\n");

    return 1;
}

/* ================= READ BLOCK ================= */

uint8_t SD_ReadBlock(uint32_t block,
                     uint8_t *buffer)
{
    uint8_t response;
    uint8_t token;
    uint16_t i;
    uint16_t timeout;

    SD_Select();

    /*
       CMD17 = READ_SINGLE_BLOCK

       For SDHC/SDXC:
       argument = block number
    */

    response = SD_SendCommand(
                    17,
                    block,
                    0x01);

    if (response != 0x00)
    {
        SD_Deselect();
        return 0;
    }

    /* Wait for data token 0xFE */

    for (timeout = 0; timeout < 1000; timeout++)
    {
        token = SPI_Transfer(0xFF);

        if (token == 0xFE)
            break;
    }

    if (token != 0xFE)
    {
        SD_Deselect();
        return 0;
    }

    /* Read 512 bytes */

    for (i = 0; i < 512; i++)
    {
        buffer[i] = SPI_Transfer(0xFF);
    }

    /* Read CRC */

    SPI_Transfer(0xFF);
    SPI_Transfer(0xFF);

    SD_Deselect();

    return 1;
}

/* ================= WRITE BLOCK ================= */

uint8_t SD_WriteBlock(uint32_t block,
                      uint8_t *buffer)
{
    uint8_t response;
    uint8_t data_response;
    uint16_t i;
    uint16_t timeout;

    SD_Select();

    /*
       CMD24 = WRITE_SINGLE_BLOCK
    */

    response = SD_SendCommand(
                    24,
                    block,
                    0x01);

    if (response != 0x00)
    {
        SD_Deselect();
        return 0;
    }

    /* Data start token */

    SPI_Transfer(0xFE);

    /* Send 512 bytes */

    for (i = 0; i < 512; i++)
    {
        SPI_Transfer(buffer[i]);
    }

    /* Dummy CRC */

    SPI_Transfer(0xFF);
    SPI_Transfer(0xFF);

    /* Data response token */

    data_response = SPI_Transfer(0xFF);

    /*
       0x05 = Data accepted
    */

    if ((data_response & 0x1F) != 0x05)
    {
        SD_Deselect();
        return 0;
    }

    /* Wait until card finishes writing */

    for (timeout = 0; timeout < 1000; timeout++)
    {
        if (SPI_Transfer(0xFF) == 0xFF)
            break;
    }

    SD_Deselect();

    return 1;
}

/* ================= MAIN ================= */

int main(void)
{
    uint8_t buffer[512];

    uint16_t i;

    USART_Init();
    SPI_Init();

    USART_SendString("\r\n");
    USART_SendString("SPI + SD CARD BARE METAL\r\n");
    USART_SendString("========================\r\n");

    /* Initialize SD card */

    if (!SD_Init())
    {
        USART_SendString("SD INITIALIZATION FAILED!\r\n");

        while (1);
    }

    /*
       Use block 100 for testing.

       DO NOT use block 0 because it contains
       important filesystem/partition information.
    */

    /* ---------- WRITE ---------- */

    USART_SendString("\r\nWriting BLOCK 100...\r\n");

    /*
       Fill buffer with:
       00 01 02 03 ... FF
       00 01 02 03 ... FF
    */

    for (i = 0; i < 512; i++)
    {
        buffer[i] = (uint8_t)i;
    }

    if (SD_WriteBlock(100, buffer))
    {
        USART_SendString("WRITE SUCCESSFUL!\r\n");
    }
    else
    {
        USART_SendString("WRITE FAILED!\r\n");

        while (1);
    }

    /* ---------- READ ---------- */

    USART_SendString("\r\nReading BLOCK 100...\r\n");

    for (i = 0; i < 512; i++)
    {
        buffer[i] = 0;
    }

    if (SD_ReadBlock(100, buffer))
    {
        USART_SendString("READ SUCCESSFUL!\r\n");
    }
    else
    {
        USART_SendString("READ FAILED!\r\n");

        while (1);
    }

    /* ---------- DISPLAY ---------- */

    USART_SendString("\r\nFirst 32 bytes:\r\n");

    for (i = 0; i < 32; i++)
    {
        USART_SendHex(buffer[i]);
        USART_SendChar(' ');

        if ((i + 1) % 16 == 0)
            USART_SendString("\r\n");
    }

    /* ---------- VERIFY ---------- */

    USART_SendString("\r\nVerifying data...\r\n");

    for (i = 0; i < 512; i++)
    {
        if (buffer[i] != (uint8_t)i)
        {
            USART_SendString("VERIFY FAILED!\r\n");

            while (1);
        }
    }

    USART_SendString("VERIFY SUCCESSFUL!\r\n");

    USART_SendString("\r\nSPI SD READ/WRITE COMPLETE!\r\n");

    while (1)
    {
    }
}