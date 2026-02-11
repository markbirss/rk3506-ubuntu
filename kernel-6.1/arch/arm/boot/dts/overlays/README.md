# PCM5102A Device Tree Overlays for Luckfox Lyra Zero W

Diese Device Tree Overlays ermöglichen die Verwendung des PCM5102A DAC auf dem Luckfox Lyra Zero W Board.

## Verfügbare Overlays

### 1. rk3506-pcm5102a-sai0.dts
Verwendet den SAI0 Interface mit folgenden Pins:
- **GPIO0_A0** (Pin 0): LRCK → PCM5102A LCK
- **GPIO0_A1** (Pin 1): SCLK → PCM5102A BCK
- **GPIO0_A2** (Pin 2): MCLK → PCM5102A SCK
- **GPIO0_A3** (Pin 3): SDO  → PCM5102A DIN

### 2. rk3506-pcm5102a-sai1.dts
Verwendet den SAI1 Interface mit folgenden Pins:
- **GPIO0_B0** (Pin 8):  MCLK → PCM5102A SCK
- **GPIO0_B1** (Pin 9):  SCLK → PCM5102A BCK
- **GPIO0_B2** (Pin 10): LRCK → PCM5102A LCK
- **GPIO0_B4** (Pin 12): SDO0 → PCM5102A DIN

## PCM5102A Verdrahtung

Zusätzlich zu den I2S-Signalen müssen folgende Pins des PCM5102A verbunden werden:

### Stromversorgung
- **VDD** → 3.3V
- **GND** → GND
- **33V** → 3.3V (falls vorhanden)

### Konfigurationspins (empfohlene Einstellungen)
- **FLT** → GND (Normal latency, oder VCC für low latency)
- **FMT** → GND (I2S format, oder VCC für left-justified)
- **DEMP** → GND (De-emphasis off)
- **XSMT** → 3.3V (Soft mute control, kann auch an GND für dauerhaftes Mute)

### Audio-Ausgang
- **OUTL** → Linker Kanal Audio-Ausgang
- **OUTR** → Rechter Kanal Audio-Ausgang
- **AGND** → Audio Ground

## Kompilierung

### Auf dem Entwicklungsrechner

```bash
cd /home/nadlech/privat/luckfox/rk3506-ubuntu/kernel-6.1/arch/arm/boot/dts/overlays
chmod +x build-overlays.sh
./build-overlays.sh
```

### Manuell kompilieren

Für SAI0:
```bash
dtc -@ -I dts -O dtb -o rk3506-pcm5102a-sai0.dtbo rk3506-pcm5102a-sai0.dts
```

Für SAI1:
```bash
dtc -@ -I dts -O dtb -o rk3506-pcm5102a-sai1.dtbo rk3506-pcm5102a-sai1.dts
```

## Installation auf dem Target

1. Kompiliere die Overlays (siehe oben)

2. Kopiere die `.dtbo` Dateien auf dein Board:
```bash
scp rk3506-pcm5102a-sai0.dtbo root@luckfox:/boot/overlays/
# oder für SAI1:
scp rk3506-pcm5102a-sai1.dtbo root@luckfox:/boot/overlays/
```

3. Overlay aktivieren:

**Methode 1: U-Boot (falls unterstützt)**
Editiere die Boot-Konfiguration (z.B. `/boot/uEnv.txt` oder `/boot/extlinux/extlinux.conf`):
```
overlays=rk3506-pcm5102a-sai0
```

**Methode 2: Direct Device Tree Append**
Falls das System keine Overlay-Unterstützung hat, kannst du das Overlay manuell zum Device Tree hinzufügen:
```bash
# Decompile das aktuelle DTB
dtc -I dtb -O dts -o current.dts /boot/dtb/*.dtb

# Merge mit Overlay (manuell oder mit fdtoverlay)
fdtoverlay -i /boot/dtb/*.dtb -o /boot/dtb/modified.dtb /boot/overlays/rk3506-pcm5102a-sai0.dtbo

# Backup erstellen und ersetzen
cp /boot/dtb/*.dtb /boot/dtb/*.dtb.backup
cp /boot/dtb/modified.dtb /boot/dtb/*.dtb
```

4. Reboot:
```bash
reboot
```

## Testing

Nach dem Reboot sollte die Sound-Karte erkannt werden:

```bash
# Checke ob die Sound-Karte verfügbar ist
aplay -l

# Sollte etwas wie zeigen:
# card 0: PCM5102ASAI0 [PCM5102A-SAI0], device 0: ...

# Liste ALSA Geräte
cat /proc/asound/cards

# Test Audio Playback
speaker-test -c2 -t wav

# Oder mit aplay
aplay -D hw:0,0 /usr/share/sounds/alsa/Front_Center.wav
```

## Troubleshooting

### Overlay wird nicht geladen
```bash
# Checke Device Tree
ls -l /sys/firmware/devicetree/base/pcm5102a*

# Checke Kernel Log
dmesg | grep -i pcm5102
dmesg | grep -i sai
dmesg | grep -i sound
```

### Kein Sound
```bash
# Checke ob SAI Device aktiv ist
cat /sys/kernel/debug/clk/clk_summary | grep sai

# Checke ALSA Mixer
amixer

# Setze Volume
amixer sset 'PCM' 100%

# Checke ob der Treiber geladen ist
lsmod | grep snd
```

### Pin Conflicts
Falls andere Overlays oder Konfigurationen die selben Pins verwenden, kann es zu Konflikten kommen. Überprüfe:
```bash
cat /sys/kernel/debug/pinctrl/*/pinmux-pins | grep -i gpio0
```

## Unterschiede zwischen SAI0 und SAI1

Beide Interfaces sind funktional identisch. Wähle basierend auf:
1. **Pin-Verfügbarkeit** auf deinem Board
2. **Andere Peripherie** die bereits SAI0 oder SAI1 nutzt
3. **PCB Layout** - welche Pins sind physisch einfacher zu erreichen

Start mit **SAI0**, da es oft als primäres Audio-Interface genutzt wird.

## Erweiterte Konfiguration

### MCLK Frequenz anpassen

Falls du eine andere MCLK Frequenz benötigst, editiere in der `.dts` Datei:
```dts
simple-audio-card,mclk-fs = <256>;  // 256x sample rate (Standard)
// oder
simple-audio-card,mclk-fs = <512>;  // 512x sample rate
```

### Format ändern

Standard ist I2S, aber du kannst auch andere Formate nutzen:
```dts
simple-audio-card,format = "i2s";        // Standard
// simple-audio-card,format = "left_j";  // Left Justified
// simple-audio-card,format = "right_j"; // Right Justified
```

Beachte: Du musst auch den FMT Pin am PCM5102A entsprechend setzen.

## Links und Referenzen

- [PCM5102A Datasheet](https://www.ti.com/product/PCM5102A)
- [Rockchip RK3506 Documentation](https://opensource.rock-chips.com/)
- [Linux Kernel Simple Audio Card](https://www.kernel.org/doc/Documentation/devicetree/bindings/sound/simple-card.txt)

## Support

Bei Problemen oder Fragen, bitte öffne ein Issue im Repository.

## Lizenz

SPDX-License-Identifier: (GPL-2.0+ OR MIT)
