#!/bin/bash

# Define static IP
ETH_IP="192.168.100.1/24"

# Set static IP for eth0
echo "Setting static IP $ETH_IP on eth0..."
nmcli con modify eth0 ipv4.addresses "$ETH_IP"
nmcli con modify eth0 ipv4.method manual
nmcli con up eth0

# Install dnsmasq if not installed
if ! command -v dnsmasq &> /dev/null; then
    echo "Installing dnsmasq..."
    sudo apt update
    sudo apt install dnsmasq -y
else
    echo "dnsmasq is already installed."
fi

# Configure dnsmasq
echo "Configuring dnsmasq for DHCP on eth0..."
cat <<EOF | sudo tee /etc/dnsmasq.conf > /dev/null
# Use eth0 as the DHCP server interface
interface=eth0

# DHCP range and lease time (Assigns IPs from 192.168.100.100 to 192.168.100.200)
dhcp-range=192.168.100.100,192.168.100.200,12h

# Set default gateway (Raspberry Pi itself)
dhcp-option=3,192.168.100.1

# Set DNS servers (Google's DNS)
dhcp-option=6,8.8.8.8,8.8.4.4
EOF

# Restart dnsmasq service
echo "Restarting dnsmasq..."
sudo systemctl restart dnsmasq
sudo systemctl enable dnsmasq

echo "DHCP server setup complete! 🎉"
