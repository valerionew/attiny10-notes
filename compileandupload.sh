#!/bin/bash

# You might want to change this path according to the position of your avr-gcc tools
# Here i'm using the toolchain included in arduino and an avrdude.conf i downloaded from somewhere
TOOLS=$HOME/arduino-1.8.9/hardware/tools/avr/bin
CONFIGFILE=./avrdude.conf

TARGET=attiny10

# Basic chech
if [[ $1 -eq 0 ]] ; then
    echo 'Usage: loadscript.sh source.c /dev/port'
    exit 1
fi
SOURCE=$1

$TOOLS/avr-gcc -Os -Wall -Wextra -mmcu=$TARGET $SOURCE -o tempbin.elf
$TOOLS/avr-objcopy -j .text -j .data -O ihex tempbin.elf tempbin.hex
avrdude -C$CONFIGFILE -v -p$TARGET -cUSBasp -Uflash:w:tempbin.hex:i

rm tempbin.hex tempbin.elf

# ../arduino-1.8.5/hardware/tools/avr/bin/avr-gcc -Wa,-adhln -g -mmcu=attiny10 -c blink.c -o blink.asm
# for debugging line to line


# see https://andreasrohner.at/posts/Electronics/How-to-fix-device-permissions-for-the-USBasp-programmer/
# for permission
