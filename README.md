# zigbee temperature sensor

This repository contains project about a temperature sensor based on Xbee device.
It will be coupled with the zigbee controler and the web_monitoring projets to resp. retrieve the value from the sensor 
and display thought a web site.


## Hardware part
The hardware folder groups schematics. it is based on a PIC device (PIC18LF2420) coupled with a XBee module (from Digi)
Temperature and Humidity sensor is a HYT221


## Firmware part
The folder contains source code to build the firmware for the PIC device.

## Features
 - read temperature and humidity around each minutes
 - read the battery voltage
 - PIC clock calibration
Most of 
 
##### Note:
 - acquisition period will be reviewed to improve consumption
 
 First test gives a autonomy of around 1 we with a temperature/humidity monitoring of 1min and battery monitoring at 5min

