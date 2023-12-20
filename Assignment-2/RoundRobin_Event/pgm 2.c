#include <rtl.h>
#include <lpc21xx.h>
#include <stdio.h>

void delay(unsigned int x);
void serial(void);
void cmd(unsigned int value);
void data(unsigned int);

unsigned int c[] = {0x30, 0x30, 0x20, 0x20, 0x28, 0x01, 0x06, 0x0e, 0x89};
unsigned char mg[25];
unsigned int i, j, s;

OS_TID tsk1, tsk2, tsk3; // declare task identification number variables
OS_RESULT RE1, RE2, RE3;

int cnt1, cnt2, cnt3;   // counter
__task void job1(void); // declare function for job1
__task void job2(void); // declare function for job2
__task void job3(void);

__task void job1(void) // job1 function definition
{
    int i;

    os_tsk_prio_self(3);    // assign priority to job1 as 2
    tsk1 = os_tsk_self();   // task id
    os_tsk_create(job2, 2); // create job2 and keep in ready state
    while (1)
    {

        RE1 = os_evt_wait_and(0x0001, 0x0010); // wait for event
        if (RE1 == OS_R_EVT)
        {
            for (i = 0; i < 1; i++)
            {
                serial();
                cnt1++;
                for (j = 0; j < 25; j++)
                {
                    while (!(U0LSR & 0x01))
                        ;

                    mg[j] = U0RBR;
                    if (mg[j] == '.')
                    {
                        break;
                    }
                }

                for (i = 0; i < j; i++)
                {
                    while (!(U0LSR & 0x20))
                        ;
                    U0THR = mg[i];
                }
                for (i = 0; i < 65000; i++)
                    ;
            }
        }
        os_evt_set(0x0003, tsk3);
    }
}

__task void job2(void) // job2 function definition
{

    int i, o;
    // PINSEL1=0x00000000;
    // IODIR0|=0x000f0000;
    os_tsk_prio_self(2);
    // tsk2=os_tsk_self(); 	   // task id
    os_tsk_create(job3, 1); // create job2 and keep in ready state
    while (1)
    {

        RE2 = os_evt_wait_and(0x0002, 0x0010); // wait for event
        if (RE2 == OS_R_EVT)
        {
            for (i = 0; i < 1; i++)
            {
                cnt2++;
                for (o = 0; o < 5; o++)
                {
                    IOSET0 = 0x000f0000;
                    for (i = 0; i < 1000000; i++)
                        ;
                    IOCLR0 = 0x000f0000;
                    for (i = 0; i < 1000000; i++)
                        ;
                }
                for (i = 0; i < 65000; i++)
                    ;
            }
        }
        os_evt_set(0x0001, tsk1);
    }
}

__task void job3(void)
{

    int i, j;

    // os_tsk_prio_self (1);
    tsk3 = os_tsk_self(); // task id
    while (1)
    {

        RE3 = os_evt_wait_and(0x0003, 0x0010); // wait for event
        if (RE3 == OS_R_EVT)
        {
            for (i = 0; i < 1; i++)
            {
                int m = 0;
                cnt3++;
                for (i = 0; i < s; i++)

                {
                    cmd(c[i]);
                    delay(10000);
                }

                while (m < 50)
                {
                    cmd(0x80);
                    delay(10000);
                    // delay(10000);
                    for (j = 0; j < 7; j++)
                    {

                        data(mg[j]);
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
                    m++;
                }
                for (i = 0; i < 65000; i++)
                    ;
            }
        }
        os_evt_set(0x0002, tsk2);
    }
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

void serial()
{
    PINSEL0 = 0x00000005;
    U0LCR = 0x83;
    U0DLL = 0x61;
    U0LCR = 0x03;
    U0IER = 0x01;
}

void delay(unsigned int x)
{

    unsigned int del;

    for (del = 0; del < x; del++)
        ;
}

int main(void)
{
    cnt1 = 0;
    cnt2 = 0;
    cnt3 = 0;
    IODIR0 = 0x000f00fc;
    IOCLR0 = 0x000f00fc;
    os_sys_init(job1); // initialize job1
    while (1)
        ;
}