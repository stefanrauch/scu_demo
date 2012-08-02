#ifndef __BOARD_H
#define __BOARD_H

//#define CPU_CLOCK 125000000ULL

#define BASE_PIO	0x100400
#define BASE_UART  	0x100700
#define BASE_ONEWIRE  	0x100600
#define BASE_SCUB 0x400000
//#define BASE_ONEWIRE 0x20600 //0x20600

#define FMC_EEPROM_ADR 0x50

#define UART_BAUDRATE 115200ULL /* not a real UART */

static inline void delay(int x)
{
  while(x--) asm volatile("nop");
}

static inline void usleep(int x)
{
  int i;
  for (i = x * CPU_CLOCK/4; i > 0; i--) asm("# noop");
}  
#endif
