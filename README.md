# BerryLan

**Raspberry Pi WIFI setup**

Get your headless Raspian into your WIFI without the Console

![BerryLan](https://berrylan.app/assets/gif/overview.gif)

## Download & installation 

You can setup the wireless LAN of your headless Raspberry PI in just three steps.

#### Etch the image

Download the BerryLan flavoured [Raspbian image](https://downloads.nymea.io/images/berrylan/raspbian-stretch-berrylan-lite-latest.zip) and flash the SD Card. We recommend Etcher to do that.

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

#### Open Source
We love Open Source & Qt. Contribute here.

## Installation on a plain Raspbian

Add the nymea repository to your system:

    echo "deb http://repository.nymea.io stretch main raspbian" | sudo tee /etc/apt/sources.list.d/nymea.list
    sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key A1A19ED6

Configure apt to prefer packages from the raspbian section of the repository

> Note: the **Raspberry Pi Zero W** requires packages from the `raspbian` section, since they are not plain `armhf`, but specially built for all Raspberry Pi versions. If you are on Raspberry Pi 2/3 you can skip this step.

    nano /etc/apt/preferences.d/nymea

Add following lines to the file:

    Package: *
    Pin: release c=raspbian
    Pin-Priority: 700

    Package: *
    Pin: origin repository.nymea.io c=main
    Pin-Priority: 500


Install the dependencies:

    sudo apt-get update
    sudo apt-get install nymea-networkmanager dirmngr

Disable dhcpcd:

    sudo systemctl disable dhcpcd
    reboot

Thats it! Have fun with BerryLan!
