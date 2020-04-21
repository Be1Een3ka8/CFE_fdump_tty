// fdump.h: Author Gerallt Franke.
// Date: 20 April 2020 16:29 UTC. 

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

//#include <sys/file.h> // Linux flock()
#include <signal.h>

// Linux headers.
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// Application defines.
#define MY_VERSION "0.1, First version"
#define MY_NAME "Gerallt Franke"
#define MY_DATE "18 April 2020 13:27 UTC."
#define NEW_LINE "\n"

// Platform specific defines and constants. 
// Change here for any platform differences and recompile.
#define DEFAULT_BAUD B115200 // The default 115200 baud rate to use.
// Unix baud rate options:
// B0,  B50,  B75,  B110,  B134,  B150,  B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800

// Application constants:
const std::string DEFAULT_TTY = "/dev/ttyUSB0"; // Default serial device to use. Can also be /dev/ttyS0 (COM1) or /dev/ttyS1 (COM2).
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

const uint8_t VTIME_FAST = 1; // Set VTIME_APPLIED to this if baud rate is fast enough for read to not need to wait as long.
const uint8_t VTIME_SLOW = 10; // Set VTIME_APPLIED to this if dealing with a slow serial speed. 
const uint8_t VTIME_APPLIED = VTIME_FAST;

const uint8_t BYTES_PER_LINE = 16; // FDUMP_CMD usually returns 16 bytes of data at once. Likely may break if changed.
const char EXT_CTRL_C = '\x03'; // Ctrl-c is etx so send ASCII code 0x03 \x03.

enum FlowControl
{
	FC_NONE = 0,
	FC_XON_XOFF = 1,
	FC_RTS_CTS = 2
};

// Application global variables:
// Singletons are bad!
bool parity = false;
uint stop_bits = 1; 	// Use only one stop bit.
uint data_bits = 8; 	// How many bits per byte.
FlowControl flow_control = FC_NONE;
bool not_modem = true;
bool canonical_mode = false; // If true, input is processed when new line is recieved.
bool echo = false; // If true, sent characters are echoed back.
bool signal_characters = false;

int serial_port = -1;
bool tty_opened;
struct termios tty;
bool continue_cfe = true;

std::regex re_seqid(REGEX_SEQ_ID); // For tty data line parser.
std::regex re_data(REGEX_HEX_DATA);

std::ofstream _of_flash; // The output file stream.
std::string* of_name = nullptr; // The output file target.
bool output_to_file = true;

bool verbose = false;
bool very_verbose = false;
bool print_data = false; 

uint offset;
std::string* device_name = nullptr;
std::string* tty_interface = nullptr;
uint block_size;
uint size_in_bytes;
uint blocks_to_copy;

