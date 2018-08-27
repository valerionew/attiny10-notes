# ATtiny10-notes
These notes refer to the usage of the USBasp programmer with avr-gcc, avr-libc and avrdude on a linux system to compile and upload programs to an ATtiny10 microcontroller. With little modifications, the same notes will probably apply also to ATtiny4 and ATtiny5.

## Setting up the USBasp programmer
ATtinys use the TPI interface for programming. Make sure to have your usbasp updated to the latest firmware, otherwise TPI is not supported.  
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

## Compiling
The most basic command you can compile with is:
```
avr-gcc -mmcu=attiny10 $MYPROGRAM -o $MYPROGRAM.elf
```
Where $MYPROGRAM is your source file name, such as `blink.c`.  

This probably will work for the most basic programs, but soon you'll get in trouble. See the [Setting the prescaler](#setting-the-prescaler) section for example.  
At some point, probably you'll want at least to add some optimization. Given the limited memory of the tiny10, `-Os` (optimize level 2, but perform only the options that don't increase the size) would be the best optimization type. This is not something specific to `avr-gcc`, it is simply from the GNU C compiler.
```
avr-gcc -Os -mmcu=attiny10 $MYPROGRAM -o $MYPROGRAM.elf
```

If you want to get more information about warnings, you can turn all on with:
```
avr-gcc -Wall -Wextra -Wpedantic -Werror -Os -mmcu=attiny10 $MYPROGRAM -o $MYPROGRAM.elf
```
See [The GCC reference for warning options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) to select which ones you want to enable.

## Check the assembly output
At some point in spacetime you will at least want to check what's happening under the hood.
You can do this in multiple ways. I found that the way that i prefer is:  
```
avr-gcc -Os -g -Wa,-adhln -mmcu=attiny10 $MYPROGRAM
```
It meshes the assembly lines with the c lines, so that you can see what's beeing compiled to what.  
Source [[4]](#sources)

## Setting the prescaler
At first, setting the prescaler wasn't an easy task.  
The datasheet [[3]](#sources) states:
> To avoid unintentional changes of clock frequency, a protected change sequence must be followed to
change the CLKPS bits:
> 1. Write the signature for change enable of protected I/O register to register CCP
> 2. Within four instruction cycles, write the desired value to CLKPS bits

Where "the signature" is `0xD8`. It is a neat safety feature, but it can get tou in trouble. 
The first thing i did was:
```
  char oldsreg = SREG;   // save the interrupt setting register to oldsreg
  cli();                 // disable all the interrupts (SREG = 0)
  CCP = 0xD8;            // signature to CCP
  CLKMSR = 0;            // use clock 00: Calibrated Internal 8 MHzOscillator  
  CCP = 0xD8;            // signature
  CLKPSR = 0;            // set prescaler to :1 (0x00)
  SREG = oldsreg;        // restore the sreg, enabling the interrupts
```
Compiling without optimization didn't work. If you check the assembly output, you'll get:
```
  10:blink-prescaler-register.c ****   CCP = 0xD8;       // signature to CCP
  54               		.loc 1 10 0
  55               	/* #NOAPP */
  56 0022 4CE3      		ldi r20,lo8(60)
  57 0024 50E0      		ldi r21,0
  58 0026 68ED      		ldi r22,lo8(-40)
  59 0028 E42F      		mov r30,r20
  60 002a F52F      		mov r31,r21
  61 002c 6083      		st Z,r22
  11:blink-prescaler-register.c ****   CLKMSR = 0;       // use clock 00: Internal 8 MHz
  62               		.loc 1 11 0
  63 002e 47E3      		ldi r20,lo8(55)
  64 0030 50E0      		ldi r21,0
  65 0032 E42F      		mov r30,r20
  66 0034 F52F      		mov r31,r21
  67 0036 1083      		st Z,__zero_reg__
```
If you consider, for example from the instruction at memory addres 002c (signature loaded into CCP) to instruction at 0036, when 0 is loaded into CLKMSR, there are obviously more than 4 instructions.  
The compiler is doing something weird to my eyes. Of course, to an expert, this would seem totally expected.  
At first loads the immediate 60 in r20. 60 is the address of the CCP register. Then loads zero into r21.  
At this point loads the immediate -40 (which is 0xD8 in two's complement) into r22.  
Then moves r20 and r21 to r30 and r31. These two destination registers are at least a bit special, because they are *address registers*. r31 is the high byte, and r30 is the low byte of an address. Toghether, the asm refers to them with "Z".  
Then, loads 0xD8 (stored in r22) to the CCP register, addressed by the Z register.   
Why not loading everything directly to the Z register? Compilation weirdness.  

If you take the same code and compile it with the `-Os` option, you get:
```
  10:blink-prescaler-register.c ****   CCP = 0xD8;       // signature to CCP
  31               		.loc 1 10 0
  32               	/* #NOAPP */
  33 0004 48ED      		ldi r20,lo8(-40)
  34 0006 4CBF      		out __CCP__,r20
  11:blink-prescaler-register.c ****   CLKMSR = 0;       // use clock 00: Internal 8 MHz
  35               		.loc 1 11 0
  36 0008 17BF      		out 0x37,__zero_reg__
```
Now we're talking! Loading the value into CLKMSR is the very next operation. The prescaler is working. 

If you prefer, you can take two alternative ways:
- Embed a piece of assembler directly into the c code
- Use the `clock_prescale_set(clock_div_N)` from `<avr/power.h>`, where N is the desired prescaler. But there's no fun or learning in this option. 


## Sources
[1]: [How to fix device permissions for the USBasp programmer - Andreas Rohner](https://andreasrohner.at/posts/Electronics/How-to-fix-device-permissions-for-the-USBasp-programmer/)  
[2]: [gist: arturcieslak/50-embedded_devices.rules](https://gist.github.com/arturcieslak/f097ad370d1fc90ba98a)  
[3]: [ATtiny10 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8127-AVR-8-bit-Microcontroller-ATtiny4-ATtiny5-ATtiny9-ATtiny10_Datasheet.pdf)  
[4]: [SYSTutorials - Generating Mixed Source and Assembly List using GCC](https://www.systutorials.com/240/generate-a-mixed-source-and-assembly-listing-using-gcc/)  
