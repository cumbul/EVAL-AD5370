/*
 *
 */

#ifndef DAC_H
#define DAC_H

#include "../Utilities/Singleton.h"

#include <iostream>
#include <Windows.h>
#include <vector>
#include <sstream>

// Vendor Request Commands
#define VR_SPI_WRITE 0xDD
#define VR_SPI_READ 0xDD
#define VR_SETRESET 0xDA
#define VR_CLRRESET 0xDB
#define VR_SETCLR 0xDC
#define VR_PULSE_LDAC 0xDE
#define VR_CLRCLR 0xDF
#define VR_SETLDAC 0xE2
#define VR_CLRLDAC 0xE3
#define VR_DIR_READ 1
#define VR_DIR_WRITE 0
#define VR_ZERO_DATA_LENGTH 0
#define VR_ZERO 0
#define VR_INIT 0xE0
#define VR_PORTOUT 0xE1

// Application Specific Constants 
#define MAX_BOARDS 100
#define DLL_PATH TEXT("C:\\Program Files\\Analog Devices\\USB Drivers\\ADI_CYUSB_USB4.dll")


namespace AD537x
{
	//Structure to hold information of path and handles of multiple devices
	struct DeviceHandles {
		unsigned char path;
		int handle = -1;
	};

	class DAC : public Singleton<DAC>
	{
		template <typename DAC>
		friend class Singleton;

		//DLL function prototypes
		typedef int (CALLBACK* SearchFunction) (int, int, int*, unsigned char*);
		typedef int (CALLBACK* ConnectFunction) (int, int, unsigned char, int*);
		typedef int (CALLBACK* DownloadFWFunction) (int, char[]);
		typedef int (CALLBACK* VendorRequestFunction) (int, unsigned char, unsigned short, unsigned short, unsigned char, unsigned short, unsigned char*);
		typedef int (CALLBACK* DisconnectFunction) (unsigned int);


	private:
		//Vendor ID and Product ID for AD5370 (evaluation board)
		const int _VENDOR_ID = 1110;						//HEX VALUE - 0x0456 
		const int _PRODUCT_ID = 45583;						//HEX VALUE - 0xB208 

		//@DLR
		//Fully Qualified Path to Firmware ending with a \0. This cannot be a #define because, the function requires char * not char_t?? 
		char FW_PATH[100] = "C:\\code\\AD537x\\Binaries\\AD537xSPI.hex\0";
		
		unsigned char _emptyBuffer;							//Empty Buffer to send while calling write Vendor Requests

		int _numBoards = 0;									//Number of Boards attached to the system via USB
		
		const float _vMax = +6.f;							//Max Voltage of the board based on Jumper Setting
		const float _vMin = -6.f;							//Min Voltage of the board based on Jumper Setting
		const float _vRange = _vMax - _vMin;				//Voltage Range of the board, used for computing voltage values to be sent via SPI
		const float _vOffset = _vRange / 2.f;				//Offset Voltage of the board, used for computing voltage values to be sentvia SPI

		HMODULE hinstDLL;									//HModule to load and store DLL symbols

		//Instances of the typedef of the DLL function prototypes
		SearchFunction Search_For_Boards;
		ConnectFunction Connect;
		DownloadFWFunction Download_Firmware;
		VendorRequestFunction Vendor_Request;
		DisconnectFunction Disconnect;

		int initialize_vendor_request(int device_index);
		int download_firmware(int device_index);
		int search_for_boards();
		int connect_board(int device_index);


	public:
		std::vector<DeviceHandles> devices;
		DAC();
		~DAC();

		int write_spi_word(int device_index, std::string word);		
		int write_voltage(int device_index, int channel, float voltage);
		int pulse_ldac(int device_index);
		int find_and_initialize_all_boards();
	};
}  // namespace AD537x
#endif  // DAC_H
