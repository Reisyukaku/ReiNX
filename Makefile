rwildcard = $(foreach d, $(wildcard $1*), $(filter $(subst *, %, $2), $d) $(call rwildcard, $d/, $2))

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_tools
LD = $(DEVKITARM)/bin/arm-none-eabi-ld
OBJCOPY = $(DEVKITARM)/bin/arm-none-eabi-objcopy

name := ReiNX
ver_major  := 2
ver_minor  := 3

dir_source := src
dir_data := data
dir_build := build
dir_out := out
dir_sysmod := NX_Sysmodules
dir_septchainloader := septchainloader

ARCH := -march=armv4t -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS = $(ARCH) -DVERSION_MAJOR='$(ver_major)' -DVERSION_MINOR='$(ver_minor)' -Os -nostdlib -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-inline -fno-builtin -std=gnu11# -Wall -Werror
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
	@$(MAKE) ver_maj=$(ver_major) ver_min=$(ver_minor) -C $(dir_sysmod)
	@mkdir -p "$(dir_out)/ReiNX/sysmodules"
	@mkdir -p "$(dir_out)/ReiNX/sysmodules.dis"
	@cp $(dir_sysmod)/loader/loader.kip $(dir_out)/ReiNX/sysmodules/
	@cp $(dir_sysmod)/sm/sm.kip $(dir_out)/ReiNX/sysmodules/
	@cp $(dir_sysmod)/pm/pm.kip $(dir_out)/ReiNX/sysmodules/
	@cp $(dir_sysmod)/rnx_mitm/rnx_mitm.kip $(dir_out)/ReiNX/sysmodules/
	@mkdir -p "$(dir_out)/ReiNX/titles/0100000000000034"
	@mkdir -p "$(dir_out)/ReiNX/titles/0100000000000036"
	@mkdir -p "$(dir_out)/ReiNX/titles/010000000000000D/flags"
	@cp $(dir_sysmod)/fatal/fatal.nsp $(dir_out)/ReiNX/titles/0100000000000034/exefs.nsp
	@cp $(dir_sysmod)/dmnt/dmnt.nsp $(dir_out)/ReiNX/titles/010000000000000D/exefs.nsp
	@cp $(dir_sysmod)/creport/creport.nsp $(dir_out)/ReiNX/titles/0100000000000036/exefs.nsp


$(dir_out)/$(name).bin: $(dir_build)/$(name).elf
	@mkdir -p "$(@D)"
	@mkdir -p "$(dir_out)/ReiNX/patches"
	@mkdir -p "$(dir_out)/sept"
	@$(MAKE) -C $(dir_septchainloader)
	@cp $(dir_septchainloader)/out/payload.bin $(dir_out)/sept
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
