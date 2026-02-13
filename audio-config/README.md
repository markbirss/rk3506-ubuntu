# PCM5102A Audio DAC Configuration for Luckfox Lyra Zero W

This directory contains the configuration files and overlays for using PCM5102A/PCM5100A audio DACs with the Luckfox Lyra Zero W board.

## Quick Start

After flashing the image with correct device tree:

```bash
# 1. Setup audio system (from host machine)
cd audio-config/scripts
./setup-audio-system.sh                    # Default: lyra@192.168.123.100
# or: ./setup-audio-system.sh user@IP      # Custom target

# 2. Reboot board or logout/login for audio group to take effect

# 3. Verify configuration
./verify-audio-remote.sh                   # Comprehensive diagnostics

# 4. Test audio
ssh lyra@192.168.123.100 'speaker-test -D hw:0,0 -t sine -f 1000 -c 2'
```

**Scripts accept optional SSH target as first argument. Default: `lyra@192.168.123.100`**

---

## Hardware Configurations

### SAI1 - Generic PCM5102A
Standard PCM5102A DAC with MCLK support.

**Pin Mapping:**
- MCLK: GPIO0_B0 (rm_io8)
- SCLK: GPIO0_A7 (rm_io7)  
- LRCK: GPIO0_A6 (rm_io6)
- SDO:  GPIO0_A4 (rm_io4)

**Features:**
- Master clock: 12.288 MHz
- TDM slots: 32 (64 bits total)
- Sample rates: 8-384 kHz
- Formats: S16_LE, S24_LE, S32_LE

### SAI0 - Pimoroni Audio DAC SHIM
Pimoroni Audio DAC SHIM with PCM5100A (no external MCLK needed).

**Pin Mapping:**
- LRCK:   GPIO0_C0 (rm_io16) - RM_IO pin 7
- SCLK:   GPIO0_B6 (rm_io14) - RM_IO pin 5  
- SDO:    GPIO0_C2 (rm_io18) - RM_IO pin 9
- Enable: GPIO0_B3 (rm_io11) - RM_IO pin 2

**Features:**
- No MCLK required (internal PLL)
- GPIO-controlled power enable
- Optimized for Pimoroni SHIM pinout

## Directory Structure

```
audio-config/
├── asound.conf                  # ALSA default device configuration
├── DIAGNOSIS.md                 # Detailed diagnostic report
├── QUICKSTART.md                # Quick rebuild guide
├── MODIFICATIONS.md             # SDK changes log
├── overlays/
│   ├── rk3506-pcm5102a-sai0.dts  # Pimoroni SHIM overlay source
│   └── rk3506-pcm5102a-sai1.dts  # Generic PCM5102A overlay source
└── scripts/
    ├── setup-audio-system.sh     # Configure user & ALSA on board (via SSH)
    ├── verify-audio-remote.sh    # Verify configuration remotely (via SSH)
    ├── verify-audio-config.sh    # Verify locally on board
    ├── build-overlays.sh         # Compile overlays to .dtbo
    ├── load-overlay.sh           # Load overlay at runtime
    └── test-audio.sh             # Test audio playback
```

## Build Instructions

### 1. Modify Kernel Configuration

Enable overlay support and built-in PCM5102A driver:

```bash
# Edit kernel config
cd kernel-6.1
make ARCH=arm menuconfig

# Enable these options:
# CONFIG_OF_OVERLAY=y
# CONFIG_SND_SOC_PCM5102A=y (change from =m to =y)

# Or directly edit arch/arm/configs/rk3506_luckfox_defconfig:
sed -i 's/CONFIG_SND_SOC_PCM5102A=m/CONFIG_SND_SOC_PCM5102A=y/' arch/arm/configs/rk3506_luckfox_defconfig
echo 'CONFIG_OF_OVERLAY=y' >> arch/arm/configs/rk3506_luckfox_defconfig
```

### 2. Enable Device Tree Symbol Support

Edit `kernel-6.1/arch/arm/boot/dts/Makefile`:

```makefile
# Find the line with your board DTS and add -@ flag:
DTC_FLAGS_rk3506b-luckfox-lyra-zero-w-sd := -@

# Add before the dtb-$(CONFIG_...) line for your board
```

### 3. Add Audio to Base Device Tree

For **SAI1** (Generic PCM5102A), edit `kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-zero-w-sd.dts`:

```dts
&sai1 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&rm_io8_sai1_mclk &rm_io7_sai1_sclk &rm_io6_sai1_lrck &rm_io4_sai1_sdo0>;
    assigned-clocks = <&cru MCLK_SAI1>;
    assigned-clock-rates = <12288000>;
    rockchip,clk-trcm = <1>;
    #sound-dai-cells = <0>;
};

/ {
    sound_pcm5102a_sai1: sound-pcm5102a-sai1 {
        compatible = "simple-audio-card";
        simple-audio-card,name = "PCM5102A-SAI1";
        simple-audio-card,format = "i2s";
        simple-audio-card,mclk-fs = <256>;
        
        simple-audio-card,cpu {
            sound-dai = <&sai1>;
            system-clock-frequency = <12288000>;
            dai-tdm-slot-num = <32>;
            dai-tdm-slot-width = <64>;
        };
        
        simple-audio-card,codec {
            sound-dai = <&pcm5102a_sai1>;
        };
    };
    
    pcm5102a_sai1: pcm5102a-sai1 {
        compatible = "ti,pcm5102a";
        #sound-dai-cells = <0>;
    };
};
```

For **SAI0** (Pimoroni), replace `sai1` with `sai0` and use corresponding pins.

### 4. Compile Overlays (Optional)

If you want to use overlays instead of base DT:

```bash
cd audio-config/overlays
../scripts/build-overlays.sh

# This creates:
# - rk3506-pcm5102a-sai0.dtbo
# - rk3506-pcm5102a-sai1.dtbo
```

Copy to device:
```bash
scp *.dtbo lyra@192.168.123.100:/boot/overlays/
```

### 5. Build Complete Image

```bash
cd /path/to/rk3506-ubuntu
./build.sh lunch
# Select: 6. luckfox_lyra_zero-w_ubuntu_sdmmc
./build.sh
```

### 6. Flash to SD Card

```bash
sudo ./rkflash.sh update
```

## System Configuration

### Quick Setup (Automated via SSH)

Run the setup script from your host machine:
```bash
cd audio-config/scripts

# Setup user permissions and ALSA config (default: lyra@192.168.123.100)
./setup-audio-system.sh

# Or specify custom SSH target:
./setup-audio-system.sh user@192.168.123.50
```

This script automatically:
- Adds user to audio group
- Deploys /etc/asound.conf
- Verifies configuration

### Manual Setup

If you prefer manual setup on the board:

#### 1. User Permissions
```bash
sudo usermod -aG audio lyra
# Logout and login for changes to take effect
```

#### 2. ALSA Configuration
```bash
cat | sudo tee /etc/asound.conf > /dev/null <<'EOF'
# Default ALSA configuration for PCM5102A-SAI1
pcm.!default {
    type hw
    card 0
    device 0
}

ctl.!default {
    type hw
    card 0
}
EOF
```

## Testing

### Automated Verification (via SSH)

Run comprehensive diagnostics from your host machine:
```bash
cd audio-config/scripts

# Verify configuration (default: lyra@192.168.123.100)
./verify-audio-remote.sh

# Or specify custom SSH target:
./verify-audio-remote.sh user@192.168.123.50
```

This checks:
- ✓ Sound card registration
- ✓ Device tree properties (all required properties present)
- ✓ Pin configuration (4 pins including MCLK)
- ✓ TDM values (32 slots × 64 bits)
- ✓ User permissions (audio group)
- ✓ ALSA configuration
- ✓ Audio device access (tests if speaker-test hangs)

### Manual Testing

If connected to the board directly:

#### 1. Verify Sound Card
```bash
# Check if card is registered
cat /proc/asound/cards

# Expected output:
# 0 [PCM5102ASAI1  ]: simple-card - PCM5102A-SAI1
#                      PCM5102A-SAI1

# List playback devices
aplay -l
```

#### 2. Test Audio Playback
```bash
# Play 1kHz sine wave for 5 seconds
speaker-test -D hw:0,0 -t sine -f 1000 -c 2 -l 5

# Or use provided test script
/tmp/test-audio.sh
```

#### 3. Play Audio File
```bash
# WAV file
aplay -D hw:0,0 audiofile.wav

# MP3 (requires mpg123)
mpg123 -a hw:0,0 audiofile.mp3
```

## Troubleshooting

### No Sound Card Found

```bash
# Check if driver is loaded
lsmod | grep pcm5102a
cat /sys/bus/platform/drivers/snd_soc_pcm5102a/module/refcnt

# Check device tree
ls /sys/firmware/devicetree/base/sound*

# Check pinmux
cat /sys/kernel/debug/pinctrl/pinctrl/pinmux-pins | grep sai1
```

### Permission Denied

```bash
# Check user in audio group
groups
# Should show: lyra audio ...

# Check device permissions
ls -l /dev/snd/
# Should show: crw-rw---- 1 root audio ...

# If not in audio group:
sudo usermod -aG audio $USER
# Then logout and login
```

### No Audio Output

1. Check connections (especially GND)
2. Verify DAC power supply (3.3V)
3. Check volume (PCM5102A has no software volume control)
4. Test with speaker-test first before audio files
5. Some PCM5102A boards need SCK pin grounded

## Hardware Connections

### PCM5102A Module Wiring

| PCM5102A Pin | Lyra Pin | Function |
|--------------|----------|----------|
| VCC          | 3.3V     | Power    |
| GND          | GND      | Ground   |
| SCK (Filter) | GND      | Filter bypass |
| BCK          | rm_io7   | Bit Clock |
| DIN          | rm_io4   | Data In  |
| LCK          | rm_io6   | L/R Clock |
| XMT          | 3.3V/GND | Mute control (3.3V=unmute) |

### Pimoroni Audio DAC SHIM Wiring

The Pimoroni SHIM is designed for direct connection via female headers. Follow the silkscreen markings on the SHIM for orientation.

| SHIM Pin | Lyra Pin | Function |
|----------|----------|----------|
| VCC      | 3.3V     | Power    |
| GND      | GND      | Ground   |
| BCK      | rm_io14  | Bit Clock |
| DIN      | rm_io18  | Data In  |
| LRCK     | rm_io16  | L/R Clock |
| EN       | rm_io11  | Enable   |

## References

- [Luckfox Forum: PCM5102A on SAI1](https://forums.luckfox.com/viewtopic.php?p=7343)
- [Pimoroni Audio DAC SHIM](https://shop.pimoroni.com/products/audio-dac-shim-line-out)
- [PCM5102A Datasheet](https://www.ti.com/product/PCM5102A)
- [Rockchip SAI Documentation](https://opensource.rock-chips.com/wiki_Audio)

## Contributing

Tested configurations:
- ✅ SAI1 with generic PCM5102A module (2025-02-12)
- ⏳ SAI0 with Pimoroni Audio DAC SHIM (untested)

Please report working/non-working hardware combinations!
