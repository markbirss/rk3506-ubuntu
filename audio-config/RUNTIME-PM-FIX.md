# SAI1 Runtime PM Issue - Complete Analysis

## Problem Summary

Audio playback on SAI1 (PCM5102A) was hanging indefinitely. After deep analysis, the root cause was identified: **Runtime Power Management keeps the SAI device in suspended state, preventing clock activation.**

## Root Cause Analysis

### Symptoms
- ✓ Sound card registers correctly: `PCM5102A-SAI1`
- ✓ Device tree configuration is complete and correct
- ✓ Driver (`rockchip-sai`) is bound to the device
- ✓ All 6 critical DT properties are present with correct values
- ✗ **speaker-test hangs after setting hardware parameters**
- ✗ **MCLK clock enable_count remains 0** (clock not activated)

### Investigation Steps

#### 1. Device Tree Verification
All values are correct (Big Endian format):
```bash
$ cat /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-num | od -An -tu4 --endian=big
32  ✓

$ cat /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-width | od -An -tu4 --endian=big
64  ✓

$ cat /sys/firmware/devicetree/base/sai@ff310000/assigned-clock-rates | od -An -tu4 --endian=big
12288000  ✓ (12.288 MHz)
```

#### 2. Driver Binding Check
```bash
$ readlink /sys/devices/platform/ff310000.sai/driver
../../../bus/platform/drivers/rockchip-sai  ✓
```
Driver is correctly bound.

#### 3. Clock Status Investigation
```bash
$ cat /sys/kernel/debug/clk/clk_summary | grep mclk_sai1
mclk_sai1   0  0  0  12288000  0  0  50000  N  ff310000.sai  mclk
            ^  ^
            |  |
            |  +-- prepare_count = 0 (not prepared)
            +-- enable_count = 0 (NOT ENABLED) ✗
```

The MCLK is configured to 12.288 MHz but **NOT ENABLED**!

#### 4. Runtime PM Status (THE ROOT CAUSE)
```bash
$ cat /sys/devices/platform/ff310000.sai/power/runtime_status
suspended  ✗

$ cat /sys/devices/platform/ff310000.sai/power/control
auto
```

**The SAI device is in Runtime PM suspended state!**

### Why This Causes the Problem

The `rockchip_sai` driver uses Runtime PM to manage clocks:

```c
// From kernel-6.1/sound/soc/rockchip/rockchip_sai.c

static int rockchip_sai_runtime_resume(struct device *dev)
{
    struct rk_sai_dev *sai = dev_get_drvdata(dev);
    
    clk_prepare_enable(sai->hclk);  // Enable bus clock
    clk_prepare_enable(sai->mclk);  // Enable master clock ← CRITICAL
    
    regcache_sync(sai->regmap);
    return 0;
}

static int rockchip_sai_runtime_suspend(struct device *dev)
{
    struct rk_sai_dev *sai = dev_get_drvdata(dev);
    
    clk_disable_unprepare(sai->mclk);  // Disable master clock
    clk_disable_unprepare(sai->hclk);  // Disable bus clock
    return 0;
}
```

**When the device is suspended:**
1. `rockchip_sai_runtime_suspend()` is called
2. Both `mclk` and `hclk` are disabled
3. Hardware registers become inaccessible
4. Any audio playback attempt hangs waiting for hardware

**The ASoC framework should automatically call `pm_runtime_get_sync()` before audio operations, but this doesn't happen consistently.**

## The Solution

### Immediate Fix
Force the device to stay active:

```bash
echo on > /sys/devices/platform/ff310000.sai/power/control
```

This:
1. Changes power control from `auto` to `on`
2. Triggers `rockchip_sai_runtime_resume()`
3. Enables both MCLK and HCLK
4. Device stays active permanently

### Verification
After applying the fix:

```bash
$ cat /sys/devices/platform/ff310000.sai/power/runtime_status
active  ✓

$ cat /sys/kernel/debug/clk/mclk_sai1/clk_enable_count
1  ✓ (Clock is now enabled!)

$ speaker-test -D hw:0,0 -t sine -f 1000 -c 2 -l 3
speaker-test 1.2.9
Playback device is hw:0,0
...
Time per period = 2.760080
 0 - Front Left
 1 - Front Right
...
✓ WORKS!
```

## Permanent Solutions

### Option 1: Automated Script (Recommended)
Run after each boot:
```bash
./audio-config/scripts/fix-runtime-pm.sh
```

### Option 2: systemd Service
Create `/etc/systemd/system/sai-pm-fix.service`:
```ini
[Unit]
Description=Fix SAI1 Runtime PM
After=sound.target

[Service]
Type=oneshot
ExecStart=/bin/sh -c 'echo on > /sys/devices/platform/ff310000.sai/power/control'
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```

Enable:
```bash
sudo systemctl enable sai-pm-fix.service
sudo systemctl start sai-pm-fix.service
```

### Option 3: udev Rule
Create `/etc/udev/rules.d/99-sai-pm.rules`:
```
SUBSYSTEM=="platform", KERNEL=="ff310000.sai", ATTR{power/control}="on"
```

Reload udev:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### Option 4: Device Tree Fix (Most Proper)
Add to SAI node in device tree:
```dts
&sai1 {
    ...
    power-domains = <&power RK3506_PD_AUDIO>;
    pm-ignore-notify;  /* Don't use runtime PM */
    ...
};
```

However, this may require kernel driver modifications.

## Testing

Complete test sequence:

```bash
# 1. Check current status
cat /sys/devices/platform/ff310000.sai/power/runtime_status
cat /sys/kernel/debug/clk/mclk_sai1/clk_enable_count

# 2. Apply fix
echo on | sudo tee /sys/devices/platform/ff310000.sai/power/control

# 3. Verify
cat /sys/devices/platform/ff310000.sai/power/runtime_status  # Should be "active"
cat /sys/kernel/debug/clk/mclk_sai1/clk_enable_count          # Should be "1"

# 4. Test audio
speaker-test -D hw:0,0 -t sine -f 1000 -c 2 -l 3

# 5. Test with aplay
aplay -D hw:0,0 /path/to/test.wav
```

## Related Issues

### Issue: Endianness in verify-audio-remote.sh
Device Tree properties are stored in Big Endian format. The verify script was initially reading them as Little Endian, causing false warnings:

```bash
# Wrong:
$ cat .../dai-tdm-slot-num | od -An -tu4
536870912  ✗ (0x20000000 in little endian)

# Correct:
$ cat .../dai-tdm-slot-num | od -An -tu4 --endian=big
32  ✓ (0x00000020 in big endian)
```

**Fixed** by adding `--endian=big` to all `od` commands in verify-audio-remote.sh.

## Summary

| Component | Status | Notes |
|-----------|--------|-------|
| Device Tree | ✓ Correct | All 6 properties present with correct Big Endian values |
| Driver Binding | ✓ Correct | rockchip-sai driver bound to ff310000.sai |
| Clock Configuration | ✓ Correct | MCLK set to 12.288 MHz |
| **Runtime PM** | **✗ Suspended** | **Root cause - clocks not enabled** |
| **Solution** | **✓ Force "on"** | **Set power/control to "on"** |

## Lessons Learned

1. **Device tree correctness doesn't guarantee functionality** - Runtime PM issues can prevent hardware access even with perfect DT configuration.

2. **Clock configuration ≠ Clock activation** - `assigned-clock-rates` sets the rate but doesn't enable the clock. The driver must call `clk_prepare_enable()`.

3. **Endianness matters** - Device Tree properties are Big Endian. Always use `od --endian=big` when reading raw DT values.

4. **Runtime PM is implicit** - Not all drivers handle Runtime PM correctly with ASoC. Sometimes manual intervention is needed.

5. **Debugging workflow**: Device tree → Driver binding → Clock tree → Runtime PM → Hardware registers

## References

- Kernel driver: `kernel-6.1/sound/soc/rockchip/rockchip_sai.c`
- Device tree: `device/rockchip/rk3506/rk3506b-luckfox-lyra-zero-w-sd.dts`
- Clock tree: `/sys/kernel/debug/clk/clk_summary`
- Runtime PM: `/sys/devices/platform/ff310000.sai/power/`

---
**Status**: ✓ **RESOLVED** - Audio works after Runtime PM fix
**Date**: 2026-02-13
