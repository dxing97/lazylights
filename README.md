# Lazylights: IoT Lights for the Lazy and Cheap

Rough diagram:
``HomeKit <-> Homebridge <-> Homebridge-http <- REST API on Local Network -> Arduino Ethernet Module <-> Arduino``

Requirements:
* Arduino (tested with Arduino Nano)
* Arduino Ethernet Module (tested with ENC28J60 module) 
* Device capable of running Homebridge (tested with Raspberry Pi 3, W, Intel Xeon server running Debian)
* The light you want to control, along with hardware to allow your Arduino to control it. (LED strip, 12V power supply, and high power NMOS or BJT)

Steps:
* Setup Homebridge
  * Make sure HomeKit apps can talk to Homebridge
* Setup Arduino
  * Make sure you can make REST requests to the Arduino and that the Arduino can respond to them
* Configure Arduino and Homebridge to talk together
  * Tell Homebridge what routes to use and how to use them
  * Tell Arduino what routes to respond to and how to act on them

## Possible Improvements:
* Obtain a IP address using DHCP
* Learn node.js, fork Homebridge-http and add support for more devices on Homebridge-http and on the Arduino
* Using Ethercard or other ethernet libraries to get a smaller memory footprint (less of a problem on the Arduino Mega, big problem on the Uno and Nano)

## Acknowledgements
Homebridge-http config is a slightly tweaked example config provided by homebridge-http

The arduino code for implementing a basic REST server is a heavily modified version of the code found [here, under UIPEthernet.](https://www.tweaking4all.com/hardware/arduino/arduino-enc28j60-ethernet/) Great article, highly recommended read if you want to use a ENC28J60-based ethernet devices. 
