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

.PHONY: dependence clean

# build all
all: $(NAME)
# genereting dependence
$(DEP): ../$(SRC)*.c
	$(CC) -MM ../$(SRC)*.c > $(DEP)
# include genereted dependence
-include $(DEP)
# create object modules
%.o : ../$(SRC)%.c $(DEP)
	$(CC) $(CFLAGS) -c $< -o $@
# create ELF
$(NAME).elf: $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@
# create HEX
$(NAME).hex: $(NAME).elf
	$(OBJCOPY) -R .eeprom -O ihex $(NAME).elf $(NAME).hex
# disassembly EFL
$(NAME).lss: $(NAME).elf
	$(OBJDUMP)  -h -S kokos_step_motor_driver.elf > $@
# analyze size ELF
$(NAME): $(NAME).elf $(NAME).hex $(NAME).lss
	$(BINSIZE) $(NAME).elf
# clean files
clean:
	$(RM) *.o *.list *.elf *.hex *.lss
