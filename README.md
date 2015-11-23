## PSoC FatFs: A port of ELM-CHAN Fat file system library to Cypress PSoC microcontrollers. ##

### About this repository. ###
* PSoCFatFsLibrary: this is the library itself.
* PSoC4FatFs: demo/testing project for PSoc 4 microcontroller.
* PSoC4M_FatFs: demo/testing project for PSoc 4M microcontroller.
* PSoC5LP_FatFs: demo/testing project for PSOC 5LP.

### Development environment. ###
* PSoC Creator 3.3 (ARM GCC 4.9 compiler)
* Cypress development kit CY8CKIT-042 PSoC 4 Pioneer Kit.
* Cypress development kit CY8CKIT-044 PSoC 4 M-Series Pioneer Kit.
* Cypress development kit CY8CKIT-059 PSoC 5LP Prototyping Kit.

    This are the tools i am using while porting the library, of course; you don´t need all of them or you can use your own hardware.
    
### Status. ###
* PSoC 4. Demo/testing software working. Probably, needs more testing.
* PSoC 4M. Same code as for PSoC 4. Demo/testing software working. Probably, needs more testing.
* PSoC 5LP. Work in progress. Reading from sdcard appear to be working, but problems while creating or writing to files.

    All demos tested with: Sandisk 2GB sdcard, Sandisk 4GB Ultra (30MB/s) SDHC I card and three other 2GB unbranded micro sd cards.

MORE INFO ASAP.