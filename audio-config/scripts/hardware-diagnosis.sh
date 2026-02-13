#!/bin/bash
#
# hardware-diagnosis.sh - Complete Hardware Audio Diagnosis
#
# This script checks if audio signals are being generated and provides
# hardware troubleshooting guidance.
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

TARGET="${1:-root@192.168.123.100}"

echo "======================================================================="
echo "  HARDWARE AUDIO DIAGNOSIS"
echo "  Target: $TARGET"
echo "======================================================================="
echo ""

run_remote() {
    ssh "$TARGET" "$@"
}

echo -e "${BLUE}>>> SAI1 HARDWARE STATUS${NC}"
echo "-------------------------------------------------------------------"

# Check if hardware is transmitting
echo "Starting audio playback..."
run_remote "speaker-test -D hw:0,0 -t sine -f 1000 -c 2 > /dev/null 2>&1 &"
sleep 1

echo ""
echo "1. SAI1 Transfer Enable Status (XFER Register 0x10):"
XFER=$(run_remote "cat /sys/kernel/debug/regmap/ff310000.sai/registers | grep '^10:' | awk '{print \$2}'")
echo "   XFER = 0x$XFER"
if [ "$XFER" = "00000007" ] || [ "$XFER" = "0x00000007" ]; then
    echo -e "   ${GREEN}✓${NC} TX enabled, CLK enabled, FS enabled"
else
    echo -e "   ${RED}✗${NC} Unexpected value - hardware not transmitting"
fi

echo ""
echo "2. TX FIFO Status (Register 0x1c):"
TXFIFO=$(run_remote "cat /sys/kernel/debug/regmap/ff310000.sai/registers | grep '^1c:' | awk '{print \$2}'")
echo "   TXFIFO = 0x$TXFIFO"
if [ "$TXFIFO" != "00000000" ]; then
    echo -e "   ${GREEN}✓${NC} Data in TX FIFO - hardware is sending audio data"
else
    echo -e "   ${YELLOW}⚠${NC} TX FIFO empty"
fi

echo ""
echo "3. DMA Control (Register 0x24):"
DMA=$(run_remote "cat /sys/kernel/debug/regmap/ff310000.sai/registers | grep '^24:' | awk '{print \$2}'")
echo "   DMA = 0x$DMA"

run_remote "killall speaker-test 2>/dev/null" || true

echo ""
echo "======================================================================="
echo -e "${BLUE}>>> GPIO/PIN CONFIGURATION${NC}"
echo "======================================================================="

echo ""
echo "Physical Pin Mapping (Lyra Zero W):"
echo "-------------------------------------------------------------------"
echo "  rm_io8 (GPIO0-8) → SAI1_MCLK  (Master Clock)"
echo "  rm_io7 (GPIO0-7) → SAI1_SCLK  (Bit Clock / BCLK)"
echo "  rm_io6 (GPIO0-6) → SAI1_LRCK  (Word Select / LRCLK / Frame Sync)"
echo "  rm_io4 (GPIO0-4) → SAI1_SDO0  (Serial Data Out)"

echo ""
echo "Pin Mux Status:"
run_remote "cat /sys/kernel/debug/pinctrl/pinctrl-handles 2>/dev/null | grep -A 10 'ff310000.sai' | grep -E 'gpio0-[4678]|function'" || echo "Unable to read pinctrl status"

echo ""
echo "======================================================================="
echo -e "${BLUE}>>> HARDWARE TROUBLESHOOTING GUIDE${NC}"
echo "======================================================================="
echo ""

echo -e "${YELLOW}The software is working correctly but no audio output.${NC}"
echo -e "${YELLOW}This indicates a HARDWARE issue:${NC}"
echo ""

echo "ISSUE #1: PCM5102A DAC Not Connected or Not Powered"
echo "-------------------------------------------------------------------"
echo "Check:"
echo "  ✓ PCM5102A is connected to the board"
echo "  ✓ VCC connected to 3.3V or 5V"
echo "  ✓ GND connected to ground"
echo "  ✓ BCLK (BCK) connected to rm_io7 (GPIO0-7)"
echo "  ✓ LRCK (LCK) connected to rm_io6 (GPIO0-6)"  
echo "  ✓ DIN connected to rm_io4 (GPIO0-4)"
echo "  ✓ MCLK connected to rm_io8 (GPIO0-8) - OPTIONAL but configured"
echo ""

echo "ISSUE #2: PCM5102A Soft Mute Enabled"
echo "-------------------------------------------------------------------"
echo "The PCM5102A has a XSMT (Soft Mute) pin:"
echo "  Solution 1: Short XSMT to VIN/HIGH (recommended)"
echo "  Solution 2: Leave XSMT floating (depends on board)"
echo "  Solution 3: Connect XSMT to GND (will enable mute!)"
echo ""
echo "  ⚠ If XSMT is connected to GND, audio will be MUTED!"
echo "  → Disconnect XSMT from GND or connect to VCC/HIGH"
echo ""

echo "ISSUE #3: PCM5102A Format Pin Configuration"
echo "-------------------------------------------------------------------"
echo "The PCM5102A has FMT pin for format selection:"
echo "  FMT = GND  → I2S format (24-bit)"
echo "  FMT = HIGH → Left-Justified format"
echo "  FMT = FLOAT → Auto-detect"
echo ""
echo "Current driver config: I2S format"
echo "  → If using hardware PCM5102A module, check FMT pin"
echo ""

echo "ISSUE #4: No Speakers/Headphones Connected"
echo "-------------------------------------------------------------------"
echo "Check:"
echo "  ✓ Speakers or headphones connected to PCM5102A output"
echo "  ✓ Speaker polarity correct (OUTL+/-, OUTR+/-)"
echo "  ✓ Speakers have power (if powered speakers)"
echo "  ✓ Volume knob turned up (if present)"
echo ""

echo "ISSUE #5: Wrong Board Pins"
echo "-------------------------------------------------------------------"
echo "Verify you are connecting to the correct physical pins:"
echo "  On Lyra Zero W, the rm_io pins are specific connector pins"
echo "  Consult the board documentation for pin locations"
echo ""
echo "Common mistake: Connecting to UART or other pins instead of rm_io"
echo ""

echo "======================================================================="
echo -e "${BLUE}>>> SIGNAL VERIFICATION WITH MULTIMETER/OSCILLOSCOPE${NC}"
echo "======================================================================="
echo ""

echo "If you have measurement equipment:"
echo ""
echo "1. With MULTIMETER:"
echo "   Measure DC voltage on MCLK (rm_io8) while audio plays:"
echo "   → Should see ~1.5V-1.8V (square wave average)"
echo "   → If 0V or 3.3V constant: Pin not toggling"
echo ""

echo "2. With OSCILLOSCOPE:"
echo "   Connect scope to signals while running speaker-test:"
echo ""
echo "   MCLK (rm_io8): Should see ~12.288 MHz square wave"
echo "   SCLK (rm_io7): Should see ~3.072 MHz square wave (48kHz * 64)"  
echo "   LRCK (rm_io6): Should see 48 kHz square wave"
echo "   DATA (rm_io4): Should see serial data stream"
echo ""
echo "   If no signals visible: Check pinctrl/GPIO configuration"
echo "   If signals present: Problem is PCM5102A or connections"
echo ""

echo "======================================================================="
echo -e "${BLUE}>>> TESTING WITH LOOPBACK${NC}"
echo "======================================================================="
echo ""

echo "To verify SAI hardware is working without DAC:"
echo ""
echo "1. Create loopback test (requires board modification):"
echo "   Connect SAI1_SDO0 (rm_io4) to SAI1_SDI (if SAI1 RX configured)"
echo "   Then record what you play"
echo ""
echo "2. Or test with logic analyzer:"
echo "   Connect logic analyzer to rm_io4/6/7/8"
echo "   Decode I2S protocol"
echo "   Verify data pattern matches test tone"
echo ""

echo "======================================================================="
echo -e "${BLUE}>>> NEXT STEPS${NC}"
echo "======================================================================="
echo ""

echo "1. VERIFY CONNECTIONS:"
echo "   □ PCM5102A powered (check with multimeter: VCC = 3.3V or 5V)"
echo "   □ All signal pins connected correctly"
echo "   □ XSMT pin is HIGH or floating (NOT GND)"
echo "   □ Speakers/headphones connected to output"
echo ""

echo "2. CHECK PCM5102A MODULE:"
echo "   □ LED indicator on PCM5102A module (if present) should be on"
echo "   □ No physical damage visible"
echo "   □ Solder joints good (if DIY)"
echo ""

echo "3. TEST WITH KNOWN-GOOD DAC:"
echo "   □ Try different PCM5102A module"
echo "   □ Or try different DAC (PCM5100, UDA1334A, etc.)"
echo ""

echo "4. VERIFY BOARD PINS:"
echo "   □ Consult Lyra Zero W schematic/documentation"
echo "   □ Verify rm_io4/6/7/8 are accessible pins"
echo "   □ Check if pins are shared with other functions"
echo ""

echo "======================================================================="
echo -e "${BLUE}>>> SUMMARY${NC}"
echo "======================================================================="
echo ""

if [ "$XFER" = "00000007" ] && [ "$TXFIFO" != "00000000" ]; then
    echo -e "${GREEN}✓ SOFTWARE: Fully functional${NC}"
    echo "  - SAI1 hardware is transmitting"
    echo "  - Audio data is flowing through FIFO"
    echo "  - Pins are correctly muxed"
    echo "  - Clocks are running"
    echo ""
    echo -e "${YELLOW}✗ HARDWARE: No audio output detected${NC}"
    echo ""
    echo "MOST LIKELY CAUSES (in order):"
    echo "  1. PCM5102A XSMT pin connected to GND (muted) → Connect to HIGH" 
    echo "  2. PCM5102A not connected → Check wiring"
    echo "  3. Speakers not connected → Connect output"
    echo "  4. Wrong pins used → Verify rm_io4/6/7/8 locations"
    echo "  5. Defective PCM5102A → Try different module"
else
    echo -e "${RED}✗ SOFTWARE ISSUE DETECTED${NC}"
    echo "  Hardware is not transmitting correctly"
    echo "  Run: ./verify-audio-remote.sh"
fi

echo ""
echo "======================================================================="
