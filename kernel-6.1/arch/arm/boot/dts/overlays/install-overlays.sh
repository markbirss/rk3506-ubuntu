#!/bin/bash
# Install PCM5102A overlays to Ubuntu image
#
# This script automatically:
#   1. Builds the overlays (if needed)
#   2. Mounts the rootfs image
#   3. Copies .dtbo and .dts files to /boot/overlays/
#   4. Verifies the installation
#   5. Unmounts the image
#
# Usage:
#   ./install-overlays.sh           # Install with source files
#   ./install-overlays.sh --no-sources  # Install only .dtbo files

set -e

# Parse arguments
INCLUDE_SOURCES=1
while [[ $# -gt 0 ]]; do
    case $1 in
        --no-sources)
            INCLUDE_SOURCES=0
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--no-sources]"
            exit 1
            ;;
    esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OVERLAYS_DIR="${SCRIPT_DIR}"
KERNEL_DIR="$(cd "${SCRIPT_DIR}/../../../../.." && pwd)"
SDK_DIR="${KERNEL_DIR}/.."
ROOTFS_IMAGE="${SDK_DIR}/ubuntu/output/images/rootfs.ext4"
MOUNT_POINT="/tmp/luckfox-rootfs-mount-$$"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

cleanup() {
    if mountpoint -q "${MOUNT_POINT}" 2>/dev/null; then
        log_info "Unmounting rootfs..."
        sudo umount "${MOUNT_POINT}"
    fi
    if [ -d "${MOUNT_POINT}" ]; then
        rmdir "${MOUNT_POINT}"
    fi
}

trap cleanup EXIT

log_info "PCM5102A Device Tree Overlay Installer"
echo "========================================"
echo ""

# Check if running as root for mount operations
if [ "$EUID" -ne 0 ]; then 
    log_warn "This script requires sudo for mounting the image."
    log_info "Relaunching with sudo..."
    exec sudo bash "$0" "$@"
fi

# Step 1: Build overlays if needed
log_info "Step 1: Building overlays..."
if [ ! -f "${OVERLAYS_DIR}/rk3506-pcm5102a-sai0.dtbo" ] || \
   [ ! -f "${OVERLAYS_DIR}/rk3506-pcm5102a-sai1.dtbo" ]; then
    log_info "Overlays not found, building them..."
    cd "${OVERLAYS_DIR}"
    sudo -u ${SUDO_USER:-$USER} bash ./build-overlays.sh
else
    log_info "Overlays already built."
fi

# Step 2: Check if rootfs image exists
if [ ! -f "${ROOTFS_IMAGE}" ]; then
    log_error "Rootfs image not found at: ${ROOTFS_IMAGE}"
    log_error "Please build the image first using: ./build.sh"
    exit 1
fi

log_info "Found rootfs image: ${ROOTFS_IMAGE}"
log_info "Image size: $(du -h "${ROOTFS_IMAGE}" | cut -f1)"

# Step 3: Create mount point and mount the image
log_info "Step 2: Mounting rootfs image..."
mkdir -p "${MOUNT_POINT}"

if ! mount -o loop "${ROOTFS_IMAGE}" "${MOUNT_POINT}"; then
    log_error "Failed to mount rootfs image!"
    exit 1
fi

log_info "Rootfs mounted at: ${MOUNT_POINT}"

# Step 4: Create overlays directory in the image
log_info "Step 3: Creating /boot/overlays directory..."

# Remove old overlays if they exist (for clean reinstall)
if [ -d "${MOUNT_POINT}/boot/overlays" ]; then
    log_warn "Removing old overlays for clean installation..."
    rm -rf "${MOUNT_POINT}/boot/overlays"/*
fi

mkdir -p "${MOUNT_POINT}/boot/overlays"

# Step 5: Copy overlay files
log_info "Step 4: Copying overlay files..."
echo ""

COPIED_FILES=()

# Copy .dtbo files (always)
for dtbo in "${OVERLAYS_DIR}"/*.dtbo; do
    if [ -f "$dtbo" ]; then
        filename=$(basename "$dtbo")
        cp -v "$dtbo" "${MOUNT_POINT}/boot/overlays/"
        COPIED_FILES+=("$filename")
        log_info "  ✓ Copied: $filename"
    fi
done

# Copy .dts source files (if requested)
if [ $INCLUDE_SOURCES -eq 1 ]; then
    for dts in "${OVERLAYS_DIR}"/*.dts; do
        if [ -f "$dts" ]; then
            filename=$(basename "$dts")
            # Skip if it's not an overlay source file
            if [[ "$filename" == rk3506-pcm5102a-* ]]; then
                cp -v "$dts" "${MOUNT_POINT}/boot/overlays/"
                COPIED_FILES+=("$filename")
                log_info "  ✓ Copied: $filename (source)"
            fi
        fi
    done
else
    log_info "  Skipping source files (--no-sources)"
fi

# Copy documentation files
log_info "Step 5: Copying documentation..."
if [ -f "${OVERLAYS_DIR}/README.md" ]; then
    cp -v "${OVERLAYS_DIR}/README.md" "${MOUNT_POINT}/boot/overlays/"
    COPIED_FILES+=("README.md")
    log_info "  ✓ Copied: README.md"
fi

echo ""

# Step 6: Verify installation
log_info "Step 6: Verifying installation..."
VERIFY_OK=1

for file in "${COPIED_FILES[@]}"; do
    if [ ! -f "${MOUNT_POINT}/boot/overlays/$file" ]; then
        log_error "  ✗ Missing: $file"
        VERIFY_OK=0
    fi
done

if [ $VERIFY_OK -eq 1 ]; then
    log_info "  ✓ All files verified successfully!"
else
    log_error "  ✗ Some files are missing!"
    exit 1
fi

# Show what's in the overlays directory
echo ""
log_info "Contents of /boot/overlays in image:"
ls -lh "${MOUNT_POINT}/boot/overlays/" | tail -n +2 | while read -r line; do
    echo "  $line"
done

# Calculate total size
TOTAL_SIZE=$(du -sh "${MOUNT_POINT}/boot/overlays/" | cut -f1)
echo ""
log_info "Total size: ${TOTAL_SIZE}"

# Step 7: Unmount (will be done by trap)
echo ""
log_info "Step 7: Unmounting..."

# Success message
echo ""
echo "========================================"
log_info "${GREEN}Installation completed successfully!${NC}"
echo "========================================"
echo ""
echo "Installed files:"
for file in "${COPIED_FILES[@]}"; do
    echo "  - $file"
done
echo ""
log_info "The overlays are now embedded in the rootfs image."
log_info "Flash the update.img to your device to use them."
echo ""
log_info "On the device, overlays will be available at:"
log_info "  /boot/overlays/"
echo ""
log_info "To apply an overlay on the device:"
log_info "  1. Boot the device"
log_info "  2. See README.md in /boot/overlays/ for instructions"
echo ""
