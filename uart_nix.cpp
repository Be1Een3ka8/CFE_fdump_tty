// uart_nix.cpp: Minimal C++ Uart library. Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

#ifdef POSIX
	#include <cstring>
	#include "uart.h"

	void uart_init(uart_dev** dev)
	{
		*dev = (uart_dev*)malloc(sizeof(uart_dev));
	}

	void uart_set_baud(uart_dev* dev, uint32_t baud_rate)
	{
		dev->baud = baud_rate;
	}

	void uart_set_flowctrl(uart_dev* dev, int32_t flow_control)
	{
		dev->flow_control = flow_control;
	}

	void uart_set_parity(uart_dev* dev, bool parity, int32_t parity_mode)
	{
		dev->parity = parity;
		dev->parity_mode = parity_mode;
	}

	void uart_set_stopbits(uart_dev* dev, uint32_t stop_bits)
	{
		dev->stop_bits = stop_bits;
	}

	void uart_set_databits(uart_dev* dev, uint32_t data_bits)
	{
		dev->data_bits = data_bits;
	}

	void uart_set_verbosity(uart_dev* dev, bool verbosity)
	{
		dev->verbose = verbosity;
	}

	bool uart_open(uart_dev* dev, std::string port_name)
	{
		dev->port_name = port_name;
		dev->serial_port = open(port_name.c_str(), O_RDWR);

		dev->tty_opened = (dev->serial_port >= 0);

		// # Check for errors.
		if (dev->tty_opened)
		{
			if (dev->verbose)
			{
				std::cout << std::string("Opened ") << port_name << "	[ok]" << std::endl;
			}

			//serial_lock();

			return true;
		}
		else
		{
			std::cout << "Opening " << port_name << "			[failed]	Error " << std::to_string(errno) << " from open: " << strerror(errno) << std::endl << std::endl;
			std::cout << "Try changing the tty by the -tty= switch, or you might need to add your user to dialout group." << std::endl;
			std::cout << " e.g 1: (for COM1) ./fdump -tty=/dev/ttyS0 <options>" << std::endl;
			std::cout << " e.g 2: (for COM2) ./fdump -tty=/dev/ttyS1 <options>" << std::endl;
			std::cout << " e.g 3:            ./fdump -tty=/dev/ttyUSB0 <options>" << std::endl;
			std::cout << " e.g 4: (BSD)      ./fdump -tty=/dev/ttyU0 <options>" << std::endl;
			//std::cout << " default tty is set to: " << DEFAULT_TTY << std::endl << std::endl;
			std::cout << " !**You might also need to change the default settings: 115200 baud 8/N/1 and recompile." << std::endl;
		}
		return false;
	}

	bool uart_config(uart_dev* dev)
	{
		memset(&dev->tty, 0, sizeof(struct termios));

		if (tcgetattr(dev->serial_port, &dev->tty) != 0)
		{
			std::cout << "Error " << std::to_string(errno) << " from tcgetattr: " << strerror(errno) << std::endl;
		}

		if (dev->parity)
		{
			dev->tty.c_cflag |= PARENB;  // Set parity bit, enabling parity.

			if (dev->verbose)
			{
				std::cout << "Parity			[enabled]";
			}

			if(dev->parity_mode == PM_EVEN)
			{
				dev->tty.c_cflag &= ~PARODD;  // Clear odd parity bit, enabling even parity.

				std::cout << " [even]";
			}
			else if(dev->parity_mode == PM_ODD)
			{
				dev->tty.c_cflag |= PARODD;  // Set even parity bit, enabling even parity.

				std::cout << " [odd]";
			}

			std::cout << std::endl;
		}
		else
		{
			dev->tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity. (most common)

			if (dev->verbose)
			{
				std::cout << "Parity 			[disabled]" << std::endl;
			}
		}

		if (dev->stop_bits == 1)
		{
			dev->tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication. (most common)

			if (dev->verbose)
			{
				std::cout << "Stop bits		[1]" << std::endl;
			}
		}
		else if (dev->stop_bits == 2)
		{
			dev->tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication.

			if (dev->verbose)
			{
				std::cout << "Stop bits		[2]" << std::endl;
			}
		}
		else
		{
			std::cout << "Stop bits		[unknown value]" << std::endl;
		}

		if (dev->verbose)
		{
			std::cout << "Data bits 		[" << dev->data_bits << "]" << std::endl;
		}

		switch (dev->data_bits)
		{
		case 5:
			dev->tty.c_cflag |= CS5; // 5 bits per byte.
			break;
		case 6:
			dev->tty.c_cflag |= CS6; // 6 bits per byte.
			break;
		case 7:
			dev->tty.c_cflag |= CS7; // 7 bits per byte.
			break;
		case 8:
		default:
			dev->tty.c_cflag |= CS8; // 8 bits per byte. (most common)
		}

		switch (dev->flow_control)
		{
		case FC_NONE:
			if (dev->verbose)
			{
				std::cout << "Flow control		[None]" << std::endl;
			}

			// Disable RTS/CTS hardware flow control (most common)
			dev->tty.c_cflag &= ~CRTSCTS;

			// Disable XON/XOFF software flow control on input which we don't want.
			dev->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
			break;
		case FC_RTS_CTS:
			if (dev->verbose)
			{
				std::cout << "Flow control		[RTS/CTS]" << std::endl;
			}

			// Enable RTS/CTS hardware flow control
			dev->tty.c_cflag |= CRTSCTS;

			// Disable XON/XOFF software flow control on input which we don't want.
			dev->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
			break;
		case FC_XON_XOFF:
			if (dev->verbose)
			{
				std::cout << "Flow control		[XON/XOFF]" << std::endl;
			}

			// Enable XON/XOFF flow control on input.
			dev->tty.c_cflag |= IXANY;
			break;
		case FC_DSR_DTR:
			if (dev->verbose)
			{
				std::cout << "Flow control		[DSR/DTR] is unsupported, defaulting to [None]" << std::endl;
			}

			// // Enable DSR/DTR hardware flow control
			// dev->tty.c_cflag |= CDTRDSR; // Support for this flag 'CDTRDSR' varies.

			// // Disable RTS/CTS hardware flow control.
			// dev->tty.c_cflag &= ~CRTSCTS;

			// // Disable XON/XOFF software flow control on input which we don't want.
			// dev->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

			if (dev->verbose)
			{
				std::cout << "Flow control		[None]" << std::endl;
			}

			// Disable RTS/CTS hardware flow control (most common)
			dev->tty.c_cflag &= ~CRTSCTS;

			// Disable XON/XOFF software flow control on input which we don't want.
			dev->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
			break;
		default:
			std::cout << "Flow control		[unknown value]" << std::endl;
		}

		// Turn on READ & ignore ctrl lines (CLOCAL = 1).
		// CLOCAL ignores modem carrier detect and other signal lines 
		// because this is not a modem otherwise this process would recieve a SIGHUP when modem disconnected.
		dev->tty.c_cflag |= CREAD | CLOCAL;

		//dev->tty.c_cflag |= CREAD;
		//if(not_modem)
		//{
		//	dev->tty.c_cflag |= CLOCAL;
		//}

		if (!canonical_mode)
		{
			// Disable canonical mode because this is a serial port.
			dev->tty.c_lflag &= ~ICANON;
		}

		if (!echo)
		{
			dev->tty.c_lflag &= ~ECHO; 	// Disable echo
			dev->tty.c_lflag &= ~ECHOE; 	// Disable erasure
			dev->tty.c_lflag &= ~ECHONL; // Disable new-line echo
		}

		if (!signal_characters)
		{
			// Disable interpretation of INTR, QUIT and SUSP signal characters.
			dev->tty.c_lflag &= ~ISIG;
		}

		// No special handling of the data on receive of bytes, we need raw data.
		dev->tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

		dev->tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
		dev->tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

		// Wait for up to 1s (VTIME=10 deciseconds), returning as soon as any data is received.
		dev->tty.c_cc[VTIME] = VTIME_APPLIED;
		dev->tty.c_cc[VMIN] = 0;

		if (dev->verbose)
		{
			std::cout << "Baud rate		[" << dev->baud << "]" << std::endl;	
		}
		
		// Set line speed.
		// Set in/out baud rate (defines are in termbits.h)
		switch(dev->baud)
		{
			case 0:
				cfsetispeed(&dev->tty, B0);
				cfsetospeed(&dev->tty, B0);
			break;
			case 50:
				cfsetispeed(&dev->tty, B50);
				cfsetospeed(&dev->tty, B50);
			break;
			case 75:
				cfsetispeed(&dev->tty, B75);
				cfsetospeed(&dev->tty, B75);
			break;
			case 110:
				cfsetispeed(&dev->tty, B110);
				cfsetospeed(&dev->tty, B110);
			break;
			case 134:
				cfsetispeed(&dev->tty, B134);
				cfsetospeed(&dev->tty, B134);
			break;
			case 150:
				cfsetispeed(&dev->tty, B150);
				cfsetospeed(&dev->tty, B150);
			break;
			case 200:
				cfsetispeed(&dev->tty, B200);
				cfsetospeed(&dev->tty, B200);
			break;
			case 300:
				cfsetispeed(&dev->tty, B300);
				cfsetospeed(&dev->tty, B300);
			break;
			case 600:
				cfsetispeed(&dev->tty, B600);
				cfsetospeed(&dev->tty, B600);
			break;
			case 1200:
				cfsetispeed(&dev->tty, B1200);
				cfsetospeed(&dev->tty, B1200);
			break;
			case 1800:
				cfsetispeed(&dev->tty, B1800);
				cfsetospeed(&dev->tty, B1800);
			break;
			case 2400:
				cfsetispeed(&dev->tty, B2400);
				cfsetospeed(&dev->tty, B2400);
			break;
			case 4800:
				cfsetispeed(&dev->tty, B4800);
				cfsetospeed(&dev->tty, B4800);
			break;
			case 9600:
				cfsetispeed(&dev->tty, B9600);
				cfsetospeed(&dev->tty, B9600);
			break;
			case 19200:
				cfsetispeed(&dev->tty, B19200);
				cfsetospeed(&dev->tty, B19200);
			break;
			case 38400:
				cfsetispeed(&dev->tty, B38400);
				cfsetospeed(&dev->tty, B38400);
			break;
			case 57600:
				cfsetispeed(&dev->tty, B57600);
				cfsetospeed(&dev->tty, B57600);
			break;
			case 115200:
				cfsetispeed(&dev->tty, B115200);
				cfsetospeed(&dev->tty, B115200);
			break;
			case 230400:
				cfsetispeed(&dev->tty, B230400);
				cfsetospeed(&dev->tty, B230400);
			break;
			default:
				cfsetispeed(&dev->tty, B115200);
				cfsetospeed(&dev->tty, B115200);
		}

		// Unix options:
		// B0,  B50,  B75,  B110,  B134,  B150,  B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800

		// Save tty settings, also checking for error
		if (tcsetattr(dev->serial_port, TCSANOW, &dev->tty) == 0)
		{
			if (dev->verbose)
			{
				std::cout << "Settings saved." << std::endl << std::endl;
			}

			return true;
		}
		else
		{
			std::cout << "Settings not saved. 	Error " << std::to_string(errno) << " from tcsetattr: " << strerror(errno) << std::endl;

			return false;
		}
	}

	unsigned long uart_write(uart_dev* dev, void* data, unsigned long bytes_to_write)
	{
		unsigned long num_bytes = (unsigned long)write(dev->serial_port, data, bytes_to_write);

	#ifdef UART_TRACING
		std::cout << "bytes written " << num_bytes << std::endl;
	#endif

		return num_bytes;
	}

	unsigned long uart_read(uart_dev* dev, void** data, unsigned long bytes_to_read)
	{
		void* read_buffer = *data;

		unsigned long num_bytes = read(dev->serial_port, read_buffer, bytes_to_read);

		if (num_bytes > 0)
		{
	#ifdef UART_TRACING
			std::cout << "bytes read " << num_bytes << std::endl;
	#endif

			// Output the data.
			*data = &read_buffer;
		}

		// Nothing to read if num_bytes == 0.
		return num_bytes;
	}

	void uart_close(uart_dev* dev)
	{
		if (dev->tty_opened)
		{
			if (dev->verbose)
			{
				std::cout << "Closing handle to " << dev->port_name;
			}

			close(dev->serial_port);

			if (dev->verbose)
			{
				std::cout << "	[done]" << std::endl;
			}
		}
	}

	void uart_free(uart_dev* dev)
	{
		// Free allocated memory;
		if (dev != nullptr)
		{
			free(dev);
		}
	}

	// Uncomplete tty lock code.
	// void serial_lock()
	// {
	// 	int fd = serial_port;

	// 	//FILE *fp=fopen((*tty_interface).c_str(),"r+");
	// 	//int fd = fileno(fp);

	// 	//printf("fileno(stdin) = %d\n", fileno(fp));

	// 	//char fd = fp->fd;
	//     // Acquire non-blocking exclusive lock
	//     if(flock(fd, LOCK_EX | LOCK_NB) == -1) 
	//     {
	//         throw std::runtime_error("Serial port with file descriptor " + 
	//             std::to_string(fd) + " is already locked by another process.");
	//     }

	//     //fclose(fp);
	// }
#endif
