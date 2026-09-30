TARGET = bai2

CC = arm-none-eabi-gcc

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -mfloat-abi=soft \
         -O0 \
         -g \
         -Wall \
         -Wextra \
         -ffreestanding \
         -ffunction-sections \
         -fdata-sections \
         -I.

LDFLAGS = -T stm32f103c8.ld \
          -nostartfiles \
          -Wl,--gc-sections \
          -Wl,-Map=$(TARGET).map

C_SOURCES = main.c startup.c

OBJECTS = $(C_SOURCES:.c=.o)

all: $(TARGET).elf $(TARGET).bin $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	arm-none-eabi-objcopy -O binary $< $@

$(TARGET).hex: $(TARGET).elf
	arm-none-eabi-objcopy -O ihex $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.bin *.hex *.map
