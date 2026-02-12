# Quick Start Guide - PCM5102A Audio

Schnellanleitung zum Wiederaufbau der Audio-Konfiguration.

## Dateien Übersicht

```
audio-config/
├── README.md                    # Vollständige Dokumentation
├── QUICKSTART.md               # Diese Datei
├── asound.conf                 # ALSA Standard-Konfiguration
├── overlays/
│   ├── rk3506-pcm5102a-sai0.dts   # Pimoroni SHIM Overlay
│   └── rk3506-pcm5102a-sai1.dts   # Generic PCM5102A Overlay
└── scripts/
    ├── build-overlays.sh       # Overlay-Compiler
    ├── load-overlay.sh         # Overlay zur Laufzeit laden
    └── test-audio.sh           # Audio-Test Script
```

## Schnell-Rebuild (mit Base DT - empfohlen)

### 1. Kernel-Config anpassen

```bash
cd kernel-6.1

# PCM5102A als Built-in (nicht Modul)
sed -i 's/CONFIG_SND_SOC_PCM5102A=m/CONFIG_SND_SOC_PCM5102A=y/' arch/arm/configs/rk3506_luckfox_defconfig

# Overlay-Support aktivieren
grep -q CONFIG_OF_OVERLAY arch/arm/configs/rk3506_luckfox_defconfig || \
    echo 'CONFIG_OF_OVERLAY=y' >> arch/arm/configs/rk3506_luckfox_defconfig
```

### 2. Device Tree Symbol-Support

```bash
# Makefile editieren
nano arch/arm/boot/dts/Makefile

# Diese Zeile VOR der dtb-$(CONFIG_ROCKCHIP_RK3506) Zeile einfügen:
# DTC_FLAGS_rk3506b-luckfox-lyra-zero-w-sd := -@
```

### 3. Base DTS für SAI1 Audio

```bash
nano arch/arm/boot/dts/rk3506b-luckfox-lyra-zero-w-sd.dts
```

Nach dem bestehenden Inhalt (vor dem letzten `};`) einfügen:

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

### 4. Image bauen

```bash
cd /path/to/rk3506-ubuntu
./build.sh lunch  # Option 6 wählen für Lyra Zero W
./build.sh
```

### 5. Flashen

```bash
sudo ./rkflash.sh update
```

### 6. System-Konfiguration auf dem Board

```bash
# Per SSH verbinden
ssh lyra@192.168.123.100

# User zur Audio-Gruppe hinzufügen
sudo usermod -aG audio lyra

# ALSA-Konfiguration kopieren
# (asound.conf vom Host kopieren oder manuell erstellen)

# Ausloggen und neu einloggen damit Gruppe aktiv wird
exit
```

### 7. Test

```bash
ssh lyra@192.168.123.100

# Sound Card prüfen
cat /proc/asound/cards
# Erwartung: Card 0: PCM5102ASAI1

# Audio-Test
speaker-test -D hw:0,0 -t sine -f 1000 -c 2 -l 5
```

## Overlay-Methode (Optional - für Tests)

Wenn du Overlays zur Laufzeit testen möchtest:

### 1. Overlays kompilieren

```bash
cd audio-config/overlays
../scripts/build-overlays.sh
```

### 2. Auf Board kopieren

```bash
scp *.dtbo lyra@192.168.123.100:/boot/overlays/
scp ../scripts/load-overlay.sh lyra@192.168.123.100:/tmp/
```

### 3. Laden (auf dem Board)

```bash
ssh lyra@192.168.123.100
sudo /tmp/load-overlay.sh /boot/overlays/rk3506-pcm5102a-sai1.dtbo
```

**Hinweis:** Overlay-Methode erfordert dass Base-DT mit `-@` Flag kompiliert wurde (siehe oben).

## Pimoroni SHIM (SAI0)

Für Pimoroni Audio DAC SHIM:
- Verwende `rk3506-pcm5102a-sai0.dts` statt SAI1
- Ersetze `&sai1` durch `&sai0` im Base-DTS
- Beachte andere Pin-Belegung (siehe README.md)

## Pin-Verbindungen

### Generic PCM5102A (SAI1)

| PCM5102A | Lyra Zero W | GPIO |
|----------|-------------|------|
| VCC      | 3.3V        | -    |
| GND      | GND         | -    |
| BCK      | rm_io7      | GPIO0_A7 |
| DIN      | rm_io4      | GPIO0_A4 |
| LCK      | rm_io6      | GPIO0_A6 |
| SCK      | GND         | -    |
| XMT      | 3.3V        | -    |

### Pimoroni SHIM (SAI0)

| SHIM Pin | Lyra Zero W | GPIO |
|----------|-------------|------|
| VCC      | 3.3V        | -    |
| GND      | GND         | -    |
| BCK      | rm_io14     | GPIO0_B6 |
| DIN      | rm_io18     | GPIO0_C2 |
| LRCK     | rm_io16     | GPIO0_C0 |
| EN       | rm_io11     | GPIO0_B3 |

## Troubleshooting

```bash
# Kein Sound? Prüfe:

# 1. Driver geladen?
cat /sys/bus/platform/drivers/snd_soc_pcm5102a/module/refcnt
# Sollte > 0 sein

# 2. User in audio Gruppe?
groups
# Sollte "audio" enthalten

# 3. Device vorhanden?
ls -l /dev/snd/pcmC0D0p

# 4. Hardware-Fähigkeiten
aplay -D hw:0,0 --dump-hw-params < /dev/zero 2>&1 | head -20

# 5. Kernel-Log
dmesg | grep -i "pcm5102\|sai1\|audio"
```

## Weitere Infos

Siehe [README.md](README.md) für:
- Detaillierte Pin-Beschreibungen
- Hardware-Anschlussdiagramme
- Fortgeschrittene Konfiguration
- Referenz-Links
- Troubleshooting-Guide

---

**Status (2025-02-12):**
- ✅ SAI1 (Generic PCM5102A): Getestet, funktioniert
- ⏳ SAI0 (Pimoroni SHIM): Konfiguration bereit, noch nicht getestet
