/* this program demonstrate the usage of mailbox , task1 will count 0to15 and send 15 to task2 via mailbox
task2 will increment count value from 15 to 30 and then task1 resumes*/


#include <rtl.h>
#include<stdio.h>
#include<lpc214x.h>


os_mbx_declare (MsgBox, 100);                /* Declare an RTX mailbox  100 msgs with name MsgBox*/


/* Reserve a memory for 32 blocks of 20 bytes  */
_declare_box(mpool,20,32);

char displayadc[18];
float brightness;
char ledoutput[18];

int dutycycle;
float adcvalue;

void initilizePLL(void);
void initilizePWM(unsigned int periodPWM);
void delaytime(unsigned int j);

void initilizePLL(void) // Function to use PLL for clock generation
{
    PLL0CON = 0x01;
    PLL0CFG = 0x24;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;

    while (!(PLL0STAT & 0x00000400))
        ;
    PLL0CON = 0x03;
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
    VPBDIV = 0x01;
}

void delaytime(unsigned int j) // fucntion to generate 1 milisecond delay
{
    unsigned int x, i;
    for (i = 0; i < j; i++)
    {
        for (x = 0; x < 6000; x++)
            ;
    }
}

void initilizePWM(unsigned int PWMvalue)
{
    PINSEL0 = 0x00000002;         // Setting pin P0.0 for PWM output
    PWMTCR = (1 << 1);            // Setting PWM Timer Control Register as counter reset
    PWMPR = 0X00;                 // Setting PWM prescale value
    PWMMCR = (1 << 0) | (1 << 1); // Setting PWM Match Control Register
    PWMMR0 = PWMvalue;            // Giving PWM value Maximum value
    PWMLER = (1 << 0);            // Enalbe PWM latch
    PWMTCR = (1 << 0) | (1 << 3); // Enabling PWM and PWM counter
}

void LCD_SEND(char command) // Function to send hex commands
{
    IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((command & 0xF0) << 8)); // Send upper nibble of command
    IO0SET = 0x00000040;                                        // Making Enable HIGH
    IO0CLR = 0x00000030;                                        // Making RS & RW LOW
    delaytime(5);
    IO0CLR = 0x00000040; // Makeing Enable LOW
    delaytime(5);
    IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((command & 0x0F) << 12)); // Send Lower nibble of command
    IO0SET = 0x00000040;                                         // ENABLE HIGH
    IO0CLR = 0x00000030;                                         // RS & RW LOW
    delaytime(5);
    IO0CLR = 0x00000040; // ENABLE LOW
    delaytime(5);
}

void LCD_INITILIZE(void) // Function to get ready the LCD
{
    IO0DIR = 0x0000FFF0; // Sets pin P0.12,P0.13,P0.14,P0.15,P0.4,P0.6 as OUTPUT
    delaytime(20);
    LCD_SEND(0x02); // Initialize lcd in 4-bit mode of operation
    LCD_SEND(0x28); // 2 lines (16X2)
    LCD_SEND(0x0C); // Display on cursor off
    LCD_SEND(0x06); // Auto increment cursor
    LCD_SEND(0x01); // Display clear
    LCD_SEND(0x80); // First line first position
}

void LCD_DISPLAY(char *msg) // Function to print the characters sent one by one
{
    unsigned int i = 0;
    while (msg[i] != 0)
    {
        IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((msg[i] & 0xF0) << 8)); // Sends Upper nibble
        IO0SET = 0x00000050;                                       // RS HIGH & ENABLE HIGH to print data
        IO0CLR = 0x00000020;                                       // RW LOW Write mode
        delaytime(2);
        IO0CLR = 0x00000040; // EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0)
        delaytime(5);
        IO0PIN = ((IO0PIN & 0xFFFF00FF) | ((msg[i] & 0x0F) << 12)); // Sends Lower nibble
        IO0SET = 0x00000050;                                        // RS & EN HIGH
        IO0CLR = 0x00000020;
        delaytime(2);
        IO0CLR = 0x00000040;
        delaytime(5);
        i++;
    }
}


unsigned int cnt1,cnt2;




__task void task1 (void);
__task void task2 (void);


__task void task1 (void)
{
    /* This task will send a count value. */
    U32 *mptr;
    os_tsk_create (task2, 0);
    os_mbx_init (MsgBox, sizeof(MsgBox));
    mptr = _alloc_box (mpool);                /* Allocate a memory for the message */
	  
		initilizePLL();                    // Calls function initilizePLL
    initilizePWM(255); 
	  PINSEL1 = 0x00040000; // p0.25   pin is selected as as ad0.4
	
    //IO0DIR = 0x000f0000;

	  //PINSEL1 = 0x01000000;              // Setting P0.28 as ADC INPUT
    AD0CR = (((14) << 8) | (1 << 21)); // Setting clock and PDN for A/D Conversion
                    // Calls function initilizePWM with value 255
    PWMPCR |= (1 << 9);                // To enable PWM1 output at pin P0.0 of LPC2148

    while (1)
    {
			 AD0CR = 0x09200010; // control register for adc     ad.04
        while((temp = AD0GDR) == 0x80000000);
        adcvalue = AD0GDR;      // to get the adc values in result
        adcvalue &= ~0x01000000; // to stop the ADC
        adcvalue = (adcvalue >> 6);
        adcvalue = adcvalue & 0x000003FF;
			  dutycycle = dutycycle = adcvalue / 4;
			
		
			/*
        AD0CR |= (1 << 1); // Select AD0.1 channel in ADC register
        delaytime(10);
        AD0CR |= (1 << 24); // Start the A/D conversion
        while ((AD0DR1 & (1 << 31)) == 0)
            ;                             // Check the DONE bit in ADC Data register
        adcvalue = (AD0DR1 >> 6) & 0x3ff; // Get the RESULT from ADC data register
        dutycycle = adcvalue / 4;         // formula to get dutycycle values from (0 to 255)
			*/

				cnt1=dutycycle;
				
        mptr[0] = cnt1;
        cnt1=0;
        os_mbx_send (MsgBox, mptr, 0xffff);	 // Send the count value to a 'Mailbox' 
				
        os_dly_wait(5);
    }

}

__task void task2 (void)
{
    /* This task will receive a count. */
    U32 *rptr  ;
    cnt2=0;
    while(1)
    {
        os_mbx_wait (MsgBox, (void**)&rptr, 0xffff);      /* Wait for the message to arrive. */
			
        cnt2 = rptr[0];	    /* copy the count value from task1 to cnt2  */
				PWMMR1 = cnt2; // set dutycycle value to PWM match register
        PWMLER |= (1 << 1); // Enable PWM output with dutycycle value
        brightness = cnt2;
        LCD_SEND(0x80);
        sprintf(displayadc, "adcvalue=%f", adcvalue);
        LCD_DISPLAY(displayadc); // Display ADC value (0 to 1023)
        LCD_SEND(0xC0);
        sprintf(ledoutput, "PWM OP=%.2f   ", brightness);
        LCD_DISPLAY(ledoutput);
        os_tsk_prio(tsk3, 3);
        os_tsk_prio(tsk1, 2);
        os_tsk_prio_self(1);
        os_sem_send(semaphore1);
        cnt2=0;
    }
}
void delay(unsigned int x){
	int k=0;
	for(k=0;k<x;k++);
}

int main (void)
{
    //init_serial();
    _init_box (mpool, sizeof(mpool), sizeof(U32));
    os_sys_init(task1);
}

/*----------------------------------------------------------------------------
*       init_serial:  Initialize Serial Interface
*---------------------------------------------------------------------------*/

/*
void init_serial (void)
{
    PINSEL0 = 0X0000005;                 // Enable RxD0 and TxD0
    U0LCR = 0x83;                         // 8 bits, no Parity, 1 Stop bit
    U0DLL = 0x61;                           // 9600 Baud Rate @ 15MHz VPB Clock
    U0LCR = 0x03;                         // DLAB = 0
}
*/



/*

        if (result < 0x00CC)  //  <  0.66
        {
            // IOCLR0 = 0x00000000;
            IO0CLR = 0x000F0000;	   	//all led's off
        }

        if (result > 0x00CC && result <= 0x0199)  // from 0.66 to 1.32
        {
            IOSET0 = 0x00080000;
            IOCLR0 = 0x00070000;
            delay(100000);
        }

        else if (result > 0x0199 && result <= 0x0266)	// from 1.32 to 1.98
        {
            IOSET0 = 0x000C0000;
            IOCLR0 = 0x00030000;
            delay(100000);
        }

        else if (result > 0x0266 && result <= 0x0333)	//from 1.98 ro 2.64
        {
            IOSET0 = 0x000E0000;
            IOCLR0 = 0x00010000;
            delay(100000);
        }

        else if(result > 0x0333 && result <= 0x03FF)	 //from 2.64 to 3.3
        {
            IOSET0 = 0x000F0000;
            IOCLR0 = 0x00000000;
            delay(100000);
        }
		
    }
*/

