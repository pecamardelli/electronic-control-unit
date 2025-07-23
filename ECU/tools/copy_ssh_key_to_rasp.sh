#!/bin/bash

# Configuration
REMOTE_USER="pablin"
REMOTE_HOST="192.168.0.102"
REMOTE_PORT="22"
SSH_KEY_PATH="$HOME/.ssh/id_rsa.pub" # Path to your public SSH key

# Check if the SSH key exists
if [ ! -f "$SSH_KEY_PATH" ]; then
    echo "SSH key not found at $SSH_KEY_PATH."
    echo "Generating a new SSH key pair..."
    ssh-keygen -t rsa -b 4096 -f "${SSH_KEY_PATH%.*}" -N ""
    echo "SSH key generated."
fi

# Copy the SSH key to the Raspberry Pi
echo "Copying SSH key to $REMOTE_USER@$REMOTE_HOST..."
ssh-copy-id -i "$SSH_KEY_PATH" -p "$REMOTE_PORT" "$REMOTE_USER@$REMOTE_HOST"

if [ $? -eq 0 ]; then
    echo "SSH key successfully copied! You can now SSH to the Raspberry Pi without a password."
else
    echo "Failed to copy SSH key. Please check the configuration and try again."
fi
