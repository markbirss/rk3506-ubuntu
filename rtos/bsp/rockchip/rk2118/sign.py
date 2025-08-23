#!/usr/bin/env python3

import subprocess
import argparse

# Default parameters if not provided
DEFAULT_CHIP = "2118"
DEFAULT_KEY = "../tools/sign_tool/private_key.pem"
DEFAULT_PUBKEY = "../tools/sign_tool/public_key.pem"
DEFAULT_FIRMWARE = "./Image/update.img"
DEFAULT_LOADER = "./Image/MiniLoaderAllDDR.bin"
SIGN_TOOL_PATH = "../tools/sign_tool/sign_tool"
DEFAULT_FLAG = "0x20"

def run_command(command):
    """Execute the given command and display the output"""
    try:
        result = subprocess.run(command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e.stderr}")

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Tool for executing secure signing operations")
    parser.add_argument('--chip', default=DEFAULT_CHIP, help="Specify the chip type (default: 2118)")
    parser.add_argument('--key', default=DEFAULT_KEY, help="Path to the RSA private key (default: ../tools/sign_tool/private_key.pem)")
    parser.add_argument('--pubkey', default=DEFAULT_PUBKEY, help="Path to the RSA public key (default: ../tools/sign_tool/public_key.pem)")
    parser.add_argument('--firmware', default=DEFAULT_FIRMWARE, help="Path to the firmware file (default: ./Image/update.img)")
    parser.add_argument('--loader', default=DEFAULT_LOADER, help="Path to the loader file (default: ./Image/MiniLoaderAllDDR.bin)")
    parser.add_argument('--flag', default=DEFAULT_FLAG, help="Signing flag to enable OTP (default: 0x20)")

    args = parser.parse_args()

    # Execute chip selection command
    command_chip = f"{SIGN_TOOL_PATH} cc --chip={args.chip}"
    print(f"Executing command: {command_chip}")
    run_command(command_chip)

    # Execute key selection command
    command_key = f"{SIGN_TOOL_PATH} lk --key={args.key} --pubkey={args.pubkey}"
    print(f"Executing command: {command_key}")
    run_command(command_key)

    # Set the signing flag for OTP
    command_flag = f"{SIGN_TOOL_PATH} ss --flag={args.flag}"
    print(f"Executing command: {command_flag}")
    run_command(command_flag)

    # Execute firmware signing command
    command_firmware = f"{SIGN_TOOL_PATH} sf --firmware={args.firmware}"
    print(f"Executing command: {command_firmware}")
    run_command(command_firmware)

    # Execute loader signing command
    command_loader = f"{SIGN_TOOL_PATH} sl --loader={args.loader}"
    print(f"Executing command: {command_loader}")
    run_command(command_loader)

if __name__ == "__main__":
    main()
