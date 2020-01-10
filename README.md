# ftdi-eeprom-mod

A simple utility to modify bytes in an FTDI EEPROM image.

I needed this to make modifications to an FTDI chip on an ECP5 EVN FGPA
board that was programmed incorrectly. The first two bytes in the image
specify what type of port is connected, and they were both marked as
FIFO (0x01). Lattice must be misprogramming these devices, because the
second one is meant to be a serial port (0x08).

Note: This won't work on FT230X chips.

## Steps

* Install ftdi_eeprom. On Fedora: `dnf install libftdi-devel`

* Create a config file:

```
cat << EOF > ftdi.conf
vendor_id=0x403
product_id=0x6010
filename="eeprom.bin"
flash_raw=true
EOF
```

* Write the current flash image to a file, back it up, modify it then
  write it back to the device:

```
ftdi_eeprom --read-eeprom ftdi.conf
cp eeprom.bin eeprom.bin~
ftdi-eeprom-mod eeprom.bin 0x1 0x08
ftdi_eeprom --flash-eeprom ftdi.conf
```
