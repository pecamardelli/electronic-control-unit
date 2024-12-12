# This will activate the main relay a few seconds earlier than the program.
echo "dtoverlay=gpio-poweroff,gpiopin=17,active_high=1" >> /boot/firmware/config.txt