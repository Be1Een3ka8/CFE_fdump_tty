### CFE fdump over serial tty interface
#### A program to help make backup copies of router flash memory through a serial CFE console using fdump, pipe to binary file or screen.


[1. Quick jump to Compilation on GNU/Linux](../../../CFE_fdump_tty#compilation-gnulinux-version "Quick jump to Compilation on GNU/Linux")

[2. Quick jump to Compilation on Unix/BSD](../../../CFE_fdump_tty#compilation-unixbsd-version "Quick jump to Compilation on Unix/BSD")

[3. Quick jump to Compilation on Windows](../../../CFE_fdump_tty#compilation-windows-version "Quick jump to Compilation on Windows")

###### fdump: 

Example dump first 128 bytes of flash0 nvram to screen:

![Dump to screen](https://github.com/geralltf/CFE_fdump_tty/raw/master/screenshots/test0_dump_to_screen.png "Example dump to screen.")

Example dump first 128 bytes of nvram to an output file:

![Dump to file](https://github.com/geralltf/CFE_fdump_tty/raw/master/screenshots/test1_dump_to_file.png "Example dump to file.")

Different command that dumped *all* nvram to file, and checking the file is sane in a hex editor: 

![Dumped nvram binary output](https://github.com/geralltf/CFE_fdump_tty/raw/master/screenshots/test1_dump_to_file_all_nvram.png "Dumped nvram opened in hex editor.")

    ./fdump if=flash0.nvram of=f0.nvram.bin offset=0 bs=65536 size=65536 -v -l


Make sure you have added current user to dialout group.


Description: 

    Clone flash memory on CFE terminal using fdump commands over serial tty.

Note:
 
    1. Make sure you have added current $USER to dialout group and have a working serial tty interface.

    2. On Windows you may initially get the weird ERROR_FILE_NOT_FOUND
       some combination of uninstalling the serial driver and reinstalling
       unsure if setting the SECURITY_DESCRIPTOR to Everyone group
       will let you run as a local user instead of as Administrator.
       Reinstalling driver seemed to fix this issue.

 Usage: 

    1. Boot into CFE using another program like Putty to interact with the serial tty. 

    2. When you have a CFE> console, quit Putty and run this program instead. 

    3. You can have both programs running on the same tty however they will both compete 
       and the data will likely get scrambled.

    4. This program will automate the sending of command(s) to CFE to extract flash memory,
       and save it locally in an image.

    5. You may need to change the baud rate (DEFAULT_BAUD B115200), if it's a slow speed you will 
       need to set VTIME_APPLIED to VTIME_SLOW and recompile. There are Comm Timeout settings for Windows.

    6. It works a bit like Unix dd, to read the flash you will need to at least specify:

      1. The device name e.g. flash0, or flash0.boot, or flash0.nvram, etc.

      2. Block size and the size (in bytes) of the data to read.

      3. It is optional to specify the offset and if to output into an output file.

      4. Other settings include: verbose, very verbose, and if to print the data.

    Block size is a multiple of 16 (BYTES_PER_LINE) and can be set to the
    total size of the data to read. Having small values for block size
    with a really big read decreases the efficency of the read, esp on a serial tty.

Required options are: if=, offset=, bs=, size=

    1. if           - Input flash device name e.g. flash0.boot, flash0.boot2, flasho0.trx,
                      flasho0.os, flash0.nvram, etc.

    2. offset/skip  - The offset (in bytes) into memory to use. Most of the time it is fine
                      to set offset=0 to read from the beginning.

    3. bs           - The block size. Must be set to multiples of 16 to help parser.
                      Small block sizes and a really large 'size' set is really slow for reads.

    4. size/count   - The count in bytes of memory to copy. All values are in decimal.
                      Importantly you will likely need to know the exact size you need to copy
                      which can be hard to figure out.

Valid options are:

    1. -h / -help / --help, Display the help and exit.

    2. -of for output file

    3. -v for verbose, -vv for very verbose, -l to print the data like hexdump.

    4. -tty=/dev/ttyUSB0   To change the tty serial device.

    5. -tty=/dev/ttyS0     Can also use an older serial port (COM1)

    6. -tty=/dev/ttyS1     (COM2)

    7. -tty=COM1           To change the tty usb serial device on Windows.
                           Maybe also try COM2, or anything above COM10 to COM256.

   You may also need to change the baud rate and settings which are: 115200 8/N/1

   *To do that you will have to change the code and recompile.

 Examples:
   To list the first 640 bytes of flash0.nvram without saving to file use:

       ./fdump if=flash0.nvram offset=0 bs=64 size=640 -vv -l

       ./fdump tty=/dev/ttyS0 if=flash0.nvram offset=0 bs=64 size=640 -vv -l

   To list and store a large block of nvram in a file (large block size to make reads quicker):

       ./fdump if=flash0.nvram of=f0.nvram.bin offset=0 bs=65536 size=65536 -v -l

 Known Issues: You may press ctrl-c to cancel, however it likely will not cancel the operation on the CFE console.

    Date:     24 April 2020 10:24 UTC.
    Author:   Gerallt Franke
    Version:  0.2

#### Compilation: (GNU/Linux version)

To compile on Linux open a terminal and cd into this directory. 

Just type: 

    $ make

This will build the Release target. To build the Debug target do:

    $ make Debug

To clean up all binaries and object files do:

    $ make clean 

Linux will always make the GNUmakefile and require GNU compilers, this project uses g++ std=c++17.

There is no 'make install' target as you can just run ./fdump &lt;options&gt; as this is a small utility. Formal install options and dist packaging might be added later as things progress. 

#### Compilation: (Unix/BSD version)

Yes you can compile on Unix/BSD systems without using hacks like gmake or downloading any GNU toolchains.
You might of course first need to install git or curl to get the sources.

To compile on OpenBSD, FreeBSD, NetBSD open a terminal and cd into this directory. 

Just type: 

    $ make

This will build the Release target. To build the Debug target do:

    $ make Debug

To clean up all binaries and object files do:

    $ make clean 

Depending on what Unix/BSD is used, make will either pick BSDmakefile or Makefile. Luckily Makefile realizes this and passes on parameters to whichever implementation is required. Depending on the BSD platform, this project uses clang++ or g++ with std=c++17.

There is no 'make install' target as you can just run ./fdump &lt;options&gt; as this is a small utility. Formal install options and dist packaging might be added later as things progress. 

#### Compilation: (Windows version)

You may compile on Windows just open the solution file (.sln) in Visual Studio 2019 Community Edition and build the project.
The project should build straight away but only after you download the Windows 10 SDK which is an optional feature in the Visual Studio Installer so that you have all the C/C++ development header file dependencies.

#### Compilation: (macOS, OS X)

So this will highly likely run in macOS (maybe without changing much) I haven't tested this platform, but given I've heard unistd.h termios.h might still work and the build system has POSIX compatibility, macOS has a shell/Terminal, git tools, and a text editor like vi or vim.
You can use Xcode. Once you have installed Xcode you'll have the LLVM/Clang or GNU compilers and likely the header libraries. 
You will need g++ or clang++ and should be able to run the respective makefile or add the files to a fresh C++ Xcode console project and use the IDE to build. 
The preprocessor definition 'POSIX' must be specified to the compiler if you are using Xcode so the uart library uses the correct implementation.

To compile on macOS without the IDE:

Open Terminal (Applications → Utilities → Terminal) and cd into this directory. 

Just type: 

    $ make -f <GNU or BSD>makefile

This will build the Release target. To build the Debug target do:

    $ make -f <GNU or BSD>makefile Debug

To clean up all binaries and object files do:

    $ make -f <GNU or BSD>makefile clean 