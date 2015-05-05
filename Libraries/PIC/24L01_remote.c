/*
    Testing with the new nRF24L01's
    
    This code receives with a Mirf V2 and  PIC16F88 dip.  It will output the 4 byte RX payload at 9600 baud.
        
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


#define RX_CE      PORTB.0  //PIN 6
#define RX_CSN     PORTB.1  //PIN 7
#define RX_SCK     PORTB.3  //PIN 9
#define RX_MOSI    PORTB.4  //PIN 10
#define RX_MISO    PORTB.6  //PIN 12
#define RX_IRQ     PORTA.0  //PIN 17

#define RED_LED        PORTB.7
#define BLUE_LED       PORTA.2

uns8 data_array[4];


void boot_up(void);

void configure_RX(void);

void reset_RX(void);

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
    
    configure_RX();
    
    
    while(1)
    {
        
        if (RX_IRQ == 0)    //wait for anything
        {
            for (x = 0; x < 5; x++)
            {
                RED_LED = 1;
                delay_ms(10);
                RED_LED = 0;
                BLUE_LED = 1;
                delay_ms(10);
                BLUE_LED = 0;
            }
            
            delay_ms(200);
            reset_RX();
            
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



//2.4G Configuration - Transmitter
//This sets up one RF-24G for shockburst transmission
void configure_RX(void)
{
    uns8 i, j, data, cmd;
    
    RX_CSN = 0;
    RX_CE = 0;
    
    data = 0x39; //PRX, CRC enabled======================================================================================
    cmd = 0x20;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;
    
    
    RX_CSN = 0;
    
    data = 0x00; //dissable auto-ack for all channels==================================================================   
    cmd = 0x21;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;
    
    
    RX_CSN = 0;
    
    data = 0x03; //address width = 5 bytes =====================================================================================
    cmd = 0x23;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;

    
    RX_CSN = 0;
    
    data = 0x07; //data rate = 1MB =========================================================================================
    cmd = 0x26;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;
    

    RX_CSN = 0;

    //
    data = 0x04; //4 byte payload =======================================================================================
    cmd = 0x31;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;

    
    RX_CSN = 0;

    //set channel 2======================================================================================================
    data = 0x02; 
    cmd = 0x25;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;
    
    
    
    
    RX_CSN = 0;

    //set address E7E7E7E7E7===============================================================================================
    data = 0xE7; 
    cmd = 0x30;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for (j = 0; j < 5; j++)
    {
        for(i = 0 ; i < 8 ; i++)
        {
            RX_MOSI = data.7;

            RX_SCK = 1;

            RX_SCK = 0;
            
            data <<= 1;
        }
        
        data = 0xE7;
    }
    
    RX_CSN = 1;
  
  
    RX_CSN = 0;
    
    data = 0x3B; //PWR_UP = 1 ==============================================================================================
    cmd = 0x20;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
    }
    
    RX_CSN = 1;
    
    RX_CE = 1;
   
    
    
}



//reset all ints
void reset_RX(void)
{
    uns8 i, j, data, cmd;
    
    
    cmd = 0x61; //Read RX payload ==========================================================================================
    
    RX_CSN = 0;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    for (j = 0; j < 4; j++)
    {
        data = 0;
        
        for(i = 0 ; i < 8 ; i++)
        {
            data <<= 1;
            
            RX_SCK = 1;
            
            data.0 = RX_MISO;
    
            RX_SCK = 0;

        }
        
        while(TXIF == 0);   //print out RX payload...=====================================================================
        TXREG = data;
 
    }
    
    RX_CSN = 1;
    
    while(TXIF == 0);
    TXREG = 10;
    while(TXIF == 0);
    TXREG = 13;
    
    cmd = 0xE2; //Flush RX FIFO ==========================================================================================
    
    RX_CSN = 0;
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;

        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
    }
    
    RX_CSN = 1;

    RX_CSN = 0;

    //
    cmd = 0x27;
    data = 0x40;    //reset int ===========================================================================================
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = cmd.7;
                
        RX_SCK = 1;

        RX_SCK = 0;
        
        cmd <<= 1;
        
    }
    
    for(i = 0 ; i < 8 ; i++)
    {
        RX_MOSI = data.7;
                
        RX_SCK = 1;

        RX_SCK = 0;
        
        data <<= 1;
        
    }
    
    RX_CSN = 1;
    
    
    
}

//General short delay
void delay_ms(uns16 x)
{
    uns8 y, z;
    for ( ; x > 0 ; x--)
        for ( y = 0 ; y < 4 ; y++)
            for ( z = 0 ; z < 176 ; z++);
}