/* this program demonstrate the usage of mailbox , task1 will count 0to15 and send 15 to task2 via mailbox
task2 will increment count value from 15 to 30 and then task1 resumes*/


#include <rtl.h>
#include <stdio.h>
#include <lpc214x.h>

unsigned int result, temp;
int adc();
void init_serial(void);
void delay(unsigned int);
os_mbx_declare (MsgBox, 100);                /* Declare an RTX mailbox  100 msgs with name MsgBox*/


/* Reserve a memory for 32 blocks of 20 bytes  */
_declare_box(mpool,20,32);


unsigned int cnt1,cnt2, brightness;
char arr1[20];
int i=0, k=0;


__task void task1 (void);
__task void task2 (void);


int adc(){
		int x;
        AD0CR = 0x09200010; // control register for adc     ad.04
        while((temp = AD0GDR) == 0x80000000);
        x = AD0GDR;      // to get the adc values in result
        AD0CR &= ~0x01000000; // to stop the ADC
        x = (x >> 6);
        x = x & 0x000003FF;	
		brightness = x / 4;	
			sprintf(arr1,"Brightness level :%d",brightness);
  			while (arr1[i] != '\0')
			{	os_dly_wait(1);
	  			while (!(U0LSR & 0x20));
	  			U0THR = arr1[i];
					i++;
			}
			  return x;
	}
	

__task void task1 (void)
{
	  
    /* This task will send a count value. */
    U32 *mptr;
	  init_serial();
    os_tsk_create (task2, 0);
    os_mbx_init (MsgBox, sizeof(MsgBox));
    mptr = _alloc_box (mpool);                /* Allocate a memory for the message */
	
	while(1)
    {
			result = adc();
				
			  	
			
			
			if (result < 0x00CC)  //  <  0.66
        {
            cnt1=0;
        }

        if (result > 0x00CC && result <= 0x0199)  // from 0.66 to 1.32
        {
            cnt1=1;
        }

        else if (result > 0x0199 && result <= 0x0266)	// from 1.32 to 1.98
        {
            cnt1 = 2;
        }

        else if (result > 0x0266 && result <= 0x0333)	//from 1.98 ro 2.64
        {
            cnt1 = 3;
        }

        else if(result > 0x0333 && result <= 0x03FF)	 //from 2.64 to 3.3
        {
            cnt1 = 4;
        }
					
        mptr[0] = cnt1;
        cnt1=0;
        os_mbx_send (MsgBox, mptr, 0xffff);	 /* Send the count value to a 'Mailbox' */
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
			
			if (cnt2 == 0)  //  <  0.66
        {
            // IOCLR0 = 0x00000000;
            IO0CLR = 0x000F0000;	   	//all led's off
					
        }

        else if (cnt2 == 1)  // from 0.66 to 1.32
        {
            IOSET0 = 0x00080000;
            IOCLR0 = 0x00070000;
            delay(100000);
        }

        else if (cnt2 == 2)	// from 1.32 to 1.98
        {
            IOSET0 = 0x000C0000;
            IOCLR0 = 0x00030000;
            delay(100000);
        }

        else if (cnt2 == 3)	//from 1.98 ro 2.64
        {
            IOSET0 = 0x000E0000;
            IOCLR0 = 0x00010000;
            delay(100000);
        }

        else if(cnt2 == 4)	 //from 2.64 to 3.3
        {
            IOSET0 = 0x000F0000;
            IOCLR0 = 0x00000000;
            delay(100000);
        }

        cnt2=0;
    }
}
void delay(unsigned int x){
	int k=0;
	for(k=0;k<x;k++);
}

int main (void)
{
    
    _init_box (mpool, sizeof(mpool), sizeof(U32));
    os_sys_init(task1);
}

/*----------------------------------------------------------------------------
*       init_serial:  Initialize Serial Interface
*---------------------------------------------------------------------------*/


void init_serial (void)
{
    PINSEL0 = 0X0000005;                 // Enable RxD0 and TxD0
    U0LCR = 0x83;                         // 8 bits, no Parity, 1 Stop bit
    U0DLL = 0x61;                           // 9600 Baud Rate @ 15MHz VPB Clock
    U0LCR = 0x03;                         // DLAB = 0
}




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

