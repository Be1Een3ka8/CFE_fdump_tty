// uart_win.cpp: Minimal C++ Uart library. Author Gerallt Franke.
// Date: 24 April 2020 10:24 UTC. 

#ifdef WIN32
	#include "uart.h"

	void uart_init(uart_dev** dev)
	{
		*dev = (uart_dev*)malloc(sizeof(uart_dev));
	}

	void uart_set_baud(uart_dev* dev, uint32_t baud_rate)
	{
		dev->baud = (DWORD)baud_rate;
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
		dev->overlapped_io = OVERLAPPED_IO;

		std::string device_path = std::string("\\\\.\\");
		device_path = device_path.append(port_name);

		std::wstring* w_s_temp = new std::wstring(device_path.begin(), device_path.end());
		LPCWSTR w_dev_path = w_s_temp->c_str();

		// If NULL, no security: the file or device associated with the returned handle is assigned a default security descriptor.
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;

		DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;  //Read/Write
		DWORD dwShareMode = 0;  // No Sharing. Exclusive access. // Whatever you set other than 0 the serial driver ignores.
		DWORD dwCreationDisposition = OPEN_EXISTING; // Open existing port only. Opens a file or device, only if it exists.	
		DWORD dwFlagsAndAttributes;

		if (dev->overlapped_io)
		{
			// The file or device is being opened or created for asynchronous I/O.
			dwFlagsAndAttributes = FILE_FLAG_OVERLAPPED;
		}
		else
		{
			// Non Overlapped I/O. No threads. Just synchronous I/O.
			dwFlagsAndAttributes = 0;
		}

		HANDLE hTemplateFile = NULL; // Null for Comm Devices. No templates.

		dev->win_handle = CreateFile(w_dev_path, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

		dev->com_opened = (dev->win_handle != INVALID_HANDLE_VALUE);

		// Free allocated memory.
		delete w_s_temp;

		// # Check for errors.
		if (dev->com_opened)
		{
			if (dev->verbose)
			{
				std::cout << std::string("Opened ") << port_name << "	[ok]" << std::endl;
			}

			return true;
		}
		else
		{
			std::cout << "Opening " << port_name << "			[failed]	Error " << win32_get_error_msg(GetLastError()) << " (" << GetLastError() << ")" << " from open: " << strerror(errno) << std::endl << std::endl;
			std::cout << "Try changing the tty by the -tty= switch, or you might have a permissions issue." << std::endl;
			std::cout << " e.g 1:    ./fdump -tty=COM1 <options>" << std::endl;
			std::cout << " e.g 2:    ./fdump -tty=COM2 <options>" << std::endl;
			std::cout << " e.g 3:    ./fdump -tty=COM10 <options>" << std::endl;
			std::cout << " e.g 4:    ./fdump -tty=COM256 <options>" << std::endl;
			//std::cout << " default tty is set to: " << DEFAULT_TTY << std::endl << std::endl;
			std::cout << " !**You might also need to change the default settings: 115200 baud 8/N/1 and recompile." << std::endl;

			return false;
		}
	}

	bool uart_config(uart_dev* dev)
	{
		DCB conf; // tty configuration.
		COMMTIMEOUTS timeout; // Timeout configuration.

		// Apply configuration of serial communication port to current configuration.
		if (GetCommState(dev->win_handle, &conf) != false)
		{
			// Default configuration.
			conf.fBinary = true;
			conf.fDsrSensitivity = false;
			conf.fTXContinueOnXoff = false;
			conf.fErrorChar = false;
			conf.fNull = false;
			conf.fAbortOnError = false;

			// New applied configuration.
			
			// The parity options: 0-4, NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, SPACEPARITY
			if (dev->parity)
			{
				if (dev->verbose)
				{
					std::cout << "Parity			[enabled]" << std::endl;

					if(dev->parity_mode == PM_EVEN)
					{
						conf.Parity = EVENPARITY;
					}
					else if(dev->parity_mode == PM_ODD)
					{
						conf.Parity = ODDPARITY;
					}
				}
			}
			else
			{
				if (dev->verbose)
				{
					std::cout << "Parity 			[disabled]" << std::endl;

					conf.Parity = NOPARITY;
				}
			}	

			// Stop bits: 1, 1.5, 2, ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
			if(dev->stop_bits == 1)
			{
				conf.StopBits = ONESTOPBIT;
			}
			else if(dev->stop_bits == 2)
			{
				conf.StopBits = TWOSTOPBITS;
			}
			else
			{
				std::cout << "Stop bits		[unknown value]" << std::endl;
			}

			if (dev->verbose)
			{
				std::cout << "Data bits 		[" << dev->data_bits << "]" << std::endl;
			}

			conf.ByteSize = dev->data_bits; // Size of byte in bits for data communicated. Default 8.

			// Set the flow control.
			switch (dev->flow_control) 
			{
			case FC_NONE:
				if (dev->verbose)
				{
					std::cout << "Flow control		[None]" << std::endl;
				}
				conf.fInX = false;
				conf.fOutX = false;
				conf.fOutxCtsFlow = false;
				conf.fRtsControl = RTS_CONTROL_ENABLE;
				conf.fOutxDsrFlow = false;
				conf.fDtrControl = DTR_CONTROL_ENABLE;
				break;
			case FC_RTS_CTS:
				if (dev->verbose)
				{
					std::cout << "Flow control		[RTS/CTS]" << std::endl;
				}
				conf.fOutxCtsFlow = true;
				conf.fRtsControl = RTS_CONTROL_HANDSHAKE;
				break;
			case FC_XON_XOFF:
				if (dev->verbose)
				{
					std::cout << "Flow control		[XON/XOFF]" << std::endl;
				}
				conf.fInX = true;
				conf.fOutX = true;
				break;
			case FC_DSR_DTR:
				if (dev->verbose)
				{
					std::cout << "Flow control		[DSR/DTR]" << std::endl;
				}
				conf.fOutxDsrFlow = true;
				conf.fDtrControl = DTR_CONTROL_HANDSHAKE;			
				break;
			}

			if (dev->verbose)
			{
				std::cout << "Baud rate		[" << dev->baud << "]" << std::endl;	
			}

			// Set serial line speed. The in/out baud rate.
			conf.BaudRate = dev->baud;

			if (SetCommState(dev->win_handle, &conf) != false)
			{
				std::cout << "Set new settings on serial port." << std::endl;

				// Timeout configuration.
				timeout.ReadIntervalTimeout = 1; // The specified timeout between each byte recieved.
				timeout.ReadTotalTimeoutMultiplier = 1; // Value that is multiplied by the number of bytes to read.
				timeout.ReadTotalTimeoutConstant = 1; // Value that is added to the ReadTotalTimeoutMultiplier multiplier.
				timeout.WriteTotalTimeoutMultiplier = 1; // Value that is multiplied by the number of bytes to be sent.
				timeout.WriteTotalTimeoutConstant = 1; // Value that is added to the WriteTotalTimeoutMultiplier multiplier.

				// Save the timeout configuration in the device.
				if (SetCommTimeouts(dev->win_handle, &timeout) != false)
				{
					if (dev->verbose)
					{
						std::cout << "Settings saved." << std::endl << std::endl;
					}
				}
				else
				{
					std::cout << "Settings not saved. 	Unable to save timeout configuration." << std::endl;

					return false;
				}
			}
			else
			{
				std::cout << "Settings not saved. 	Error unable to configure serial port with new settings." << std::endl;

				return false;
			}
		}
		else
		{
			std::cout << "Settings not saved. 	Error there was a problem getting the current configuration." << std::endl;

			return false;
		}

		return true;
	}

	unsigned long uart_write(uart_dev* dev, void* data, unsigned long bytes_to_write)
	{
		DWORD num_bytes = 0;

		// Overlapped IO, or threading: If dwFlagsAndAttributes is set to FILE_FLAG_OVERLAPPED 
		// need to specify OVERLAPPED structure with 0 offsets applied.
		OVERLAPPED* overlapped = NULL;
		if (dev->overlapped_io)
		{
			overlapped = (OVERLAPPED*)malloc(sizeof(OVERLAPPED));
			ZeroMemory(overlapped, sizeof(OVERLAPPED));

			overlapped->Offset = 0;
			overlapped->OffsetHigh = 0;

			//TODO: BUG: overlapped io not fully implemented 
		}

		BOOL result = WriteFile(dev->win_handle, data, bytes_to_write, &num_bytes, overlapped);

		if (dev->overlapped_io)
		{
			free(overlapped);
		}

		if (result != false)
		{
	#ifdef UART_TRACING
			std::cout << "bytes written " << num_bytes << std::endl;
	#endif
			return num_bytes;
		}

		std::cout << "Writing to serial has a problem." << std::endl;

		std::cout << "Msg: " << win32_get_error_msg(GetLastError()) << " (" << GetLastError() << ")" << std::endl;

		return num_bytes;
	}

	unsigned long uart_read(uart_dev* dev, void** data, unsigned long bytes_to_read)
	{
		DWORD num_bytes = 0;

		// Overlapped IO, or threading: If dwFlagsAndAttributes is set to FILE_FLAG_OVERLAPPED 
		// need to specify OVERLAPPED structure with 0 offsets applied.
		OVERLAPPED* overlapped = NULL;
		if (dev->overlapped_io)
		{
			overlapped = (OVERLAPPED*)malloc(sizeof(OVERLAPPED));
			ZeroMemory(overlapped, sizeof(OVERLAPPED));

			overlapped->Offset = 0;
			overlapped->OffsetHigh = 0;

			//TODO: BUG: overlapped io not fully implemented 
		}

		BOOL result = ReadFile(dev->win_handle, *data, bytes_to_read, &num_bytes, overlapped);

		if (dev->overlapped_io)
		{
			free(overlapped);
		}

		if (result != false)
		{
	#ifdef UART_TRACING
			std::cout << "bytes read " << num_bytes << std::endl;
	#endif
			return num_bytes;
		}

		std::cout << "Reading from serial has problem." << std::endl;

		std::cout << "Msg: " << win32_get_error_msg(GetLastError()) << " (" << GetLastError() << ")" << std::endl;

		// Nothing to read if num_bytes == 0.
		num_bytes = 0;
		return num_bytes;
	}

	void uart_close(uart_dev* dev)
	{
		if(dev->com_opened)
		{
			if (dev->verbose)
			{
				std::cout << "Closing handle to " << dev->port_name;
			}

			// Close the Serial Port.
			CloseHandle(dev->win_handle);

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

	std::string win32_get_error_msg(DWORD last_error)
	{
		// More codes here:
		// https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
		switch (last_error)
		{
		case ERROR_SUCCESS:
			return "success";
		case ERROR_INVALID_FUNCTION:
			return "invalid function";
		case ERROR_FILE_NOT_FOUND:
			return "file not found";
		case ERROR_PATH_NOT_FOUND:
			return "path not found";
		case ERROR_TOO_MANY_OPEN_FILES:
			return "too many open files";
		case ERROR_ACCESS_DENIED:
			return "access denied";
		case ERROR_INVALID_HANDLE:
			return "invalid handle";
		case ERROR_ARENA_TRASHED:
			return "arena trashed";
		case ERROR_NOT_ENOUGH_MEMORY:
			return "not enough memory";
		case ERROR_INVALID_BLOCK:
			return "invalid block";
		case ERROR_BAD_ENVIRONMENT:
			return "bad environment";
			// ...
		case ERROR_INVALID_PARAMETER:
			return "invalid parameter";
		}
		return "unknown error";
	}
#endif
