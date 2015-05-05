/*
    
    nRF2401 test code to run on the 24G demo board, V02, text between 2 modules
    
    Compiles with the free version of CC5X...?
    Pete Dokter, 2/22/06
    

    config_setup word 16 bits found on pages 13-15
    
    23: 0 Payloads have an 8 bit address
    22: 0
    21: 1
    20: 0
    19: 0
    18: 0
    17: 1 16-Bit CRC
    16: 1 CRC Enabled

    15: 0 One channel receive
    14: 1 ShockBurst Mode
    13: 0 250K Transmission Rate
    12: 0
    11: 1
    10: 1
    9: 1 RF Output Power
    8: 0 RF Output Power

    7: 0 Channel select (channel 2)
    6: 0
    5: 0
    4: 0
    3: 0
    2: 1
    1: 0
    0: 0 Transmit mode
    
*/
#define Clock_8MHz
#define Baud_9600

#include "C:\Global\PIC\C\16F88.h"


#pragma config |= 0x3F30 //Internal Oscillator, No WDT, MCLR Enabled


#define CS       PORTA.0        //out
#define CLK1     PORTA.1        //out
#define DATA1    PORTA.2        //I/O
#define DR1      PORTA.3        //in
#define DATA2    PORTA.4        //in
#define CE       PORTA.6        //out
#define CLK2     PORTA.7        //out


#define DR2      PORTB.0        //in
#define stat1    PORTB.1        //out
#define stat2    PORTB.3        //out
#define stat3    PORTB.4        //out




uns8 data_array[4];


void boot_up(void);
void configure_receiver(void);
void configure_transmitter(void);
void transmit_data(void);
void receive_data(void);
void delay_ms(uns16);

void main()
{
    uns8 x;
        
    boot_up();
    
    for (x = 0; x < 3; x++)
        {
                    
            stat1 = 1;
            delay_ms(25);
            stat1 = 0;
            stat2 = 1;
            delay_ms(25);
            stat2 = 0;
            stat3 = 1;
            delay_ms(25);
            stat3 = 0;
        }
    
    stat1 = 1;
    
    configure_receiver();
   
    while(1)
    {
        if (RCIF == 1)
        {
            
            configure_transmitter();
            
            data_array[0] = RCREG;
            
            transmit_data();
            
            configure_receiver();
        }
    

        
        if(DR1 == 1) //We have data!
        {
           
            receive_data();
            
            
            if (stat3 == 1)
            {
                stat3 = 0;
                stat1 = 1;                    
            }
            
            else if (stat2 == 1)
            {
                stat2 = 0;
                stat3 = 1;
            }
            
            else if (stat1 == 1)
            {
                stat1 = 0;
                stat2 = 1;
            }
            
            TXREG = data_array[0];
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
    TRISA = 0b.0011.1100;  //0 = Output, 1 = Input

    PORTB = 0b.0000.0000;  
    TRISB = 0b.1100.0101;  //0 = Output, 1 = Input

    BRGH = 1; //High speed UART
    
    SPBRG = 51;//9600 baud


    SYNC = 0;
    SPEN = 1;

    CREN = 1;
    TXEN = 1; //Enable transmission

    
    RCIF = 0;
    RCIE = 1;
    PEIE = 1;
    GIE = 1;
    
    data_array[0] = 0x00;
    data_array[1] = 0x00;
    data_array[2] = 0x00;
    data_array[3] = 0x00;
}

//This will clock out the current payload into the data_array
void receive_data(void)
{
    uns8 i, j, temp;

    CE = 0;//Power down RF Front end

    //Erase the current data array so that we know we are looking at actual received data
    data_array[0] = 0x00;
    

    //Clock in data, we are setup for 32-bit payloads
    for(i = 0 ; i < 4 ; i++) //4 bytes
    {
        for(j = 0 ; j < 8 ; j++) //8 bits each
        {
            temp <<= 1;
            temp.0 = DATA1;

            CLK1 = 1;
            CLK1 = 0;
        }

        data_array[i] = temp; //Store this byte
    }
    
    

    CE = 1; //Power up RF Front end
}



//This sends out the data stored in the data_array
//data_array must be setup before calling this function
void transmit_data(void)
{
    uns8 i, j, temp, rf_address;
    
    CE = 1;
    
    delay_ms(1);

    //Clock in address
    for (i = 0; i < 5; i++)
    {
    
        rf_address = 0b.1110.0111; //Power-on Default for all units (on page 11)
        
        for(j = 0 ; j < 8 ; j++)
        {
            DATA1 = rf_address.7;
            CLK1 = 1;
            CLK1 = 0;
            
            rf_address <<= 1;
        }
    }
    
    //Clock in the data_array
    for(i = 0 ; i < 4 ; i++) //4 bytes
    {
        temp = data_array[i];
        
        for(j = 0 ; j < 8 ; j++) //One bit at a time
        {
            DATA1 = temp.7;
            CLK1 = 1;
            CLK1 = 0;
            
            temp <<= 1;
        }
    }
    
    CE = 0; //Start transmission   
}



//2.4G Configuration - Receiver
//This setups up a RF-24G for receiving at 1mbps
void configure_receiver(void)
{
    uns8 i,j;
    //uns24 config_setup;
    uns8 config_setup[8], temp;

    //During configuration of the receiver, we need DATA1 as an output
    PORTA = 0b.0000.0000;  
    TRISA = 0b.0011.1000;  //0 = Output, 1 = Input (DR1 is on RA3) (DATA1 is on RA2)

    //Config Mode
    CE = 0; CS = 1;
    
    delay_ms(1);
    
    //Setup configuration word, set up for 1MB
    //config_setup = 0b.0010.0011.0100.1110.0000.0101; //Look at pages 13-15 for more bit info
    config_setup[7] = 0b.1110.0111;
    config_setup[6] = 0b.1110.0111;
    config_setup[5] = 0b.1110.0111;
    config_setup[4] = 0b.1110.0111;
    config_setup[3] = 0b.1110.0111;
    
    config_setup[0] = 0b.0000.0101;
    config_setup[1] = 0b.0110.1110;
    config_setup[2] = 0b.1010.0001;
    
    /*//purchased version of CC5X
    for(i = 0 ; i < 24 ; i++)
    {
        DATA1 = config_setup.23;
        CLK1 = 1;
        CLK1 = 0;
        
        config_setup <<= 1;
        
    }
    */
    
    //free version
    for (j = 8; j > 0; j--)
    {
        for(i = 0 ; i < 8 ; i++)
        {
            DATA1 = config_setup[j-1].7;
            CLK1 = 1;
            CLK1 = 0;
            
            config_setup[j-1] <<= 1;
            
        }
    }
    
    
    //Configuration is actived on falling edge of CS (page 10)
    CE = 0; CS = 0;

    //After configuration of the receiver, we need DATA1 as an input
    PORTA = 0b.0000.0000;  
    TRISA = 0b.0011.1100;  //0 = Output, 1 = Input (DR1 is on RA3) (DATA1 is on RA2)
    
    delay_ms(1);

    //Start monitoring the air
    CE = 1; CS = 0;


}    



//2.4G Configuration - Transmitter
//This sets up one RF-24G for shockburst transmission
void configure_transmitter(void)
{
    uns8 i,j;
    //uns24 config_setup;
    uns8 config_setup[8], temp;
    
    PORTA = 0b.0000.0000;  
    TRISA = 0b.0011.1000;  //0 = Output, 1 = Input (DR1 is on RA3) (DATA1 is on RA2)

    //Config Mode
    CE = 0; CS = 1;
    
    delay_ms(1);
        
    //Setup configuration word
    //config_setup = 0b.0010.0011.0100.1110.0000.0100; //Look at pages 13-15 for more bit info
    config_setup[7] = 0b.1110.0111;
    config_setup[6] = 0b.1110.0111;
    config_setup[5] = 0b.1110.0111;
    config_setup[4] = 0b.1110.0111;
    config_setup[3] = 0b.1110.0111;
    
    config_setup[0] = 0b.0000.0100;
    config_setup[1] = 0b.0110.1110;
    config_setup[2] = 0b.1010.0001; //address width and CRC
    
    /*//purchased version
    for(i = 0 ; i < 24 ; i++)
    {
        DATA1 = config_setup.23;
        CLK1 = 1;
        CLK1 = 0;
        
        config_setup <<= 1;
    }
    */
    
    //free version
    for (j = 3; j > 0; j--)
    {
        for(i = 0 ; i < 8 ; i++)
        {
            DATA1 = config_setup[j-1].7;
            CLK1 = 1;
            CLK1 = 0;
            
            config_setup[j-1] <<= 1;
            
            
        }
    }
    
    
    delay_ms(1);
    
    //Configuration is actived on falling edge of CS (page 10)
    CE = 0; CS = 0;

    
}



//General short delay
void delay_ms(uns16 x)
{
    uns8 y, z;
    for ( ; x > 0 ; x--)
        for ( y = 0 ; y < 4 ; y++)
            for ( z = 0 ; z < 176 ; z++);
}