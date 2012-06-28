#include <stdint.h>

#include <inttypes.h>
#include <stdarg.h>

#include "board.h"
#include "uart.h"
#include "ownet.h"
#include "findtype.h"
#include "temp42.h"

#define MAXDEVICES 10 
#define ONEWIRE_PORT 0 

void _irq_entry(void) {}
void DisplaySerialNum(uchar sn[8]);


void msDelay(int msecs) {
	int i;
	for(i = msecs * CPU_CLOCK/4; i > 0; i--)
		asm("# noop");
}

void scu_init()
{
  	uchar FamilySN[MAXDEVICES][8];
	int current_temp;
	int c_frac;
  	int i = 0;
  	int j = 0;
	int cnt = 0;
  	int NumDevices = 0;
  	SMALLINT didRead = 0;
	uchar read_buffer[32];
	uchar write_buffer[32];

	uart_init();
	uart_write_string("SCU\n");
 
  	//use port number for 1-wire
  	uchar portnum = ONEWIRE_PORT;
     j = 0;
     // Find the device(s)
     NumDevices  = 0;
     NumDevices += FindDevices(portnum, &FamilySN[NumDevices], 0x42, MAXDEVICES-NumDevices);
     NumDevices += FindDevices(portnum, &FamilySN[NumDevices], 0x20, MAXDEVICES-NumDevices);
     NumDevices += FindDevices(portnum, &FamilySN[NumDevices], 0x43, MAXDEVICES-NumDevices);
     if (NumDevices)
     {
        mprintf("\r\n");

        // read the temperature and print serial number and temperature
        for (i = NumDevices; i; i--)
        {
           mprintf("(%d) ", j++);
           DisplaySerialNum(FamilySN[i-1]);
           if (FamilySN[i-1][0] == 0x43) {
//		if(!Write43(portnum, FamilySN[i-1], &write_buffer))
//			mprintf("write failed!\n");
	  	owLevel(portnum, MODE_NORMAL); 
		if (ReadMem43(portnum, FamilySN[i-1], &read_buffer))
		{
			for(cnt = 0; cnt < 32; cnt++)
			{
				mprintf("read_buffer[%x]: %x\n",cnt, read_buffer[cnt]);
			}
		}
		continue;
	   }

	   if (FamilySN[i-1][0] == 0x42) {
             didRead = ReadTemperature42(portnum, FamilySN[i-1],&current_temp,&c_frac);
	   }
           if (didRead)
           {
             	mprintf(" %d",current_temp);
		if (c_frac)
			mprintf(".5");
		else
			mprintf(".0");
		mprintf(" deegree celsius\r\n");
	   }
           else
           {
             mprintf("  Convert failed.  Device is");
             if(!owVerify(portnum, FALSE))
                mprintf(" not");
             mprintf(" present.\r\n");
#ifdef SOCKIT_OWM_ERR_ENABLE
             while(owHasErrors())
                 mprintf("  - Error %d\r\n", owGetErrorNum());
#endif
           }
 
        }
     }
     else
        mprintf("No temperature devices found!\r\n");
	
}

// -------------------------------------------------------------------------------
// Read and print the serial number.
//
void DisplaySerialNum(uchar sn[8])
{
   int i;
   for (i = 7; i>=1; i--)
     mprintf("%x:", (int)sn[i]);
   mprintf("%x", (int)sn[0]);
	
}

int main(void)
{
	scu_init();
	
	while(1) {

	}
}
