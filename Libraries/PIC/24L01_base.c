/*
    Testing with the new nRF24L01's
    
    This code transmits with a Mirf V2 and  PIC16F88 dip.  It will output the status register contents
        in a couple of spots so you can see what's going on internally, 9600 baud.
        
    This code will also compile with the free version of CC5X!!!!!!!!!!!!!!!!
        
    This configuration is compatable with nRF2401's (1MB, 8 bit CRC, 4 byte payload), it doesn't implement all of the extra
        functionality of the 24L01's
        
    Pete, 4/20/06
        
*/

#define Clock_8MHz
#define Baud_9600

#include "C:\Global\PIC\C\16F88.h"

//There is no config word because this program tested on a 16F88 using Bloader the boot load program

#pragma origin 4



#define TX_CE      PORTB.0  //PIN 6
#define TX_CSN     PORTB.1  //PIN 7
#define TX_SCK     PORTB.3  //PIN 9
#define TX_MOSI    PORTB.4  //PIN 10
#define TX_MISO    PORTB.6  //PIN 12
#define TX_IRQ     PORTA.0  //PIN 17

#define RED_LED        PORTB.7
#define BLUE_LED       PORTA.2

uns8 data_array[4];


void boot_up(void);

void configure_transmitter(void);
void transmit_data(void);

void delay_ms(uns16);

void main()
{
    uns8 x, y;
    
      
    boot_up();
    
    for (x = 0; x < 5; x++)
    {
        RED_LED = 1;
        delay_ms(10);
        RED_LED = 0;
        BLUE_LED = 1;
        delay_ms(10);
        BLUE_LED = 0;
    }
    
    
    configure_transmitter();
    
    
    
    while(1)
    {
        
        
        while (1)  
        {
            transmit_data();
            RED_LED = 1;
            delay_ms(10);
            RED_LED = 0;
           
            delay_ms(500);
           
        }
        
            
    }
        
}

void boot_up(void)
{
    OSCCON = 0b.0111.0000; //Setup internal oscillator for 8MHz
    while(OSCCON.2 == 0); //Wait for frequency to stabilize

    ANSEL = 0b.0000.0000; //Turn pins to Digital instead of Analog
    CMCON = 0b.0000.0111; //Turn off comparator on RA port

    PORTA = 0b.0000.0000;  
    TRISA = 0b.1111.1011;  //0 = Output, 1 = Input 

    PORTB = 0b.0000.0010;  
    TRISB = 0b.0100.0100;  //0 = Output, 1 = Input (RX is an input)

    BRGH = 0; //Normal speed UART
    SPBRG = 12; //8MHz for 9600 Baud

    SYNC = 0;
    SPEN = 1;
    CREN = 1;
    TXEN = 1;
}



//This sends out the data stored in the data_array
//data_array must be setup before calling this function
void transmit_data(void)
{
    uns8 i, data, cmd;
    bit data2;
    
    
    TX_CSN = 0;
    
    data = 0x7E; //clear previous ints======================================================================================
    cmd = 0x27;
    
    for(i = 0 ; i < 8 ; i++)
    {

        TX_MOSI = cmd.7;
        
        TX_SCK = 1;
        
        data2 = TX_MISO;
        TX_SCK = 0;
        
        cmd <<= 1;
        
        while(TXIF == 0);   //print out the status register before clearing stuff...==============================
        TXREG = data2+48;
    }
    
    while(TXIF == 0);
    TXREG = 44;
    while(TXIF == 0);
    TXREG = 32;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;
       
        TX_SCK = 1;
        
        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
    
    
    TX_CSN = 0;
    
    data = 0x3A; //PWR_UP = 1===========================================================================================
    cmd = 0x20;
    
    for(i = 0 ; i < 8 ; i++)
    {
        
        TX_MOSI = cmd.7;
       
        TX_SCK = 1;
        
        
        TX_SCK = 0;
        
        cmd <<= 1;
        
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        
        TX_MOSI = data.7;
       
        TX_SCK = 1;
        
        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
    
    
    TX_CSN = 0;
    
    //clear TX fifo===========================================================================================
    //the data sheet says that this is supposed to come up 0 after POR, but that doesn't seem to be the case
    cmd = 0xE1;
    
    for(i = 0 ; i < 8 ; i++)
    {
        
        TX_MOSI = cmd.7;
       
        TX_SCK = 1;
      
        
        TX_SCK = 0;
        
        cmd <<= 1;
        
    }
    
    
    TX_CSN = 1;
    
    

    TX_CSN = 0;
    
    data2 = 0;

    
    data = 0x34; //4 byte payload================================================================================
    cmd = 0xA0;
    
    for(i = 0 ; i < 8 ; i++)
    {

        TX_MOSI = cmd.7;

        TX_SCK = 1;
        data2 = TX_MISO;

        TX_SCK = 0;
        
        cmd <<= 1;
        
        while(TXIF == 0);   //print out status register after clearing ints...===============================
        TXREG = data2+48;
        
    }
    
    
    while(TXIF == 0);
    TXREG = 10;
    while(TXIF == 0);
    TXREG = 13;
    
    //clock in payload==============================================================================================
    for(i = 0 ; i < 8 ; i++)    //"4"
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    data = 0x33;
    
    for(i = 0 ; i < 8 ; i++)    //"3"
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    data = 0x32;
    
    for(i = 0 ; i < 8 ; i++)    //"2"
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    data = 0x31;
    
    for(i = 0 ; i < 8 ; i++)    //"1"
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
    
    //Pulse CE to start transmission ========================================================================
    TX_CE = 1;
    delay_ms(1);
    TX_CE = 0;
    

}




//2.4G Configuration - Transmitter
//This sets up one RF-24G for shockburst transmission
void configure_transmitter(void)
{
    uns8 i, j, data, data2, cmd;
    data2 = 0;
    
    TX_CE = 0;

    TX_CSN = 0;


    cmd = 0x20;
    data = 0x38; // PTX, CRC enabled, mask a couple of ints=============================================================
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;    
    
    
    TX_CSN = 0;
    
    data = 0x00; //auto retransmit off===========================================================================================
    cmd = 0x24;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;

    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;  
    
    
    TX_CSN = 0;
    
    data = 0x03; //address width = 5======================================================================================
    cmd = 0x23;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        data2.0 = TX_MISO;
        TX_SCK = 0;
        
        data2 <<= 1;
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
     
    
    TX_CSN = 0;
    
    data = 0x07; //data rate = 1MB=====================================================================================
    cmd = 0x26;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;    
    
    
    TX_CSN = 0;
    
    data = 0x02; //set channel 2, this is default but we did it anyway...====================================================
    cmd = 0x25;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
    
    
    
    data = 0xE7; //set address E7E7E7E7E7, also default...====================================================================
    cmd = 0x30;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for (j = 0; j < 5; j++)
    {
        for(i = 0 ; i < 8 ; i++)
        {
            TX_MOSI = data.7;

            TX_SCK = 1;

            TX_SCK = 0;
            
            data <<= 1;
        }
        
        data = 0xE7;
    }
    
    
    TX_CSN = 1;
    
    
    TX_CSN = 0;
    
    //disable auto-ack, RX mode===========================================================================================
    //shouldn't have to do this, but it won't TX if you don't
    data = 0x00; 
    cmd = 0x21;
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = cmd.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        TX_MOSI = data.7;

        TX_SCK = 1;

        TX_SCK = 0;
        
        data <<= 1;
    }
    
    TX_CSN = 1;
    
    
    
    
}



//General short delay
void delay_ms(uns16 x)
{
    uns8 y, z;
    for ( ; x > 0 ; x--)
        for ( y = 0 ; y < 4 ; y++)
            for ( z = 0 ; z < 176 ; z++);
}