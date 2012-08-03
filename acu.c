#include <stdint.h>

#include <inttypes.h>
#include <stdarg.h>

#include "board.h"
#include "uart.h"
#include "ownet.h"
#include "findtype.h"
#include "temp42.h"
#include "eep43.h"
#include "serdes.h"

#define MAXDEVICES 10 
#define ONEWIRE_PORT 0 
#define LED_REG 0x200
#define IFK_SETVAL 0x810

void _irq_entry(void) {}
void DisplaySerialNum(uchar sn[8]);
static volatile int* pio_reg = BASE_PIO;
static volatile char* scu_master = BASE_SCUB;
static int done_err[24] = {0};

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

  owInit();
  uart_init();
  return ;
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
      if (FamilySN[i-1][0] == 0x43)
      {
        owLevel(portnum, MODE_NORMAL); 
      }
      
      if (FamilySN[i-1][0] == 0x42)
      {
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
   int i = 0;

   for (i = 7; i>=1; i--)
     mprintf("%x:", (int)sn[i]);
   mprintf("%x", (int)sn[0]);
  
}

void printValues()
{
  mprintf("GXB\011DONE\011ERR\n");  
  mprintf("QL0_0: \011%d\011%d\n", done_err[QL0_DONE_0], done_err[QL0_ERR_0]);
  mprintf("QL0_1: \011%d\011%d\n", done_err[QL0_DONE_1], done_err[QL0_ERR_1]);
  mprintf("QL0_2: \011%d\011%d\n", done_err[QL0_DONE_2], done_err[QL0_ERR_2]);
  mprintf("QL0_3: \011%d\011%d\n", done_err[QL0_DONE_3], done_err[QL0_ERR_3]);
  
  mprintf("QL1_0: \011%d\011%d\n", done_err[QL1_DONE_0], done_err[QL0_ERR_0]);
  mprintf("QL1_1: \011%d\011%d\n", done_err[QL1_DONE_1], done_err[QL1_ERR_1]);
  mprintf("QL1_2: \011%d\011%d\n", done_err[QL1_DONE_2], done_err[QL1_ERR_2]);
  mprintf("QL1_3: \011%d\011%d\n", done_err[QL1_DONE_3], done_err[QL1_ERR_3]);
  
  mprintf("QL2_0: \011%d\011%d\n", done_err[QL2_DONE_0], done_err[QL2_ERR_0]);
  mprintf("QL2_1: \011%d\011%d\n", done_err[QL2_DONE_1], done_err[QL2_ERR_1]);
  mprintf("QL2 2: \011%d\011%d\n", done_err[QL2_DONE_2], done_err[QL2_ERR_2]);
  mprintf("QL2_3: \011%d\011%d\n", done_err[QL2_DONE_3], done_err[QL2_ERR_3]);
}

int addErrorBits() {
  int i;
  for (i = 0; i < PIO_SIZE; i++) {
    //accumulate the done and error bits
    if (*pio_reg & (1 << i))
      done_err[i]++;
   // mprintf("%d ", done_err[i]);
  } 
 // mprintf("\n");
  return 0;
}

int main() {
  int j;
  uint16_t i = 2;
  short val; 
  volatile uint16_t* regs;
  int slave_nr;
  int dir = 0;
  
  scu_init();
 

 // regs = scu_master;
 // regs[6] = 0xfff; //select all slaves


  //regs = scu_master + (0x9 << 17); //slave number 
  
  mprintf("testing...\n");
 
  while (1) { 
    regs = scu_master + (1  << 17);
    regs[IFK_SETVAL] = i;
    pio_reg[0] = i;


    i++;
    
    ////usleep(50);
  }

  return 0;
}

