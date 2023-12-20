#include <RTL.H>
#include <stdio.h>
#include <lpc21xx.h>

OS_TID tsk1, tsk2;
OS_RESULT RE1, RE2;
void __swi(8) ext(void);
void __swi(9) uart(void);

unsigned int num_ints = 0;
unsigned char mg[] = {"interrupt generated"};
// unsigned int num_ints2 = 0;

__task void job1(void);
__task void job2(void);

// int num_ints;
/*----------------------------------------------------------------------------
 * External 0 Interrupt Service Routine
 *---------------------------------------------------------------------------*/

void delay(unsigned int x)
{

  unsigned int del;

  for (del = 0; del < x; del++)
    ;
}

void serial()
{
  PINSEL0 = 0x00050000;		//P0.8 for UART1 -- TXD1      p0.9 RXD1 UART1
  U1LCR = 0x83;
  U1DLL = 0x61;
  U1LCR = 0x03;
  U1IER = 0x01;
}

void ext0_int(void) __irq
{
  isr_evt_set(0x0001, tsk1); /* Send event to 'process_task' */
  EXTINT = 0x01;             /* Acknowledge Interrupt */
  VICVectAddr = 0;
}

/*----------------------------------------------------------------------------
 * Task 'process_task'
 *---------------------------------------------------------------------------*/

__task void job1(void)
{
  tsk1 = os_tsk_self();
  tsk2 = os_tsk_create(job2, 1);
  while (1)
  {

    RE1 = os_evt_wait_and(0x0001, 0xffff);
    // num_ints++;
    if (RE1 == OS_R_EVT)
    {
      os_evt_set(0x0002, tsk2);
      ext();
    }
  }
}

__task void job2(void)
{

  while (1)
  {
    RE2 = os_evt_wait_and(0x0002, 0xffff);
    if (RE2 == OS_R_EVT)
    {
      serial();

      uart();
    }
  }
}

void __SWI_8(void)
{
	int i;
  i++;
}

void __SWI_9(void)
{
	int i;
  for (i = 0; i < 20; i++)
  {
    while (!(U1LSR & 0x20))
      ;
    U1THR = mg[i];
  }
}

/*----------------------------------------------------------------------------
 * Task 'init_task'
 *---------------------------------------------------------------------------*/

int main(void)
{
  PINSEL1 = 0x00000001;
  EXTMODE = 0x00000001;
  VICVectAddr0 = (U32)ext0_int;
  VICVectCntl0 = 0x20 | 14;
  VICIntEnable |= 0x00004000;
  os_sys_init(job1);

  while (1)
    ;
}