// uart_nix.h: Minimal C++ Uart library. Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

#ifndef NIX_UART_H
#define NIX_UART_H

#ifdef POSIX

#include <string>

// POSIX headers. GNU/Linux, Unix/BSD.
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

//#define UART_TRACING

struct uart_dev
{
	struct termios tty;
	int serial_port = -1;
	bool tty_opened;
	uint32_t baud;
	int32_t flow_control;
	bool parity;
    int32_t parity_mode;
	uint32_t stop_bits;
	uint32_t data_bits;
	std::string port_name;
    bool verbose;
};

const uint8_t VTIME_FAST = 1; // Set VTIME_APPLIED to this if baud rate is fast enough for read to not need to wait as long.
const uint8_t VTIME_SLOW = 10; // Set VTIME_APPLIED to this if dealing with a slow serial speed. 
const uint8_t VTIME_APPLIED = VTIME_FAST;

const bool not_modem = true;
const bool canonical_mode = false; // If true, input is processed when new line is recieved.
const bool echo = false; // If true, sent characters are echoed back.
const bool signal_characters = false;

#endif

#endif
