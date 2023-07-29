override KERNEL := $(BUILDDIR)kernel.elf
CC:=x86_64-elf-gcc
LD:=x86_64-elf-ld

CFLAGS ?= -g -O2 -pipe -Wimplicit-function-declaration

NASMFLAGS ?= -F dwarf -g -f elf64

LDFLAGS ?=

# Internal C flags that should not be changed by the user.
override CFLAGS +=       \
    -std=gnu11           \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-lto             \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD                 \
    -I.                  \
	-idirafter mlibc-headers \
    -Wall		         \
    -Wextra              \
    -masm=intel			

# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T linker.ld

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -f elf64

BUILDPATH:=$(BUILDDIR)
override CFILES := $(shell find . -type f -name '*.c')
override NASMFILES := $(shell find . -type f -name '*.asm')
override OBJ := $(addprefix $(BUILDDIR), $(CFILES:.c=.o) $(NASMFILES:.asm=.o))
override HEADER_DEPS:= $(addprefix $(BUILDDIR), $(CFILES:.c=.d) $(NASMFILES:.asm=.d))
DIRECTORY_GUARD = mkdir -p $(@D)

.PHONY: all
all: $(KERNEL)

$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@


-include $(HEADER_DEPS)

$(BUILDPATH)%.o: %.c 
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDPATH)%.o: %.asm
	$(DIRECTORY_GUARD)
	nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

