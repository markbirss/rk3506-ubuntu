# PCM5102A Audio Overlay für Luckfox Lyra Zero W

Device Tree Overlays für den PCM5102A DAC auf dem RK3506 SoC.

## Quick Start

### 1. Build & Installation ins Image

```bash
# Nach dem normalen Image-Build
./build.sh lunch
./build.sh

# Overlays ins Image installieren
cd kernel-6.1/arch/arm/boot/dts/overlays
sudo ./install-overlays.sh

# Flash das update.img auf dein Board
```

**Optionen:**

- `./install-overlays.sh` - Mit .dts Source-Files (empfohlen für Testing)
- `./install-overlays.sh --no-sources` - Nur .dtbo Files

### 2. Auf dem Board anwenden

```bash
# Nach dem Flash sind die Overlays in /boot/overlays/ verfügbar

# Overlay anwenden (Beispiel für SAI0)
cd /boot
sudo fdtoverlay -i dtb.img -o dtb-pcm5102a.img /boot/overlays/rk3506-pcm5102a-sai0.dtbo

# Backup und aktivieren
sudo mv dtb.img dtb-original.img
sudo ln -s dtb-pcm5102a.img dtb.img

# Reboot
sudo reboot

# Nach Reboot testen
aplay -l
speaker-test -c2 -t wav
```

## Hardware-Verdrahtung

### SAI0 (empfohlen)

| Luckfox Pin | PCM5102A Pin | Funktion |
|-------------|--------------|----------|
| GPIO0_A0    | LCK          | Left/Right Clock |
| GPIO0_A1    | BCK          | Bit Clock |
| GPIO0_A2    | SCK          | Master Clock |
| GPIO0_A3    | DIN          | Data Input |
| 3.3V        | VDD          | Power |
| GND         | GND          | Ground |

### SAI1 (Alternative)

| Luckfox Pin | PCM5102A Pin | Funktion |
|-------------|--------------|----------|
| GPIO0_B0    | SCK          | Master Clock |
| GPIO0_B1    | BCK          | Bit Clock |
| GPIO0_B2    | LCK          | Left/Right Clock |
| GPIO0_B4    | DIN          | Data Input |
| 3.3V        | VDD          | Power |
| GND         | GND          | Ground |

### PCM5102A Konfiguration

- **FLT** → GND (Normal latency)
- **FMT** → GND (I2S format)
- **DEMP** → GND (De-emphasis off)
- **XSMT** → 3.3V (Soft mute control)

## Troubleshooting

### Overlay wird nicht geladen

```bash
# Prüfe Device Tree
ls -l /sys/firmware/devicetree/base/ | grep pcm5102

# Kernel Messages
dmesg | grep -i "pcm5102\|sai\|audio"
```

### Kein Sound

```bash
# Checke ALSA
aplay -l

# Checke Mixer
amixer

# Test mit Debug
aplay -v test.wav
```

### Pins prüfen

```bash
# Prüfe Pin-Konfiguration
cat /sys/kernel/debug/pinctrl/pinctrl-rockchip/pinmux-pins | grep gpio0
```

## Dateien

- **build-overlays.sh** - Kompiliert die Overlays
- **install-overlays.sh** - Installiert ins rootfs Image
- **rk3506-pcm5102a-sai0.dts/.dtbo** - SAI0 Overlay
- **rk3506-pcm5102a-sai1.dts/.dtbo** - SAI1 Overlay

## Erweiterte Konfiguration

### MCLK Frequenz ändern

In der `.dts` Datei editieren (auf dem Board falls installiert):

```dts
simple-audio-card,mclk-fs = <256>;  // 256x sample rate (Standard)
// oder
simple-audio-card,mclk-fs = <512>;  // 512x sample rate
```

Dann neu kompilieren:

```bash
cd /boot/overlays
dtc -@ -I dts -O dtb -o rk3506-pcm5102a-sai0.dtbo rk3506-pcm5102a-sai0.dts
```

### Format ändern

```dts
simple-audio-card,format = "i2s";        // Standard
// simple-audio-card,format = "left_j";  // Left Justified
```

**Wichtig:** PCM5102A FMT Pin entsprechend setzen (GND=I2S, VCC=Left-Justified)

## Support

Bei Problemen prüfe:

1. Hardware-Verbindungen (besonders 3.3V, nicht 5V!)
2. PCM5102A Config-Pins korrekt gesetzt
3. Kernel Messages mit `dmesg`
4. ALSA mit `aplay -l` und `amixer`

## Lizenz

SPDX-License-Identifier: MIT
