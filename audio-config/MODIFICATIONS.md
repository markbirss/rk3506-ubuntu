# SDK Modifications Log

Änderungen am rk3506-ubuntu SDK für PCM5102A Audio-Support.

## Modified Files

### 1. Kernel Configuration
**File:** `kernel-6.1/arch/arm/configs/rk3506_luckfox_defconfig`

**Changes:**
```diff
- CONFIG_SND_SOC_PCM5102A=m
+ CONFIG_SND_SOC_PCM5102A=y
+ CONFIG_OF_OVERLAY=y
```

**Reason:** 
- Built-in driver for immediate availability at boot
- Overlay support for runtime device tree modifications

---

### 2. Device Tree Build System
**File:** `kernel-6.1/arch/arm/boot/dts/Makefile`

**Changes:**
```makefile
# Added before Luckfox Lyra Zero W dtb entry:
DTC_FLAGS_rk3506b-luckfox-lyra-zero-w-sd := -@
```

**Reason:** Enable device tree symbol generation for overlay support

---

### 3. Base Device Tree (SAI1 Audio)
**File:** `kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-zero-w-sd.dts`

**Added Sections:**

#### SAI1 Controller Configuration
```dts
&sai1 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&rm_io8_sai1_mclk 
                 &rm_io7_sai1_sclk 
                 &rm_io6_sai1_lrck 
                 &rm_io4_sai1_sdo0>;
    assigned-clocks = <&cru MCLK_SAI1>;
    assigned-clock-rates = <12288000>;
    rockchip,clk-trcm = <1>;
    #sound-dai-cells = <0>;
};
```

#### Simple Audio Card
```dts
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

**Pin Mapping:**
- MCLK: GPIO0_B0 (rm_io8)
- SCLK: GPIO0_A7 (rm_io7)
- LRCK: GPIO0_A6 (rm_io6)
- SDO0: GPIO0_A4 (rm_io4)

**Source:** Based on [Luckfox Forum Post](https://forums.luckfox.com/viewtopic.php?p=7343)

---

### 4. Device Tree Overlays (Optional)
**Files Created:**
- `kernel-6.1/arch/arm/boot/dts/overlays/rk3506-pcm5102a-sai1.dts`
- `kernel-6.1/arch/arm/boot/dts/overlays/rk3506-pcm5102a-sai0.dts`

**Purpose:** 
- SAI1: Generic PCM5102A with MCLK
- SAI0: Pimoroni Audio DAC SHIM (no MCLK)

**Status:** Created but not required (audio in base DT works)

---

## New Files Created

### Scripts
1. **build-overlays.sh** - Compile device tree overlays
   - Location: `kernel-6.1/arch/arm/boot/dts/overlays/`
   - Uses C preprocessor for includes
   - Outputs .dtbo files

2. **load-overlay.sh** - Load overlay at runtime
   - Location: `audio-config/scripts/`
   - Uses ConfigFS interface
   - For testing without reflashing

3. **test-audio.sh** - Audio playback test
   - Location: `audio-config/scripts/`
   - Tests left/right channels
   - Uses speaker-test utility

### Configuration
1. **asound.conf** - ALSA default device
   - Location: `audio-config/`
   - Sets hw:0,0 as default
   - Deployed to `/etc/asound.conf` on device

### Documentation
1. **audio-config/README.md** - Complete documentation
2. **audio-config/QUICKSTART.md** - Quick rebuild guide
3. **audio-config/MODIFICATIONS.md** - This file

---

## System Configuration Requirements

### On Device (After Flash)

1. **User Permissions:**
   ```bash
   sudo usermod -aG audio lyra
   ```

2. **ALSA Configuration:**
   ```bash
   sudo cp audio-config/asound.conf /etc/asound.conf
   ```

3. **Reboot or Re-login:**
   Required for audio group to take effect

---

## Testing Verification

### 1. Driver Status
```bash
lsmod | grep pcm5102a  # If module
cat /sys/bus/platform/drivers/snd_soc_pcm5102a/module/refcnt
```

### 2. Sound Card Registration
```bash
cat /proc/asound/cards
# Expected: 0 [PCM5102ASAI1  ]: simple-card - PCM5102A-SAI1
```

### 3. ALSA Devices
```bash
aplay -l
# Expected: card 0: PCM5102ASAI1, device 0: ff310000.sai-pcm5102a-hifi
```

### 4. Playback Test
```bash
speaker-test -D hw:0,0 -t sine -f 1000 -c 2
```

---

## Build Impact

### Affected Components
- ✅ Kernel (driver + device tree)
- ✅ Device Tree Blob (base + overlays)
- ❌ U-Boot (no changes)
- ❌ Rootfs (only user/permissions)

### Build Time
- Incremental: ~2-5 minutes (kernel + DTB)
- Full rebuild: ~30-60 minutes

### Image Size
- No significant increase (driver already in kernel)
- +~10KB for device tree changes

---

## Alternative Configurations

### Pimoroni Audio DAC SHIM (SAI0)

**Different from SAI1:**
- No MCLK required (internal PLL)
- Different pin mapping
- GPIO enable pin
- Lower pin count

**Modifications Required:**
- Replace `&sai1` with `&sai0` in base DTS
- Use different pinctrl (rm_io16, rm_io14, rm_io18)
- Add regulator-fixed for enable pin (rm_io11)

**Status:** Configuration ready, not tested yet

---

## Rollback Process

To remove audio support:

1. **Revert Kernel Config:**
   ```bash
   cd kernel-6.1
   sed -i 's/CONFIG_SND_SOC_PCM5102A=y/CONFIG_SND_SOC_PCM5102A=m/' \
       arch/arm/configs/rk3506_luckfox_defconfig
   ```

2. **Remove DTS Changes:**
   ```bash
   git checkout arch/arm/boot/dts/rk3506b-luckfox-lyra-zero-w-sd.dts
   ```

3. **Rebuild:**
   ```bash
   cd ..
   ./build.sh
   ```

---

## Version History

**2025-02-12:** Initial implementation
- SAI1 support with forum corrections
- SAI0 Pimoroni variant created
- Overlay infrastructure added
- Documentation completed
- Testing verified: ✅ SAI1 working

---

## References

1. [Luckfox Forum: PCM5102A Configuration](https://forums.luckfox.com/viewtopic.php?p=7343)
2. [Pimoroni Audio DAC SHIM Product](https://shop.pimoroni.com/products/audio-dac-shim-line-out)
3. [PCM5102A Datasheet (Texas Instruments)](https://www.ti.com/product/PCM5102A)
4. [Linux ALSA Simple Audio Card](https://www.kernel.org/doc/html/latest/sound/soc/codec-to-codec.html)
5. [Rockchip SAI Driver Documentation](https://opensource.rock-chips.com/wiki_Audio)

---

## Future Improvements

- [ ] Test SAI0 (Pimoroni) configuration
- [ ] Add volume control support (external amplifier)
- [ ] Investigate simultaneous SAI0+SAI1 usage
- [ ] PulseAudio/PipeWire integration
- [ ] Automatic audio group assignment in rootfs build
