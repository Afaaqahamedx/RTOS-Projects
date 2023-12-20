

#include <RTL.h>                      /* RTX kernel functions & defines      */
#include <LPC21xx.h>                  /* LPC21xx definitions                 */
#include <stdio.h>

__task void task1 (void);
__task void task2(void);
__task void task3(void);


void delay();
void init_serial (void);       /* Initialize Serial Interface         */

unsigned char msg1[]="one-1\r\n", msg2[]="two-2\r\n", msg3[]="three-3\r\n";

unsigned int j,i=0;

OS_TID tsk1, tsk2, tsk3;
OS_RESULT ret1;

OS_SEM semaphore1;

/*----------------------------------------------------------------------------
*    Task 1 - High Priority -
*---------------------------------------------------------------------------*/

__task void task1 (void)
{
    OS_RESULT ret;
    os_tsk_prio_self(5);
    tsk2 = os_tsk_create (task2, 0);

    //while (1)
    //{

    //  ret = os_sem_wait (semaphore1, 0x0f);
    // if (ret == OS_R_SEM)
    //{
    while (msg1[i] != '\0')
    {
        while (!(U0LSR & 0x20));
        U0THR = msg1[i];
        i++;

    }
    delay();
    i=0;


    os_tsk_prio(tsk2, 3);
    os_tsk_prio(tsk3, 2);
    os_tsk_prio_self(1);

    /* Return a token back to a semaphore */
    os_sem_send (semaphore1);

}
// }
//}


/*----------------------------------------------------------------------------
 *    Task 2 - Low Priority - looks for a free semaphore and uses the resource
 *                            whenever it is available
 *---------------------------------------------------------------------------*/


__task void task2 (void)
{
    tsk3 = os_tsk_create (task3, 0);
    while (1)
    {

        /* Wait for a free semaphore */

        /* OK, the serial interface is free now, use it. */

        while (msg2[i] != '\0')
        {
            while (!(U0LSR & 0x20));
            U0THR = msg2[i];
            i++;

        }
        delay();
        i=0;

        /* Return a token back to a semaphore. */
        os_tsk_prio(tsk3, 3);
        os_tsk_prio(tsk1, 2);
        //os_tsk_prio(tsk2, 1);
        os_tsk_prio_self(1);
				os_sem_send (semaphore1);
    }
}


__task void task3 (void)
{
    while (1)
    {
        /* Wait for a free semaphore */

        /* OK, the serial interface is free now, use it. */

        while (msg3[i] != '\0')
        {
            while (!(U0LSR & 0x20));
            U0THR = msg3[i];
            i++;
        }
        delay();
        i=0;

        os_tsk_prio(tsk1, 3);
        os_tsk_prio(tsk2, 2);
        os_tsk_prio_self(1);
        /* Return a token back to a semaphore. */
        os_sem_send (semaphore1);
    }
}


/*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/


void init_serial (void)
{
    PINSEL0 = 0X0000005;                 // Enable RxD0 and TxD1
    U0LCR = 0x83;                         // 8 bits, no Parity, 1 Stop bit
    U0DLL = 0x61;                           // 9600 Baud Rate @ 15MHz VPB Clock
    U0LCR = 0x03;                         // DLAB = 0
}

void delay(void)
{
    int j=0,i;
    for(i=0; i<1000000; i++)
    {
        j++;
    }
}

/*----------------------------------------------------------------------------
 *    Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/


int main (void)
{
    init_serial();

    /* Initialize the Semaphore before the first use */
    os_sem_init (semaphore1, 1);

    os_sys_init (task1);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

