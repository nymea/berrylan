# BerryLan

**Raspberry Pi WIFI setup**

Get your headless Raspian into your WIFI without the Console

![BerryLan](https://berrylan.app/assets/gif/overview.gif)

## Download & installation 

You can setup the wireless LAN of your headless Raspberry PI in just three steps.

#### Etch the image

Download the BerryLan flavoured [Raspbian image](https://downloads.nymea.io/images/raspberrypi/latest) and flash the SD Card. We recommend Etcher to do that.

#### Power on

Insert the SD Card and power on your Raspberry Pi. All you need is your Raspberry and a power supply.

#### Get the App

Install the BerryLan iOS or Android App and follow the instructions.
That's it, have fun!

## Features 

### Plain Raspbian
Everything is like you are used to - we just add BerryLan.

#### IP Address
The app displays you the IP Address at the end of the setup process.

#### Instant Feedback

The app tells you if there might be an issue with the WLAN.

## Licensing

BerryLan, being a nymea tool, follows the nymea license model (dual-licensed GPLv3 and nymea commercial).
For more informations please visit https://www.nymea.io/documentation/overview/license

## Installation on a plain Raspbian

Add the nymea repository to your system:

    echo -e "deb http://repository.nymea.io $(lsb_release -s -c) main" | sudo tee /etc/apt/sources.list.d/nymea.list
    sudo wget -O /etc/apt/trusted.gpg.d/nymea.gpg https://repository.nymea.io/nymea.gpg

Install the dependencies:

    sudo apt-get update
    sudo apt-get install nymea-networkmanager dirmngr

Disable dhcpcd:

    sudo systemctl disable dhcpcd
    reboot

Thats it! Have fun with BerryLan!
