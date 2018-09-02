rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

CC = $(DEVKITARM)/bin/arm-none-eabi-gcc
LD = $(DEVKITARM)/bin/arm-none-eabi-ld
OBJCOPY = $(DEVKITARM)/bin/arm-none-eabi-objcopy

name := ReiNX
ver  := 1.6

dir_source := src
dir_data := data
dir_build := build
dir_out := out
dir_sysmod := NX_Sysmodules

ARCH := -march=armv4t -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS = $(ARCH) -DVERSION='"$(ver)"' -Os -nostdlib -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-inline -fno-builtin -std=gnu11# -Wall
LDFLAGS = $(ARCH) -nostartfiles -lgcc -Wl,--nmagic,--gc-sections

objects =	$(patsubst $(dir_source)/%.s, $(dir_build)/%.o, \
			$(patsubst $(dir_source)/%.c, $(dir_build)/%.o, \
			$(call rwildcard, $(dir_source), *.s *.c)))

define bin2o
	bin2s $< | $(AS) -o $(@)
endef

.PHONY: all
all: sysmod reinx

.PHONY: sysmod
sysmod: $(dir_out)/sysmodules

.PHONY: reinx
reinx: $(dir_out)/$(name).bin

.PHONY: clean
clean:
	@echo "clean ..."
	@rm -rf $(dir_build)
	@rm -rf $(dir_out)
	@$(MAKE) -C $(dir_sysmod) clean

$(dir_out)/sysmodules: $(dir_sysmod)
	@$(MAKE) -C $(dir_sysmod)

$(dir_out)/$(name).bin: $(dir_build)/$(name).elf
	@mkdir -p "$(@D)"
	@mkdir -p "$(dir_out)/ReiNX/sysmodules"
	@mkdir -p "$(dir_out)/ReiNX/patches"
	@cp $(dir_sysmod)/loader/loader.kip $(dir_out)/ReiNX/sysmodules/
	@cp $(dir_sysmod)/sm/sm.kip $(dir_out)/ReiNX/sysmodules/
	@cp -R $(dir_data)/*.bin $(dir_out)/ReiNX/
	@cp -R $(dir_data)/*.rxp $(dir_out)/ReiNX/patches
	$(OBJCOPY) -S -O binary $< $@

$(dir_build)/$(name).elf: $(objects)
	$(CC) $(LDFLAGS) -T link.ld $^ -o $@
	
$(dir_build)/%.o: $(dir_source)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@

$(dir_build)/%.o: $(dir_source)/%.s
	@mkdir -p "$(@D)"
	$(CC) $(CFLAGS) -c $< -o $@
