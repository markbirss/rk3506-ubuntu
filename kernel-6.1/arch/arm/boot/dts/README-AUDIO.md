# Audio DAC Support for Luckfox Lyra Zero W

This directory contains Device Tree configurations for audio DAC support on the Luckfox Lyra Zero W.

## Configurations

### 1. PCM5102A on SAI1 (Generic DAC)
**File:** `rk3506b-luckfox-lyra-zero-w-sd.dts`

Generic PCM5102A DAC configuration using SAI1 interface.

**Pin Mapping (rm_io pins):**
- rm_io8 → SAI1_MCLK (Master Clock)
- rm_io7 → SAI1_SCLK (Bit Clock)
- rm_io6 → SAI1_LRCK (Left/Right Clock)
- rm_io4 → SAI1_SDO0 (Data Out)

**Features:**
- 24-bit / 192kHz audio support
- I2S format
- 12.288 MHz master clock
- Simple audio card driver

**Device Tree Nodes:**
```dts
pcm5102a_codec: PCM5102A codec
pcm5102a_sound: Simple audio card named "PCM5102A-SAI1"
&sai1: SAI1 interface configuration
```

### 2. Pimoroni Audio DAC SHIM (PCM5100A)
**File:** `rk3506b-luckfox-lyra-zero-w-sd-pimoroni.dts`

Optimized configuration for the Pimoroni Audio DAC SHIM using SAI0 interface.

**Pin Mapping (rm_io pins):**
- rm_io16 → SAI0_LRCK (Left/Right Clock / WS)
- rm_io14 → SAI0_SCLK (Bit Clock / BCK)
- rm_io18 → SAI0_SDO (Data Out / DIN)
- rm_io11 → DAC Enable (GPIO0_B3, Active High)

**Features:**
- PCM5100A DAC (compatible with PCM5102A driver)
- No external MCLK required (internally generated)
- Hardware enable control via GPIO
- I2S format
- 12.288 MHz system clock
- Simple audio card driver

**Device Tree Nodes:**
```dts
pimoroni_dac_enable: Fixed regulator for DAC enable pin
pcm5100a_codec: PCM5100A codec (using PCM5102A driver)
pimoroni_sound: Simple audio card named "Pimoroni-Audio-DAC"
&sai0: SAI0 interface configuration
```

**Special Notes:**
- The Pimoroni Audio DAC SHIM uses the PCM5100A chip, which is compatible with the PCM5102A driver
- The enable pin (rm_io11) must be pulled high for the DAC to function
- The PCM5100A has an integrated PLL and generates SCK internally from BCK, so no external MCLK is needed

## Building and Installing

### Step 1: Choose Configuration

Edit the main DTS file or create a symbolic link:

```bash
cd /home/nadlech/privat/luckfox/rk3506-ubuntu/kernel-6.1/arch/arm/boot/dts/

# For generic PCM5102A (already default):
# rk3506b-luckfox-lyra-zero-w-sd.dts

# For Pimoroni:
cp rk3506b-luckfox-lyra-zero-w-sd-pimoroni.dts rk3506b-luckfox-lyra-zero-w-sd.dts
# (backup original first!)
```

### Step 2: Build Complete Image

```bash
cd /home/nadlech/privat/luckfox/rk3506-ubuntu
./build.sh
```

This will:
1. Compile the kernel (if needed)
2. Compile the device tree
3. Build the root filesystem
4. Create the flashable image

### Step 3: Flash to Device

Use the Rockchip flash tool or:

```bash
# Method 1: Using rkflash.sh
sudo ./rkflash.sh

# Method 2: Manual flash of specific partitions
# (Advanced - requires knowledge of partition layout)
```

### Step 4: Test Audio

After booting the device:

```bash
# Check if sound card is detected
cat /proc/asound/cards

# Should show either:
# - "PCM5102A-SAI1" (for generic config)
# - "Pimoroni-Audio-DAC" (for Pimoroni config)

# List audio devices
aplay -l

# Play test sound (if available)
speaker-test -t wav -c 2

# Or play an audio file
aplay /path/to/test.wav
```

## Kernel Module Requirements

The following kernel options must be enabled:

```
CONFIG_SND=y
CONFIG_SND_SOC=y
CONFIG_SND_SOC_ROCKCHIP=y
CONFIG_SND_SOC_ROCKCHIP_SAI=y
CONFIG_SND_SIMPLE_CARD=y
CONFIG_SND_SOC_PCM5102A=y
```

Check if modules are loaded:

```bash
lsmod | grep -E "snd|pcm|sai"
```

If modules are not automatically loaded, load them manually:

```bash
sudo modprobe snd-soc-simple-card
sudo modprobe snd-soc-pcm5102a
```

## Troubleshooting

### Audio card not detected

1. **Check kernel messages:**
   ```bash
   dmesg | grep -i "audio\|snd\|sai\|pcm"
   ```

2. **Verify device tree is loaded:**
   ```bash
   find /proc/device-tree -name "*pcm*" -o -name "*sai*" -o -name "*pimoroni*"
   ```

3. **Check SAI device:**
   ```bash
   ls -la /sys/bus/platform/devices/ | grep sai
   ```

4. **Verify pinmux configuration:**
   ```bash
   cat /sys/kernel/debug/pinctrl/pinctrl/pinmux-pins | grep sai
   ```

### No sound output

1. **Check ALSA mixer settings:**
   ```bash
   alsamixer
   # Unmute and increase volume
   ```

2. **Test with speaker-test:**
   ```bash
   speaker-test -t sine -f 1000 -c 2
   ```

3. **Set default audio device:**
   ```bash
   # Create /etc/asound.conf
   defaults.pcm.card 0
   defaults.ctl.card 0
   ```

4. **For Pimoroni: Verify enable pin:**
   ```bash
   cat /sys/class/gpio/export
   echo 11 > /sys/class/gpio/export
   cat /sys/class/gpio/gpio11/value  # Should be 1
   ```

### Build errors

1. **Missing cross-compiler:**
   ```bash
   export PATH=/home/nadlech/privat/luckfox/rk3506-ubuntu/prebuilts/gcc/linux-x86/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin:$PATH
   ```

2. **Device tree compilation errors:**
   - Check for syntax errors in DTS files
   - Verify all referenced nodes exist (sai0/sai1, cru, gpio0)
   - Ensure pinctrl definitions are available

## Hardware Connections

### Generic PCM5102A
```
Luckfox Pin    →  PCM5102A
────────────      ─────────
rm_io8 (MCLK)  →  SCK
rm_io7 (SCLK)  →  BCK
rm_io6 (LRCK)  →  LRCK
rm_io4 (SDO0)  →  DIN
3.3V           →  VDD
GND            →  GND
```

### Pimoroni Audio DAC SHIM
```
Luckfox Pin     →  Pimoroni Header
─────────────      ───────────────
rm_io16 (LRCK)  →  GPIO 19 (WS)
rm_io14 (SCLK)  →  GPIO 18 (BCK)
rm_io18 (SDO)   →  GPIO 21 (DIN)
rm_io11 (EN)    →  Enable (pulled high internally)
3.3V            →  VCC (via header)
GND             →  GND (via header)
```

**Note:** The Pimoroni Audio DAC SHIM is designed to fit directly onto Raspberry Pi GPIO headers. For Luckfox, manual wiring to the rm_io pins is required.

## References

- [Luckfox Forum: PCM5102A Discussion](https://forums.luckfox.com/viewtopic.php?p=7343)
- [Pimoroni Audio DAC SHIM Product Page](https://shop.pimoroni.com/products/audio-dac-shim-line-out)
- [PCM5102A Datasheet](https://www.ti.com/product/PCM5102A)
- [PCM5100A Datasheet](https://www.ti.com/product/PCM5100A)

## Author

Christopher Nadler (2024)

## License

SPDX-License-Identifier: MIT / GPL-2.0+
