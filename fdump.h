// fdump.h: Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

#ifndef FDUMP_H
#define FDUMP_H
	// C++ headers.
	#include <string>
	#include <iostream>
	#include <regex>
	#include <algorithm>
	#include <fstream>

	// C library headers.
	#include <cstdlib>
	#include <stdio.h>
	#include <string.h>
	#include <assert.h>

	#ifdef LINUX
		//#include <sys/file.h> // Linux only flock()
	#endif
	#ifdef BSD
		// FreeBSD, OpenBSD, NetBSD
	#endif
	#ifdef POSIX
		// POSIX headers. GNU/Linux, Unix/BSD.
		#include <signal.h>
	#endif

	// Minimal C++ Uart library.
	#include "uart.h"

	// Application defines.
	#define MY_VERSION "0.2"
	#define MY_NAME "Gerallt Franke"
	#define MY_DATE "24 April 2020 10:24 UTC."
	#define NEW_LINE "\n"

	// Platform specific defines and constants. 
	// Change here for any platform differences and recompile.

	#define DEFAULT_BAUD 115200 // The default 115200 baud rate to use.
	// Unix baud rate options:
	// 0,  50,  75,  110,  134,  150,  200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800

	const FlowControl DEFAULT_FLOW_CONTROL = FC_NONE; // Set the specified flow control.
	const ParityMode DEFAULT_PARITY_MODE = PM_NONE; // Parity mode

	// Application constants:
	#ifdef WIN32
		const std::string DEFAULT_TTY = "COM1"; // Default serial device to use. COM1 -> COM256.
	#endif

	// #ifdef LINUX
	// 	const std::string DEFAULT_TTY = "/dev/ttyUSB0"; // Default serial device to use. Can also be /dev/ttyS0 (COM1) or /dev/ttyS1 (COM2).	
	// #endif
	// #ifdef BSD
	// 	const std::string DEFAULT_TTY = "/dev/ttyS0"; // Default serial device to use. Can also be /dev/ttyS0 (COM1) or /dev/ttyS1 (COM2).	
	// #endif

	#ifdef POSIX
		const std::string DEFAULT_TTY = "/dev/ttyUSB0"; // Default serial device to use. Can also be /dev/ttyS0 (COM1) or /dev/ttyS1 (COM2).	
	#endif

	const std::string FDUMP_CMD = "fdump"; // CFE command that performs flash memory dumps that outputs data to the terminal.
	const std::string FDUMP_CMD_ARG_OFFSET = "-offset="; // Offset argument for FDUMP_CMD.
	const std::string FDUMP_CMD_ARG_SIZE = "-size="; // Size argument for FDUMP_CMD.
	const std::string HELP_CMD = "help"; // CFE help command. 
	const std::string SHOW_DEVICES_CMD = "show devices"; // CFE Command to show all devices. 
	const std::string WHITESPACE = " \n\r\t\f\v";
	const std::string REGEX_SEQ_ID = "^([0-9a-fA-F]+)"; // Looks for the sequence id in the start of a line returned.
	const std::string REGEX_HEX_DATA = "(\\b[0-9a-fA-F]{2}\\b)"; // Looks for the hex data within a line returned.
	const std::string DEFAULT_DEV_NAME = "flash0.nvram"; // "flash0.boot" // for more see 'show devices'.
	const std::string DEFAULT_FILE_EXT = ".out.bin";

	const uint8_t BYTES_PER_LINE = 16; // FDUMP_CMD usually returns 16 bytes of data at once. Likely may break if changed.
	const char EXT_CTRL_C = '\x03'; // Ctrl-c is etx so send ASCII code 0x03 \x03.

	// Application global variables:
	// Singletons are bad!
	bool parity = false; 		// Also check DEFAULT_PARITY_MODE
	uint32_t stop_bits = 1; 	// Use only one stop bit.
	uint32_t data_bits = 8; 	// How many bits per byte.
	FlowControl flow_control = FC_NONE;
	bool continue_cfe = true; 	// Disabled by POSIX sig handler.

	std::regex re_seqid(REGEX_SEQ_ID); // For tty data line parser.
	std::regex re_data(REGEX_HEX_DATA);

	std::ofstream _of_flash; // The output file stream.
	std::string* of_name = nullptr; // The output file target.
	bool output_to_file = true;

	bool verbose = false;
	bool very_verbose = false;
	bool print_data = false; 

	uint32_t offset;
	std::string* device_name = nullptr;
	std::string* tty_interface = nullptr;
	uint32_t block_size;
	uint32_t size_in_bytes;
	uint32_t blocks_to_copy;

#endif
