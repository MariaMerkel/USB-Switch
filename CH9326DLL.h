//
// ****************************************
// ** Copyright (C) W.ch 1999-2011 **
// ** Web: http://www.winchiphead.com **
// ****************************************
// ** DLL for HID interface chip CH9326 **
// ** C, VC6.0 **
// ****************************************
//
// HID application layer interface library V1.1
// Nanjing Qinheng Electronics Co., Ltd. Author: zyw@wch.cn 2014.08
// HIDDLL V1.1
//
//
//

#include <windows.h>

#ifndef _CH9326_DLL_H
#define  _CH9326_DLL_H

#ifdef __cplusplus
extern  "C" {
#endif

/* BAUD rate setting */
#define  B300  		0x01
#define  B600  		0x02
#define  B1200  	0x03
#define  B2400  	0x04
#define  B4800  	0x05
#define  B9600  	0x06
#define  B14400  	0x07
#define  B19200  	0x08
#define  B28800  	0x09
#define  B38400  	0x0A
#define  B57600  	0x0B
#define  B76800  	0x0C
#define  B115200  	0x0D

/* Parity define */
#define  P_ODD  	0x01  // Odd check
#define  P_EVEN  	0x02  // Even parity
#define  P_SPC  	0x03  // blank
#define  P_NONE  	0x04  // No verification

/* Data bits define */
#define  BIT_5  	0x01
#define  BIT_6  	0x02
#define  BIT_7  	0x03
#define  BIT_8  	0x04

/* Stop bits define */
#define  STOP_1  	0x01
#define  STOP_2  	0x02

// Initialize the DLL library.
BOOL WINAPI CH9326DllInt ();

// CH9326GetHidGuid gets the GUID of HID, please refer to CH9326DBG for usage
// Equivalent to HidD_GetHidGuid, refer to MSDN
void WINAPI CH9326GetHidGuid (void *HidGuid);

// Open the device according to the VID/PID of the device
// CH9326OpenDevice returns the device handle successfully, and returns INVALID_HANDLE_VALUE if it fails
HANDLE WINAPI CH9326OpenDevice (unsigned short USB_VID, 						// VID of the device
                                unsigned short USB_PID 							// PID of the device
);

// Open the device according to the device link name
// CH9326OpenDevice returns the device handle successfully, and returns INVALID_HANDLE_VALUE if it fails
HANDLE WINAPI CH9326OpenDevicePath (PCHAR DevicePath 							// Device link name
);

BOOL WINAPI CH9326GetDevicePath (ULONG DevIndex, 								// Device index number, indicating the number of HID devices in the system
                                 PCHAR DevicePath, 								// The function returns the device link name
                                 ULONG DevicePathLen 							// DevicePath buffer length
);
// Equivalent to HidD_GetAttributes to get VID, PID, version number, refer to MSDN. Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326GetAttributes (HANDLE hDeviceHandle, 				// Device handle
                                          unsigned short *pVID, 				// returns the VID of the device
                                          unsigned short *pPID, 				// return the PID of the device
                                          unsigned short *pVer 				// Returns the version number of the device
);

// Get the maximum data length of CH9326ReadData and CH9326WriteData for one read and write
// Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326GetBufferLen (HANDLE hDeviceHandle, 					// Device handle
                                         unsigned short *InputReportLen,		// returns the length of the received data
                                         unsigned short *OutputReportLen 		// Returns the length of the sent data
);

// Set the CH9326ReadData and CH9326WriteData read and write timeout interval, in milliseconds
void WINAPI CH9326SetTimeOut (HANDLE hDeviceHandle, 							// Device handle
                              unsigned long ReadTimeOut, 						// Set the timeout interval for reading data
                              unsigned long SendTimeOut 						// Set the timeout interval for writing data
);

// Read a packet of data, the maximum length is InputReportLen-2 returned by CH9326GetBufferLen, which is determined by the chip firmware.
// CH9326 reads a packet of data with a maximum length of 31. It returns TRUE if it succeeds, and FALSE if it fails
unsigned long WINAPI CH9326ReadData (HANDLE hDeviceHandle, 						// Device handle
                                     void *ReadBuffer, 						// Point to the receiving data buffer
                                     unsigned long *pReadLen, 					// Returns the actual length of data read
                                     HANDLE hEventObject 						// Waiting for event handle
);

// Write a packet of data, the maximum length is OutputReportLen-2 returned by CH9326GetBufferLen, which is determined by the chip firmware.
// CH9326 writes a packet of data with a maximum length of 31. It returns TRUE if it succeeds, and FALSE if it fails
unsigned long WINAPI CH9326WriteData (HANDLE hDeviceHandle, 					// Device handle
                                      void *SendBuffer, 						// Point to send data buffer
                                      unsigned long SentLen, 					// The length of the data to be sent
                                      HANDLE hEventObject 						// Waiting for event handle
);
/*
ucRate: 1=300 (corresponding to a baud rate of 300 when ucRate is 1), 2=600,3=1200,4=2400,5=4800,6=9600(default),7=14400,8=19200,9=28800,10=38400,11=57600,12=76800,13=115200
ucCheck: 1=odd check, 2=even check, 3=blank bit, 4=no check (default value)
ucStop: 1=1 bit (default value), 2=2 bit
ucData: 1=5 bits, 2=6 bits, 3=7 bits, 4=8 bits (default value)
*/
// Set the baud rate, ucInterval is the timeout interval for receiving data, if the received data is less than 31,
// And if the data is not received after the time interval of ucInterval, CH9326 will upload the packaged data to the PC
// Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326SetRate (HANDLE hDeviceHandle, 					// Device handle
                                    unsigned char ucRate, 					// Baud rate
                                    unsigned char ucCheck, 				// Check mode
                                    unsigned char ucStop, 					// Stop bit
                                    unsigned char ucData, 					// Data bit
                                    unsigned char ucInterval 				// Time interval when HID is accepted, 0x10=3MS ((default value)), 0x20 is approximately equal to 6MS, and 0x30 is approximately equal to 9MS
);

/*
CH9326InitThreadData, CH9326GetThreadDataLen, CH9326ClearThreadData,
CH9326ReadThreadData, CH9326StopThread
The above group of functions are mainly for the convenience of reading data operations. It is recommended that customers use this method to read data.
CH9326InitThreadData: Create a thread and private heap, call CH9326ReadData in the thread to read data, and the read data is buffered in the internal private heap.
CH9326GetThreadDataLen: Get the length of the data in the internal buffer.
CH9326ReadThreadData: Read the data in the internal buffer.
CH9326ClearThreadData: Clear all data in the buffer.
CH9326StopThread: Stop the internal thread reading and clear the internal buffer.
*/

// Initialize thread and private heap, return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326InitThreadData (HANDLE hDeviceHandle 			// Device handle
);

// Query data and return how much data is in the thread buffer
unsigned long WINAPI CH9326GetThreadDataLen (HANDLE hDeviceHandle 			// Device handle
);
// Read the data in the thread, return TRUE if successful, and return FALSE if it fails
// Note: The ReadLen parameter is the length of the data to be read before the call. The length cannot exceed the size of the ReadBuffer buffer, otherwise a memory access overflow will be sent
unsigned long WINAPI CH9326ReadThreadData (HANDLE hDeviceHandle, 			// HID device handle
                                           void *ReadBuffer, 				//The first address of the buffer
                                           unsigned long *ReadLen 			// The length of the data to be read before the call, returns the length of the data actually read
);
// Empty the buffer data
void WINAPI CH9326ClearThreadData (HANDLE hDeviceHandle 					// Device handle
);

// Stop the internal thread to read, and clear the internal buffer
void WINAPI CH9326StopThread (HANDLE hDeviceHandle 							// Device handle
);

// Set the product descriptor information, please use it with caution, especially when setting a new VID/PID,
//When opening the device, the VID/PID corresponding to CH9326OpenDevice must be modified accordingly, otherwise the opening fails.
// Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326SetDeviceCfg (HANDLE hDeviceHandle, 				// Device handle
                                         unsigned short wVID, 				// Set a new VID
                                         unsigned short wPID, 				// Set a new PID
                                         unsigned short wPower, 			// Set new device current, 50~150
                                         char *strManufacturer, 			// Set a new manufacturer descriptor
                                         unsigned long uMSlen, 				// Set the length of the new vendor descriptor, the maximum is 29
                                         char *strProduct, 					// Set a new product descriptor
                                         unsigned long uPSlen, 				// Set the length of the new product descriptor, the maximum is 29
                                         char *strSerialNo, 				// Set new product serial number
                                         unsigned long uSSlen 				// Set the length of the new product serial number, the maximum is 29
);

// Set the default baud rate, the default baud rate after power-on, when it is not set, the default baud rate of CH9326 is 9600, the meaning of the parameters refer to CH9326SetRate
// Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326SetDefaultRate (HANDLE hDeviceHandle, 			// Device handle
                                           unsigned char ucRate, 			// Baud rate
                                           unsigned char ucCheck, 			// Check mode
                                           unsigned char ucStop, 			// Stop bit
                                           unsigned char ucData, 			// Data bit
                                           unsigned char ucInterval 		// Time interval when HID is accepted, 0x10=3MS ((default value)), 0x20 is approximately equal to 6MS, and 0x30 is approximately equal to 9MS
);

// Restore the factory default settings and invalidate the settings of CH9326SetDeviceCfg and CH9326SetDefaultRate.
// Return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326ResetToDefault (HANDLE hDeviceHandle 			// Device handle
);

// Set the direction of the 16-bit IO port, return TRUE on success, FALSE on failure
unsigned long WINAPI CH9326SetIODir (HANDLE hDeviceHandle, 					// Device handle
                                     unsigned short usDir 					// 16-bit IO port input and output settings, corresponding bits: 1 is output, 0 is input
);

// Set the level of the 16-bit IO port, return TRUE if successful, and FALSE if failed
unsigned long WINAPI CH9326WriteIOData (HANDLE hDeviceHandle, 				// Device handle
                                        unsigned short usData 				// 16-bit IO port output high and low level settings, corresponding bits: 1 is output high level, 0 is output ground level
);

// Get the pin level of the 16-bit IO port, return TRUE if it succeeds, and FALSE if it fails
unsigned long WINAPI CH9326ReadIOData (HANDLE hDeviceHandle, 				// Device handle
                                       unsigned short *pData 				// Get the pin level of the 16-bit IO port
);

// Get the manufacturer string descriptor, return TRUE on success, FALSE on failure
// Equivalent to HidD_GetManufacturerString, refer to MSDN
unsigned long WINAPI CH9326GetManufacturerString (HANDLE hDeviceHandle, 		// Device handle
                                                  void *Buffer, 				// The first address of the buffer
                                                  unsigned long BufferLength	// Buffer size
);

// Get the product string descriptor, return TRUE on success, FALSE on failure
// Equivalent to HidD_GetProductString, refer to MSDN
unsigned long WINAPI CH9326GetProductString (HANDLE hDeviceHandle, 			// Device handle
                                             void *Buffer, 					//The first address of the buffer
                                             unsigned long BufferLength 	// Buffer size
);

// Get the serial number string descriptor, return TRUE on success, FALSE on failure
// Equivalent to HidD_GetSerialNumberString, refer to MSDN
unsigned long WINAPI CH9326GetSerialNumberString (HANDLE hDeviceHandle, 		// Device handle
                                                  void *Buffer, 				// The first address of the buffer
                                                  unsigned  long BufferLength 	// Buffer size
);


// Turn off the device
void WINAPI CH9326CloseDevice (HANDLE hDeviceHandle 						// Device handle
);

#ifdef __cplusplus
}
#endif

#endif // _CH9326_DLL_H
