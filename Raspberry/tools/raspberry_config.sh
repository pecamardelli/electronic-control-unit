#!/bin/bash

FILE="/boot/firmware/config.txt"
LINE="dtoverlay=gpio-poweroff,gpiopin=17,active_high=1"

if ! grep -Fxq "$LINE" "$FILE"; then
    echo "$LINE" >> "$FILE"
    echo "Line added to $FILE."
else
    echo "Line already exists in $FILE. No changes made."
fi

echo "Creating service file..."

SERVICE_FILE="/etc/systemd/system/$1.service"

tee $SERVICE_FILE > /dev/null <<EOT
[Unit]
Description=Custom Electronic Control Unit
After=network.target

[Service]
ExecStart=$2/$1
WorkingDirectory=$2
User=root
Group=root
Restart=always
StandardOutput=syslog
StandardError=syslog

[Install]
WantedBy=multi-user.target
EOT > /dev/null

# systemctl daemon-reload
# systemctl start $1.service
# systemctl enable $1.service
# systemctl status $1.service





