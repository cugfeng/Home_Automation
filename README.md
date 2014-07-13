Home Automation
===============
The purpose of this project is to monitor and control home environment, expecially temperature.

Hardware preparation:
  1. Raspberry Pi
  2. Temperature Sensor. I choose DS18B20
  3. IR Transmitter

Software preparation:
  1. Linux. I choose Debian
  2. Apache and PHP

Raspberry Pi will monitor home temperature. Use computer or mobile device, you can get current temperature and control airconditioner remotely.

This project is still ongoing. Features ready until now are:
  1. Monitor temperatrue on Raspberry Pi
  2. Get current temperature on computer and mobile device

TODO:
  1. Control IR transmitter on Raspberry Pi
  2. Control IR transmitter on computer and mobile device

Usage:
  1. Run 'make' to build temperature 'execuable'
  2. Copy files under directory 'html' into Apache root directory 'www'
  3. Run 'script/init.sh' to initialize temperature sensor driver
  4. Run 'temperature'
  5. Access Apache server in browser on computer or mobile device

If you have any advice, feel free to contact me at cugfeng#gmail.com.
