#!/bin/bash

# Variables for static IP configuration
STATIC_IP="192.168.0.250/24"
GATEWAY="192.168.0.1"
DNS="8.8.8.8,8.8.4.4"

# Function to set static IP for a connection
set_static_ip() {
  local connection_name="$1"
  
  echo "Configuring connection: $connection_name"
  
  # Set the static IP, gateway, and DNS
  nmcli con modify "$connection_name" ipv4.addresses "$STATIC_IP"
  nmcli con modify "$connection_name" ipv4.gateway "$GATEWAY"
  nmcli con modify "$connection_name" ipv4.dns "$DNS"
  nmcli con modify "$connection_name" ipv4.method manual
  
  # Bring the connection up
  nmcli con up "$connection_name"
}

# Get the list of Wi-Fi connections
echo "Retrieving available Wi-Fi connections..."
connections=$(nmcli -t -f NAME,TYPE con show | grep wifi | cut -d: -f1)

if [[ -z "$connections" ]]; then
  echo "No Wi-Fi connections found!"
  exit 1
fi

# Loop through each connection and set static IP
for connection in $connections; do
  set_static_ip "$connection"
done

echo "All Wi-Fi connections have been configured with a static IP."
