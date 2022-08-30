#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SS       DDB2
#define DD_SCK      DDB5

uint8_t tx_addr[5] = {0x00, 0x01, 0x02, 0x03, 0x04};    //Address of line 0
char buffer_[20];                                       //Buffer to store the data
uint8_t payload[3];
uint8_t nRF_status;
uint8_t val0 = 0, val1 = 0, val2 = 0;
uint8_t PTX;

uint8_t data_ready();
uint8_t get_data(uint8_t * payload);

void setup()
{
  Serial.begin(9600);
  
  _delay_ms(1000);      // wait 1 second
  
  DDRB |= ((1 << PB2) | (1 << PB1)); //CNS | CE
  PORTB &= ~(1 << PB1); // turn off the module
  PORTB |= (1 << PB2);  // turn on SPI
  spi_init(); //init SPI

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x27); //STATUS
  spi_fast_shift(0x10); //MAX_RT
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x27); //STATUS
  spi_fast_shift(0x20); //TX_DS
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end
  _delay_ms(100);       // Espera 100ms

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x25); //RF_CH
  spi_fast_shift(23);  //CHANNEL
  PORTB |= (1 << PB2); //HIGH of SPI to detect end

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x26); //RF_SETUP
  spi_fast_shift(0x26); //power in 0dBm and 250kbps
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x31); //RX_PW_P0
  spi_fast_shift(0x03); //3 BYTES
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end

  PTX = 0;              //Start in receiving mode

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x20); //CONFIG
  spi_fast_shift(0x2F); //3 BYTES
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end

  PORTB |= (1 << PB1);  // turn on the module

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x30); //TX_ADDRESS
  spi_transmit_sync(tx_addr,5);//ADDRESS, LENGTH
  PORTB |= (1 << PB2);  //HIGH of SPI to detect end
  
  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x2A); //RX_ADDRESS_P0
  spi_transmit_sync(tx_addr,5);//ADDRESS, LENGTH
  PORTB |= (1 << PB2); //HIGH of SPI to detect end
}

void loop() {
  while (!data_ready());          // Wait until the data is received
  nRF_status = get_data(payload); // Store the data[payload]
  val0 = payload[0];              
  val1 = payload[1];
  val2 = payload[2];

  itoa(val0, buffer_, 10);
  Serial.print("Status: ");
  Serial.print(nRF_status,BIN);
  Serial.print(" ");
  Serial.println(buffer_);
}

uint8_t get_data(uint8_t * payload){
  uint8_t data_status;
  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  data_status = spi_fast_shift(0x61); //R_RX_PAYLOAD
  spi_transfer_sync(payload,payload,3); //read PAYLOAD
  PORTB |= (1 << PB2); //HIGH of SPI to detect end

  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  spi_fast_shift(0x27); //STATUS
  spi_fast_shift(0x40); //RX_DR
  PORTB |= (1 << PB2); //HIGH of SPI to detect end
  return data_status;
}
uint8_t data_ready(){
  if(PTX){
    return 0;
  }
  uint8_t data_status;
  PORTB &= ~(1 << PB2); //LOW of SPI to detect init
  data_status = spi_fast_shift(0xFF); //NOP
  PORTB |= (1 << PB2); //HIGH of SPI to detect end
  return data_status & (0x40);
}

void spi_init()
// Initialize pins for spi communication
{
    DDR_SPI &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    // Define the following pins as output
    DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK));

    
    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (1<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

    SPSR = (0<<SPI2X);              // Double Clock Rate
    
}

void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
             datain[i] = SPDR;
       }
}

void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
       }
}

uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
    return SPDR;
}
