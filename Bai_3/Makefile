TARGET = bai3

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
STFLASH = st-flash

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -g \
         -Wall \
         -ffreestanding \
         -mfloat-abi=soft \
         -DSTM32F10X_MD \
         -I.

LDFLAGS = -T stm32f103c8.ld \
          -nostartfiles \
          -Wl,--gc-sections

SOURCES = main.c startup.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET).bin

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(TARGET).bin
	$(STFLASH) --reset write $(TARGET).bin 0x08000000

clean:
	rm -f *.o *.elf *.bin *.map
