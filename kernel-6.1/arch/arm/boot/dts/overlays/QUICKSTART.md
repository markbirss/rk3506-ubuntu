# PCM5102A Quick Start Guide

## Schnellstart für Luckfox Lyra Zero W + PCM5102A

### 1. Hardware-Verbindung (SAI0 - empfohlen)

Verbinde deinen PCM5102A wie folgt:

| Luckfox Pin | PCM5102A Pin | Beschreibung |
|-------------|--------------|--------------|
| GPIO0_A2    | SCK          | Master Clock |
| GPIO0_A0    | LCK          | Left/Right Clock |
| GPIO0_A1    | BCK          | Bit Clock |
| GPIO0_A3    | DIN          | Data Input |
| 3.3V        | VIN/VDD      | Stromversorgung |
| GND         | GND          | Ground |

**PCM5102A Konfigurationspins:**
- FLT → GND
- FMT → GND  
- DEMP → GND
- XSMT → 3.3V (oder GND für dauerhaftes Mute)

### 2. Overlay auf das Board kopieren

```bash
# Von deinem Entwicklungsrechner:
scp rk3506-pcm5102a-sai0.dtbo root@dein-luckfox:/boot/overlays/
```

### 3. Overlay aktivieren

**Option A: Via fdtoverlay (empfohlen für Test)**
```bash
# Auf dem Luckfox Board:
cd /boot

# Backup erstellen
cp dtb.img dtb.img.backup

# Overlay anwenden
fdtoverlay -i dtb.img -o dtb-pcm5102a.img /boot/overlays/rk3506-pcm5102a-sai0.dtbo

# Teste mit dem neuen DTB
# (editiere bootargs oder nutze symlink)
mv dtb.img dtb-original.img
ln -s dtb-pcm5102a.img dtb.img
```

**Option B: Boot-Konfiguration editieren**

Falls dein System Overlays in der Boot-Config unterstützt (check `/boot/uEnv.txt` oder `/boot/extlinux/extlinux.conf`):

```bash
# In /boot/uEnv.txt oder ähnlich:
overlays=rk3506-pcm5102a-sai0
```

### 4. Reboot

```bash
reboot
```

### 5. Testen

```bash
# Checke ob Sound-Karte erkannt wurde
aplay -l

# Sollte zeigen:
# card 0: PCM5102ASAI0 [PCM5102A-SAI0], device 0: ...

# Test mit Speaker-Test
speaker-test -c2 -t wav -D hw:0,0

# Oder spiele eine WAV Datei
aplay -D hw:0,0 test.wav
```

## Troubleshooting

### Overlay wird nicht geladen
```bash
# Checke ob Overlay im Device Tree ist
ls -la /sys/firmware/devicetree/base/ | grep pcm5102

# Kernel Messages
dmesg | grep -i "pcm5102\|sai\|audio"
```

### Kein Ton
```bash
# Prüfe ALSA
amixer

# Prüfe ob SAI0 aktiv ist
cat /sys/kernel/debug/clk/clk_summary | grep -i sai

# Test mit mehr Debug-Info
aplay -v -D hw:0,0 test.wav
```

### Pin-Konflikte
```bash
# Prüfe ob die Pins korrekt konfiguriert sind
cat /sys/kernel/debug/pinctrl/pinctrl-rockchip/pinmux-pins | grep -A2 gpio0
```

## Alternative: SAI1 verwenden

Falls SAI0 nicht funktioniert oder die Pins bereits belegt sind, verwende SAI1:

1. Kopiere `rk3506-pcm5102a-sai1.dtbo` statt sai0
2. Verbinde zu SAI1 Pins (siehe README.md)
3. Folge den gleichen Schritten wie oben

## Weiterführende Hilfe

Siehe [README.md](README.md) für:
- Detaillierte Pin-Belegung
- Erweiterte Konfiguration
- Troubleshooting-Details
- Alternative MCLK-Frequenzen
- Format-Optionen

## Wichtige Hinweise

⚠️ **Backup:** Erstelle immer ein Backup deines Device Trees bevor du Overlays anwendest!

⚠️ **Spannung:** Der PCM5102A benötigt 3.3V - **nicht** 5V verwenden!

⚠️ **Audio-Ausgang:** Die Ausgänge (OUTL/OUTR) liefern Line-Level - für Lautsprecher wird ein Verstärker benötigt.

## Bekannte Probleme

1. **Kein MCLK:** Manche PCM5102A Module funktionieren auch ohne MCLK (SCK kann NC bleiben)
2. **Pop beim Start:** Normal für PCM5102A, kann mit XSMT Pin-Steuerung reduziert werden
3. **First Boot:** Manchmal muss das Board 2x rebooten bis Audio funktioniert

Viel Erfolg! 🎵
