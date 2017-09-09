# wykys Make file for AVR

NAME = kokos_step_motor_driver

TARGET_CHIP=atmega328p
TARGET_FAMILY=

CC=avr-gcc
CFLAGS = -mmcu=$(TARGET_CHIP)
CFLAGS += -DDEBUG
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -O0


OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump
BINSIZE = avr-size
AS = avr-as
RM = rm -rf

SRC = src/
DEP = dependence.list
OBJFILES = $(shell find ../$(SRC) -name '*.c' -printf '%f ' | sed -e 's/\.c/\.o/g')

.ONSHELL:

.PHONY: dependence clean

all: $(NAME)

# genereting dependence
dependence:
	$(CC) -MM ../$(SRC)*.c > $(DEP)
# include genereted dependence
-include $(DEP)
# create object modules
%.o : ../$(SRC)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME).elf: $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

$(NAME).hex: $(NAME).elf
	$(OBJCOPY) -R .eeprom -O ihex $(NAME).elf $(NAME).hex

$(NAME).lss: $(NAME).elf
	$(OBJDUMP)  -h -S kokos_step_motor_driver.elf > $@

$(NAME): $(NAME).elf $(NAME).hex $(NAME).lss
	$(BINSIZE) $(NAME).elf

clean:
	$(RM) *.o *.list *.elf *.hex *.lss
