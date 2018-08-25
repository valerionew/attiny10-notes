# attiny10-notes
These notes refer to the usage of the USBasp programmer with avr-gcc, avr-libc and avrdude on a linux system to compile and upload programs to an attiny10 microcontroller. With little modifications, the same notes will apply also to attiny4/5/9.

## Setting up the USBasp programmer
At the first connection of the USBasp programmer to your computer you won't have the permissions to use it.  
The way i solved this is to add an udev rule for the USBasp.  

Navigate to the `/etc/udev/rules.d/` folder. If you already have a `50-embedded_devices.rules` file open it (with **sudo**).  
Otherwise create it: `sudo touch /etc/udev/rules.d/50-embedded_devices.rules`. 

Add:
```
# USBasp Programmer rules http://www.fischl.de/usbasp/
SUBSYSTEMS=="usb", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="05dc", GROUP="users", MODE="0666"
```

Now it should be mounted correctly. 

Sources: [[1]](#sources), [[2]](#sources).



## Sources
[1]: [How to fix device permissions for the USBasp programmer - Andreas Rohner](https://andreasrohner.at/posts/Electronics/How-to-fix-device-permissions-for-the-USBasp-programmer/)
[2]: [gist: arturcieslak/50-embedded_devices.rules](https://gist.github.com/arturcieslak/f097ad370d1fc90ba98a)
