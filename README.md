# NRF24L01

How to Flash the Atmega328p using avrdude

go to: C:\WinAVR-20100110\bin\
open a CMD console
type: avrdude -p atmega328p -b 57600 -c arduino -P COM3 -U flash:w:<filename>:i
If successful the following text will displayed:

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.01s

avrdude: Device signature = 0x1e950f
avrdude: NOTE: FLASH memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: reading input file "C:\Users\Ogben\Desktop\RX.hex"
avrdude: writing flash (2056 bytes):

Writing | ################################################## | 100% 0.70s

avrdude: 2056 bytes of flash written
avrdude: verifying flash memory against C:\Users\Ogben\Desktop\RX.hex:
avrdude: load data flash data from input file C:\Users\Ogben\Desktop\RX.hex:
avrdude: input file C:\Users\Ogben\Desktop\RX.hex contains 2056 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 0.58s

avrdude: verifying ...
avrdude: 2056 bytes of flash verified

avrdude: safemode: Fuses OK

avrdude done.  Thank you.
