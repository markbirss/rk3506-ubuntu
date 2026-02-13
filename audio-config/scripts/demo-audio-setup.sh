#!/bin/bash
# Quick demo of automated audio setup and verification

set -e

SSH_TARGET="${1:-lyra@192.168.123.100}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "======================================================================="
echo "  Audio Setup & Test Demo"
echo "  Target: $SSH_TARGET"
echo "======================================================================="
echo ""

# 1. Run setup
echo "Step 1: Setting up audio system..."
"$SCRIPT_DIR/setup-audio-system.sh" "$SSH_TARGET"

# 2. Ask user to logout/login
echo ""
echo "======================================================================="
echo "IMPORTANT: Audio group requires logout/login to take effect!"
echo "======================================================================="
echo ""
read -p "Press ENTER after you have logged out and back in on the board..." 

# 3. Run verification
echo ""
echo "Step 2: Verifying configuration..."
"$SCRIPT_DIR/verify-audio-remote.sh" "$SSH_TARGET"
EXIT_CODE=$?

# 4. If verification passed, offer to play test tone
if [ $EXIT_CODE -eq 0 ]; then
    echo ""
    echo "======================================================================="
    echo "Setup complete! Ready to test audio."
    echo "======================================================================="
    echo ""
    read -p "Play test tone for 5 seconds? [Y/n] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
        echo ""
        echo "Playing 1kHz test tone on both channels..."
        ssh "$SSH_TARGET" 'speaker-test -D hw:0,0 -t sine -f 1000 -c 2' &
        TEST_PID=$!
        sleep 5
        kill $TEST_PID 2>/dev/null || true
        echo ""
        echo "Did you hear the sound? If yes, audio is working! 🎵"
    fi
else
    echo ""
    echo "======================================================================="
    echo "Verification failed. Check DIAGNOSIS.md for troubleshooting."
    echo "======================================================================="
fi
echo ""
