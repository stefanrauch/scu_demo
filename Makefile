CC      := lm32-elf-gcc
CFLAGS  := -fdata-sections -ffunction-sections -Wall -mmultiply-enabled -mbarrel-shift-enabled -nostdlib  -Os -Iinclude -Iinclude/sockitowm -DCPU_CLOCK=125000 -Wl,--gc-sections

scu_demo.bin: scu_demo.elf
	lm32-elf-objcopy -O binary $< $@

scu_demo.elf: irq.o crt0.o serdes.o dev/uart.o dev/mprintf.o dev/sockitowm/crcutil.o dev/sockitowm/ownet.o dev/sockitowm/owtran.o dev/sockitowm/owlnk.o dev/sockitowm/findtype.o dev/sockitowm/temp42.o dev/sockitowm/eep43.o	
	$(CC) $(CFLAGS) -o $@ -T linker.ld $^

clean:
	rm -f *.o dev/*.o dev/sockitowm/*.o *.elf *.bin
