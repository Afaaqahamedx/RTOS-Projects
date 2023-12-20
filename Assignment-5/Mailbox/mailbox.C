#include <lpc21xx.h>
#include <rtl.h>
#include <stdio.h>

unsigned long int value, i, cnt = 0, q;
unsigned int c[] = {0x30, 0x30, 0x20, 0x20, 0x28, 0x01, 0x06, 0x0e, 0x89};   //cmd for LCD
unsigned char m, j; 

unsigned int row0[4] = {0x00ee0000, 0x00ed0000, 0x00eb0000, 0x00e70000};
unsigned int row1[4] = {0x00de0000, 0x00dd0000, 0x00db0000, 0x00d70000};
unsigned int row2[4] = {0x00db0000, 0x00bd0000, 0x00bb0000, 0x00b70000};
unsigned int row3[4] = {0x007e0000, 0x007d0000, 0x007b0000, 0x00770000};

unsigned char msg1[] = {"ROW0"};
unsigned char msg2[] = {"ROW1"};
unsigned char msg3[] = {"ROW2"};
unsigned char msg4[] = {"ROW3"};

void delay(unsigned int);
void cmd(unsigned int);
void data(unsigned int);
void keypad_lcd_init();

__task void task1(void);
__task void task2(void);


			os_mbx_declare(MsgBox, 100); /* Declare an RTX mailbox  100 msgs with name MsgBox*/
																		/* Reserve a memory for 32 blocks of 20 bytes  */
			_declare_box(mpool, 20, 32);


void keypad_lcd_init()
{
    PINSEL1 = 0x00000000;
    // IODIR0=0xf0ff0000; // making po.16 to p0.23  and p0.28 to p0.31 output lines for disp
    // IOSET0=0XF0000000;
    IO1DIR = 0XFFF0FFFF; // set rows as output and colomn as input
    
		// init for lcd 
		PINSEL0 = 0x00000000;
    IODIR0 = 0x000000fc;
    IOCLR0 = 0x000000fc;
}

void cmd(unsigned int value)
{
    unsigned int y;
    y = value;
    y = y & 0xf0;
    IOCLR0 = 0x000000fc;
    IOCLR0 = 0X00000004;
    IOSET0 = y;

    IOSET0 = 0x00000008;
    delay(10);
    IOCLR0 = 0x00000008;

    y = value;
    y = y & 0x0f;
    y = y << 4;
    IOCLR0 = 0x000000fc;
    IOCLR0 = 0X00000004;
    IOSET0 = y;

    IOSET0 = 0x00000008;
    delay(10);
    IOCLR0 = 0x00000008;
}
void data(unsigned int dat)
{
    unsigned int y;
    y = dat;
    y = y & 0xf0;
    IOCLR0 = 0x000000fc;
    IOSET0 = 0X00000004;
    IOSET0 = y;

    IOSET0 = 0x00000008;
    delay(10);
    IOCLR0 = 0x00000008;

    y = dat;
    y = y & 0x0f;
    y = y << 4;
    IOCLR0 = 0x000000fc;
    IOSET0 = 0X00000004;
    IOSET0 = y;

    IOSET0 = 0x00000008;
    delay(10);
    IOCLR0 = 0x00000008;
}

void delay(unsigned int x)
{
    unsigned int del;
    for (del = 0; del < x; del++)
        ;
}

__task void task1(void)
{
    U32 *mptr;
    os_tsk_create(task2, 0);
    os_mbx_init(MsgBox, sizeof(MsgBox));
    mptr = _alloc_box(mpool); /* Allocate a memory for the message */

    while (1)
    {
			
			// for row1
			
        IO1PIN = 0x00ff0000; // initialize rows and colomns with one
        IOCLR1 = 0x00100000; // enable row0
        value = IOPIN1;
        delay(50000);

        value = value & 0x00ff0000;
        for (i = 0; i < 4; i++)
        {
            if (value == row0[i])
            {
                cnt = 1;
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000); // delay
            }
        }
				
				
				//for row1
				
        IO1PIN = 0x00ff0000; // initialize rows and colomns with one
        IOCLR1 = 0x00200000; // enable row1
        value = IOPIN1;
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000); // delay
        value = value & 0x00ff0000;
        for (i = 0; i < 4; i++)
        {
            if (value == row1[i])
            {
                cnt = 2;
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
            }
        }
				
				//for row2
				

        IO1PIN = 0x00ff0000; // initialize rows and colomns with one
        IOCLR1 = 0x00400000; // enable row2
        value = IOPIN1;
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000);
        value = value & 0x00ff0000;
        for (i = 0; i < 4; i++)
        {
            if (value == row2[i])
            {
                cnt = 3;
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
            }
        }
	
				//for row3
				
        IOPIN1 = 0x00ff0000; // initialize rows and colomns with one
        IOCLR1 = 0x00800000; // enable row3   -> 7f
        value = IOPIN1;
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000);
        delay(65000);
        value = value & 0x00ff0000;
        for (i = 0; i < 4; i++)
        {
            if (value == row3[i])
            {
                cnt = 4;
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
                delay(65000);
            }
        }
				cnt=2;
        mptr[0] = cnt;
        cnt = 0;
        os_mbx_send(MsgBox, mptr, 0xffff); /* Send the count value to a 'Mailbox' */
        os_dly_wait(15);
    }
}

__task void task2(void)
{

    int cnt2;
    U32 *rptr;
    cnt2 = 0;
    while (1)
    {
        os_mbx_wait(MsgBox, (void **)&rptr, 0xffff); /* Wait for the message to arrive. */
        
				cnt2 = rptr[0];
        for (m = 0; m < 9; m++)
        {
            cmd(c[m]);
            delay(10000);
        }

        cmd(0x89);
        delay(10000);

        if (cnt2 == 1)
        {
            for (j = 0; j < 4; j++)
            {

                data(msg1[j]);
                delay(500);
            }
            delay(65000);
            delay(65000);
            delay(65000);
            cmd(0x01);
            delay(65000);
            delay(65000);
            delay(65000);
            delay(65000);
        }

        if (cnt2 == 2)
        {
            for (j = 0; j < 4; j++)
            {

                data(msg2[j]);
                delay(500);
                // delay(10000);
            }
            delay(65000);
            delay(65000);
            delay(65000);
            cmd(0x01);
            delay(65000);
            delay(65000);
            delay(65000);
            delay(65000);
        }
        if (cnt2 == 3)
        {
            for (j = 0; j < 4; j++)
            {

                data(msg3[j]);
                delay(500);
                // delay(10000);
            }
            delay(65000);
            delay(65000);
            delay(65000);
            cmd(0x01);
            delay(65000);
            delay(65000);
            delay(65000);
            delay(65000);
        }
        if (cnt2 == 4)
        {
            for (j = 0; j < 4; j++)
            {
                data(msg4[j]);
                delay(500);
                // delay(10000);
            }
            delay(65000);
            delay(65000);
            delay(65000);
            cmd(0x01);
            delay(65000);
            delay(65000);
            delay(65000);
            delay(65000);
        }
    }
}

int main()
{
    keypad_lcd_init();
    _init_box (mpool, sizeof(mpool), sizeof(U32));
    os_sys_init(task1);
}
