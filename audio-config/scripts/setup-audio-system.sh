#!/bin/bash
# Audio System Setup Script
# Configures user permissions and ALSA configuration on target board

set -e

SSH_TARGET="${1:-lyra@192.168.123.100}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ASOUND_CONF="$(dirname "$SCRIPT_DIR")/asound.conf"

echo "======================================================================="
echo "  PCM5102A Audio System Setup"
echo "  Target: $SSH_TARGET"
echo "======================================================================="
echo ""

# Check if we can reach the board
echo "1. Testing SSH connection..."
if ! ssh -o ConnectTimeout=5 -o BatchMode=yes "$SSH_TARGET" "echo 'Connection OK'" >/dev/null 2>&1; then
    echo "ERROR: Cannot connect to $SSH_TARGET"
    echo ""
    echo "Make sure:"
    echo "  - Board is powered on and connected to network"
    echo "  - SSH keys are set up (run: ssh-copy-id $SSH_TARGET)"
    echo "  - IP address is correct"
    echo ""
    exit 1
fi
echo "✓ SSH connection successful"
echo ""

# Add user to audio group
echo "2. Adding user to audio group..."
USERNAME=$(echo "$SSH_TARGET" | cut -d'@' -f1)
ssh "$SSH_TARGET" "sudo usermod -aG audio $USERNAME 2>/dev/null && echo '✓ User added to audio group' || echo '✓ User already in audio group'"
echo ""

# Check if asound.conf exists locally
if [ ! -f "$ASOUND_CONF" ]; then
    echo "WARNING: asound.conf not found at: $ASOUND_CONF"
    echo "Creating default configuration..."
    cat > "$ASOUND_CONF" <<'EOF'
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
fi

# Deploy ALSA configuration
echo "3. Deploying ALSA configuration..."
cat "$ASOUND_CONF" | ssh "$SSH_TARGET" "sudo tee /etc/asound.conf > /dev/null"
echo "✓ /etc/asound.conf written"
echo ""

# Verify audio group membership
echo "4. Verifying configuration..."
GROUPS_OUTPUT=$(ssh "$SSH_TARGET" "groups $USERNAME")
if echo "$GROUPS_OUTPUT" | grep -q "audio"; then
    echo "✓ User '$USERNAME' is in audio group"
else
    echo "✗ ERROR: User '$USERNAME' NOT in audio group!"
    exit 1
fi

# Check ALSA config
if ssh "$SSH_TARGET" "[ -f /etc/asound.conf ]"; then
    echo "✓ /etc/asound.conf exists"
else
    echo "✗ ERROR: /etc/asound.conf not found!"
    exit 1
fi
echo ""

# Check sound card
echo "5. Checking sound card..."
if ssh "$SSH_TARGET" "cat /proc/asound/cards" | grep -q "PCM5102ASAI1"; then
    echo "✓ Sound card found:"
    ssh "$SSH_TARGET" "cat /proc/asound/cards" | sed 's/^/  /'
else
    echo "⚠ WARNING: PCM5102ASAI1 sound card not found"
    echo "  Make sure you flashed the correct device tree!"
    echo ""
    echo "Current sound cards:"
    ssh "$SSH_TARGET" "cat /proc/asound/cards" | sed 's/^/  /'
fi
echo ""

# Summary
echo "======================================================================="
echo "  SETUP COMPLETE"
echo "======================================================================="
echo ""
echo "Configuration applied:"
echo "  ✓ User '$USERNAME' added to audio group"
echo "  ✓ /etc/asound.conf configured"
echo ""
echo "IMPORTANT: User must logout and login for audio group to take effect!"
echo ""
echo "Next steps:"
echo "  1. Logout/login or reboot the board"
echo "  2. Run verification: ./verify-audio-remote.sh $SSH_TARGET"
echo "  3. Test audio: ssh $SSH_TARGET 'speaker-test -D hw:0,0 -t sine -f 1000'"
echo ""
