#!/bin/bash
# Remote Audio Configuration Verification Script
# Connects to board via SSH and runs comprehensive audio diagnostics

SSH_TARGET="${1:-lyra@192.168.123.100}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "======================================================================="
echo "  PCM5102A Audio Configuration Verification"
echo "  Target: $SSH_TARGET"
echo "======================================================================="
echo ""

# Check if we can reach the board
echo "Testing SSH connection..."
if ! ssh -o ConnectTimeout=5 -o BatchMode=yes "$SSH_TARGET" "echo 'Connected'" >/dev/null 2>&1; then
    echo "ERROR: Cannot connect to $SSH_TARGET"
    echo ""
    echo "Usage: $0 [user@host]"
    echo "Example: $0 lyra@192.168.123.100"
    echo ""
    echo "Make sure:"
    echo "  - Board is powered on and connected"
    echo "  - SSH keys are configured (run: ssh-copy-id $SSH_TARGET)"
    echo ""
    exit 1
fi
echo "✓ Connection successful"
echo ""

# Run verification on remote board
echo "Running diagnostics on remote board..."
echo "-----------------------------------------------------------------------"
ssh "$SSH_TARGET" 'bash -s' <<'REMOTE_SCRIPT'
#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

errors=0

# Function to check property
check_prop() {
    local prop=$1
    local path="/sys/firmware/devicetree/base/sai@ff310000/$prop"
    if [ -f "$path" ]; then
        echo -e "${GREEN}✓${NC} $prop: EXISTS"
        return 0
    else
        echo -e "${RED}✗${NC} $prop: MISSING"
        return 1
    fi
}

# 1. Check Sound Card Registration
echo "1. Sound Card Status"
echo "-------------------------------------------------------------------"
if grep -q "PCM5102ASAI1" /proc/asound/cards 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Sound card registered: PCM5102ASAI1"
    cat /proc/asound/cards | grep -A1 PCM5102ASAI1
else
    echo -e "${RED}✗${NC} Sound card NOT found!"
    echo "Available cards:"
    cat /proc/asound/cards
    ((errors++))
fi
echo ""

# 2. Check Device Nodes
echo "2. ALSA Device Nodes"
echo "-------------------------------------------------------------------"
if [ -c /dev/snd/pcmC0D0p ]; then
    echo -e "${GREEN}✓${NC} Playback device exists: /dev/snd/pcmC0D0p"
    ls -l /dev/snd/pcmC0D0p
else
    echo -e "${RED}✗${NC} Playback device NOT found!"
    echo "Available devices:"
    ls -l /dev/snd/
    ((errors++))
fi
echo ""

# 3. Check SAI1 Properties
echo "3. SAI1 Device Tree Properties"
echo "-------------------------------------------------------------------"
check_prop "#sound-dai-cells" || ((errors++))
check_prop "assigned-clocks" || ((errors++))
check_prop "assigned-clock-rates" || ((errors++))
check_prop "dai-tdm-slot-num" || ((errors++))
check_prop "dai-tdm-slot-width" || ((errors++))
check_prop "rockchip,clk-trcm" || ((errors++))

# Check that sound-dai does NOT exist (should be in simple-card, not SAI)
if [ -f /sys/firmware/devicetree/base/sai@ff310000/sound-dai ]; then
    echo -e "${RED}✗${NC} sound-dai: SHOULD NOT EXIST in SAI1 node!"
    ((errors++))
else
    echo -e "${GREEN}✓${NC} sound-dai: Correctly absent from SAI1 node"
fi
echo ""

# 4. Check Pin Configuration
echo "4. Pin Configuration (pinctrl-0)"
echo "-------------------------------------------------------------------"
if [ -f /sys/firmware/devicetree/base/sai@ff310000/pinctrl-0 ]; then
    byte_count=$(cat /sys/firmware/devicetree/base/sai@ff310000/pinctrl-0 | wc -c)
    pin_count=$((byte_count / 4))
    if [ $pin_count -eq 4 ]; then
        echo -e "${GREEN}✓${NC} pinctrl-0 has $pin_count pins (correct)"
        echo "   Expected: MCLK (rm_io8), SCLK (rm_io7), LRCK (rm_io6), SDO0 (rm_io4)"
    else
        echo -e "${RED}✗${NC} pinctrl-0 has $pin_count pins (expected: 4)"
        echo "   Missing: MCLK pin (rm_io8_sai1_mclk)"
        ((errors++))
    fi
    echo -n "   Raw phandles: "
    cat /sys/firmware/devicetree/base/sai@ff310000/pinctrl-0 | od -An -tx4
else
    echo -e "${RED}✗${NC} pinctrl-0 NOT found!"
    ((errors++))
fi
echo ""

# 5. Check TDM Values (Device Tree uses Big Endian!)
echo "5. TDM Configuration Values"
echo "-------------------------------------------------------------------"
if [ -f /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-num ]; then
    tdm_slots=$(cat /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-num | od -An -tu4 --endian=big | xargs)
    if [ "$tdm_slots" = "32" ]; then
        echo -e "${GREEN}✓${NC} dai-tdm-slot-num = 32"
    else
        echo -e "${YELLOW}⚠${NC} dai-tdm-slot-num = $tdm_slots (expected: 32)"
        ((errors++))
    fi
fi

if [ -f /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-width ]; then
    tdm_width=$(cat /sys/firmware/devicetree/base/sai@ff310000/dai-tdm-slot-width | od -An -tu4 --endian=big | xargs)
    if [ "$tdm_width" = "64" ]; then
        echo -e "${GREEN}✓${NC} dai-tdm-slot-width = 64"
    else
        echo -e "${YELLOW}⚠${NC} dai-tdm-slot-width = $tdm_width (expected: 64)"
        ((errors++))
    fi
fi

if [ -f /sys/firmware/devicetree/base/sai@ff310000/assigned-clock-rates ]; then
    mclk_rate=$(cat /sys/firmware/devicetree/base/sai@ff310000/assigned-clock-rates | od -An -tu4 --endian=big | xargs)
    if [ "$mclk_rate" = "12288000" ]; then
        echo -e "${GREEN}✓${NC} assigned-clock-rates = 12288000 (12.288 MHz)"
    else
        echo -e "${YELLOW}⚠${NC} assigned-clock-rates = $mclk_rate (expected: 12288000)"
        ((errors++))
    fi
fi
echo ""

# 6. Check User Permissions
echo "6. User Permissions"
echo "-------------------------------------------------------------------"
if groups | grep -q "audio"; then
    echo -e "${GREEN}✓${NC} User is in audio group"
    echo "   Groups: $(groups)"
else
    echo -e "${YELLOW}⚠${NC} User NOT in audio group"
    echo "   Current groups: $(groups)"
    echo "   Run setup: ./setup-audio-system.sh"
    ((errors++))
fi
echo ""

# 7. Check ALSA Configuration
echo "7. ALSA Configuration"
echo "-------------------------------------------------------------------"
if [ -f /etc/asound.conf ]; then
    echo -e "${GREEN}✓${NC} /etc/asound.conf exists"
    echo "   Content:"
    cat /etc/asound.conf | sed 's/^/   /'
else
    echo -e "${YELLOW}⚠${NC} /etc/asound.conf not found (optional)"
    echo "   Run setup: ./setup-audio-system.sh"
fi
echo ""

# 8. Runtime PM Status (CRITICAL!)
echo "8. Runtime PM Status"
echo "-------------------------------------------------------------------"
if [ -f /sys/devices/platform/ff310000.sai/power/runtime_status ]; then
    pm_status=$(cat /sys/devices/platform/ff310000.sai/power/runtime_status)
    pm_control=$(cat /sys/devices/platform/ff310000.sai/power/control)
    
    if [ "$pm_status" = "active" ]; then
        echo -e "${GREEN}✓${NC} Runtime PM status: $pm_status"
    else
        echo -e "${RED}✗${NC} Runtime PM status: $pm_status (should be 'active')"
        echo "   This is likely why audio doesn't work!"
        ((errors++))
    fi
    
    echo "   Power control: $pm_control"
    
    if [ "$pm_control" = "on" ]; then
        echo -e "${GREEN}✓${NC} Power control set to 'on' (forced active)"
    else
        echo -e "${YELLOW}⚠${NC} Power control is 'auto' - device may suspend"
        echo "   To fix: echo on | sudo tee /sys/devices/platform/ff310000.sai/power/control"
        echo "   Or run: ./fix-runtime-pm.sh"
    fi
    
    # Check clock enable status
    if [ -f /sys/kernel/debug/clk/mclk_sai1/clk_enable_count ]; then
        clk_enabled=$(cat /sys/kernel/debug/clk/mclk_sai1/clk_enable_count)
        if [ "$clk_enabled" -gt 0 ]; then
            echo -e "${GREEN}✓${NC} MCLK clock is enabled (count: $clk_enabled)"
        else
            echo -e "${RED}✗${NC} MCLK clock is NOT enabled (count: 0)"
            echo "   Clock is configured but not running!"
            echo "   This means Runtime PM has suspended the device."
            ((errors++))
        fi
    fi
else
    echo -e "${YELLOW}⚠${NC} Cannot check Runtime PM status (file not found)"
fi
echo ""

# 9. Test Audio Device Access
echo "9. Audio Device Access Test"
echo "-------------------------------------------------------------------"
echo "Testing if audio device can be opened (2 second timeout)..."
if timeout 2 aplay -D hw:0,0 --dump-hw-params < /dev/null 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Audio device opens successfully"
else
    exit_code=$?
    if [ $exit_code -eq 124 ]; then
        echo -e "${RED}✗${NC} Audio device access HANGS (hardware not initialized)"
        echo "   This means device tree properties are missing/incorrect!"
        echo "   Rebuild with correct device tree configuration."
        ((errors++))
    elif [ $exit_code -eq 1 ]; then
        echo -e "${YELLOW}⚠${NC} Audio device test failed (likely permission issue)"
        echo "   Make sure user is in audio group and logged out/in."
    else
        echo -e "${YELLOW}⚠${NC} Audio device test failed (exit code: $exit_code)"
    fi
fi
echo ""

# 10. Quick Speaker Test
echo "10. Quick Audio Playback Test (6 seconds)"
echo "-------------------------------------------------------------------"
echo "Running speaker-test with 6 second timeout..."
if timeout 6 speaker-test -D hw:0,0 -t sine -f 1000 -c 2 -l 1 >/dev/null 2>&1; then
    echo -e "${GREEN}✓${NC} speaker-test completed successfully!"
    echo "   If hardware is connected, you should have heard a tone."
    echo ""
    echo "   To hear audio, run:"
    echo "   ssh $(whoami)@$(hostname -I | awk '{print $1}') 'speaker-test -D hw:0,0 -t sine -f 1000 -c 2'"
else
    exit_code=$?
    if [ $exit_code -eq 124 ]; then
        echo -e "${RED}✗${NC} speaker-test HUNG - hardware initialization failed"
        echo "   Check Runtime PM status (Section 8 above)"
        echo "   Run: ./fix-runtime-pm.sh"
        echo "   See RUNTIME-PM-FIX.md for details."
        ((errors++))
    else
        echo -e "${YELLOW}⚠${NC} speaker-test failed with exit code: $exit_code"
        echo "   This might be a permission issue if not in audio group."
    fi
fi
echo ""

# Summary
echo "======================================================================="
echo "  VERIFICATION SUMMARY"
echo "======================================================================="
if [ $errors -eq 0 ]; then
    echo -e "${GREEN}✓ ALL CHECKS PASSED${NC}"
    echo ""
    echo "Audio system is properly configured!"
    echo ""
    echo "To test audio output:"
    echo "  speaker-test -D hw:0,0 -t sine -f 1000 -c 2"
    echo ""
    echo "To play audio files:"
    echo "  aplay -D hw:0,0 yourfile.wav"
    echo ""
    exit 0
else
    echo -e "${RED}✗ FOUND $errors ISSUE(S)${NC}"
    echo ""
    if [ $errors -ge 5 ]; then
        echo "Multiple critical device tree properties missing!"
        echo "Device tree needs to be rebuilt with correct configuration."
        echo ""
        echo "See documentation:"
        echo "  - audio-config/DIAGNOSIS.md (detailed analysis)"
        echo "  - audio-config/QUICKSTART.md (rebuild instructions)"
    else
        echo "Minor configuration issues found."
        echo "Run setup script: ./setup-audio-system.sh"
    fi
    echo ""
    exit 1
fi
REMOTE_SCRIPT

# Capture exit code from remote script
EXIT_CODE=$?

echo ""
echo "======================================================================="
echo "  VERIFICATION COMPLETE"
echo "======================================================================="
echo ""

if [ $EXIT_CODE -eq 0 ]; then
    echo "✓ Board $SSH_TARGET is configured correctly!"
    echo ""
    echo "Test audio with:"
    echo "  ssh $SSH_TARGET 'speaker-test -D hw:0,0 -t sine -f 1000 -c 2'"
else
    echo "✗ Issues found on $SSH_TARGET"
    echo ""
    echo "If device tree properties are missing:"
    echo "  1. Check: audio-config/DIAGNOSIS.md"
    echo "  2. Rebuild image with correct DTS"
    echo "  3. Flash and reboot"
    echo ""
    echo "If only permission issues:"
    echo "  ./setup-audio-system.sh $SSH_TARGET"
fi
echo ""

exit $EXIT_CODE
