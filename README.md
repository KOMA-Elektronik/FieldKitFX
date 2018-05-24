# KOMA Elektronik Field Kit FX

In this repository you will find the STM32 source code for the [Field Kit FX](https://koma-elektronik.com/?product=field-kit-fx-modular-multi-effects-processor "Field Kit FX"). It controls the Looper, Bit Crusher, Sample Rate Reduction, Frequency Shifter and CV Matrix.

The scope of our code documentation is to give a quick entry into the code and its functions. It is not there to learn programming with or from it, although the programming-oriented user will find many places in which to try out things and learn new ways of coding.

-----

For uploading new code, get the Field Kit FX into bootloader mode by pressing and holding the Loop Button while powering up the unit.

We recommend either using stm32cubeprogrammer or st-flash command line tool. If you use dfu-util this is the correct command line to program your STM device:

`dfu-util -a 0 -s 0x08000000:leave -D YourFile.bin`  


Alternatively you can use the SWO-port to flash the chip directly without entering bootloader mode:

`st-flash write YourFile.bin 0x8000000`



----

**There will be no support for programming issues or units that stopped working because of altered code.**

----

### License

The code in this repository is licensed under the GNU General Public License v3.0.
