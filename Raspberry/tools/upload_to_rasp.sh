#!/bin/bash

# Configuration
LOCAL_PATH="/home/pablin/electronic-control-unit/Raspberry"
REMOTE_PATH="/home/pablin/ecu"
HOST="192.168.0.102"
USER="pablin"
SSH_KEY_PATH="$HOME/.ssh/id_rsa"            # Path to your private SSH key

# Function to install lftp
install_lftp() {
    echo "lftp not found. Installing lftp..."

    # Check the package manager and install lftp
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y lftp
    elif command -v yum &> /dev/null; then
        sudo yum install -y lftp
    elif command -v brew &> /dev/null; then
        brew install lftp
    else
        echo "Unsupported package manager. Please install lftp manually."
        exit 1
    fi
}

# Check if lftp is installed, and install it if not
if ! command -v lftp &> /dev/null; then
    install_lftp
fi

# Perform the incremental transfer
lftp -u "$USER","DUMMY_PASSWORD" sftp://"$HOST" <<EOF
mirror --reverse --verbose $LOCAL_PATH $REMOTE_PATH
exit
EOF