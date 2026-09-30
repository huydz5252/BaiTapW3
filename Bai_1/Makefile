TARGET = bai1

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
STFLASH = st-flash

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -mfloat-abi=soft \
         -O0 \
         -Wall \
         -ffunction-sections \
         -fdata-sections

LDFLAGS = -Tstm32f103c8.ld \
          -Wl,--gc-sections \
          --specs=nosys.specs

OBJS = startup.o main.o

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

main.o: main.c stm32f10x.h
	$(CC) $(CFLAGS) -c main.c -o main.o

startup.o: startup.c stm32f10x.h
	$(CC) $(CFLAGS) -c startup.c -o startup.o

flash: $(TARGET).bin
	$(STFLASH) --reset write $(TARGET).bin 0x08000000

clean:
	rm -f *.o *.elf *.bin *.map

.PHONY: all flash clean
