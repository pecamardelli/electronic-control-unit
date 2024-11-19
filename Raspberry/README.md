## We're using the BCM2835 Library on this Ecu software

The BCM2835 library is a C library for accessing GPIOs and other peripherals on the Broadcom BCM2835 chip used in Raspberry Pi. Follow these steps to download, build, and install the library.

### 1. Download the Library
1. Open a terminal on your Raspberry Pi.
2. Download the latest version of the library from the official website:
   ```bash
   wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
   ```

   *(Replace `1.71` with the latest version if available.)*

### 2. Extract the Files
1. Extract the downloaded tarball:
   ```bash
   tar zxvf bcm2835-1.71.tar.gz
   ```

2. Navigate to the extracted directory:
   ```bash
   cd bcm2835-1.71
   ```

### 3. Build and Install the Library
1. Build the library using the following commands:
   ```bash
   ./configure
   make
   sudo make install
   ```

2. Verify that the library is installed correctly by checking the installed files in `/usr/local/lib` and `/usr/local/include`.

### 4. Enable it on the Makefile
1. Open the Makefile on the root directory and uncomment the following line:
   ```c
   USELIB = USE_BCM2835_LIB
   ```

2. Make sure the other libraries are commented out.

### Notes
- The BCM2835 library requires root privileges for some operations. Run your programs with `sudo` if necessary.
- Refer to the [official documentation](http://www.airspayce.com/mikem/bcm2835/) for additional details and examples.

## Running the Ecu at Startup Using systemd

### 1. Create a Systemd Service File
1. Open a terminal and create a new service file:
   ```bash
   sudo nano /etc/systemd/system/ecu.service
   ```

2. Add the following content, replace paths if necessary:

   ```ini
   [Unit]
   Description=My Custom Binary
   After=network.target

   [Service]
   ExecStart=/home/pablin/c/torino-ecu
   WorkingDirectory=/home/pablin/c
   User=root
   Group=root
   Restart=always
   StandardOutput=syslog
   StandardError=syslog

   [Install]
   WantedBy=multi-user.target
   ```

3. Save and close the file (`Ctrl + O`, `Enter`, `Ctrl + X`).

### 2. Enable and Start the Service
1. Reload `systemd` to recognize the new service:
   ```bash
   sudo systemctl daemon-reload
   ```

2. Enable the service to run at startup:
   ```bash
   sudo systemctl enable ecu.service
   ```

3. Start the service immediately:
   ```bash
   sudo systemctl start ecu.service
   ```

### 3. Verify the Service
- Check the status of the service:
  ```bash
  sudo systemctl status ecu.service
  ```

- View logs for the service:
  ```bash
  journalctl -u ecu.service
  ```

### Notes
- Ensure the binary is executable. If not, make it executable with:

  ```bash
  chmod +x /path/to/your/binary
  ```
- If you want to update or debug the service, edit the service file and reload `systemd`:

  ```bash
  sudo systemctl daemon-reload
  sudo systemctl restart ecu.service
  ```
