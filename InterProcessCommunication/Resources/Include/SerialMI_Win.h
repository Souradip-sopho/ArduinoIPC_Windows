#ifndef SerialMI_Win_H_
#define SerialMI_Win_H_

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <conio.h>
#include <tchar.h>
#include <io.h>

#include <Windows.h>

HANDLE hComm;                          // Handle to the Serial port		

char ComPortName[32]; // Name of the Serial port(May Change) to be opened,

__declspec(dllexport)  __stdcall void serialBegin(int *port, int *baudrate){
		
		BOOL   Status;
		
		/*----------------------------------- Setting the Serial Port Name & Baurate--------------------------------------------*/
		sprintf(ComPortName,"\\\\.\\COM%d",*port);
		int BDR = CBR_9600;
		switch(*baudrate)
		{
			case 110: {BDR = CBR_110;break;}
			case 300: {BDR = CBR_300;break;}
			case 600: {BDR = CBR_600;break;}
			case 1200: {BDR = CBR_1200;break;}
			case 2400: {BDR = CBR_2400;break;}
			case 4800: {BDR = CBR_4800;break;}
			case 9600: {BDR = CBR_9600;break;}
			case 14400: {BDR = CBR_14400;break;}
			case 19200: {BDR = CBR_19200;break;}
			case 38400: {BDR = CBR_38400;break;}
			case 56000: {BDR = CBR_56000;break;}
			case 57600: {BDR = CBR_57600;break;}
			case 115200: {BDR = CBR_115200;break;}
			case 128000: {BDR = CBR_128000;break;}
			case 256000: {BDR = CBR_256000;break;}
			default: break;
		}


		/*----------------------------------- Opening the Serial Port --------------------------------------------*/

		hComm = CreateFile( ComPortName,                       // Name of the Port to be Opened
							GENERIC_READ | GENERIC_WRITE,      // Read/Write Access
							0,                                 // No Sharing, ports cant be shared
							NULL,                              // No Security
							OPEN_EXISTING,                     // Open existing port only
							0,                                 // Non Overlapped I/O
							NULL);                             // Null for Comm Devices

		if (hComm == INVALID_HANDLE_VALUE){
			printf("Error! - Port %s can't be opened\n", ComPortName);
			exit(1);
		}
		else 
			printf("Port %s Opened\n", ComPortName);

		
		/*------------------------------- Setting the Parameters for the SerialPort ------------------------------*/

		DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		
		Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

		if (Status == FALSE){
			printf("Error! in GetCommState()\n");
			exit(1);
		}


		dcbSerialParams.BaudRate = BDR;    // Setting BaudRate = *baudrate
		dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
		dcbSerialParams.Parity   = NOPARITY;      // Setting Parity = None 

		Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

		if (Status == FALSE)
		{
			printf("Error! in Setting DCB Structure\n");
			exit(1);
		}
		
		/*------------------------------------ Setting Timeouts --------------------------------------------------*/
		
		COMMTIMEOUTS timeouts = { 0 };

		timeouts.ReadIntervalTimeout         = 50;
		timeouts.ReadTotalTimeoutConstant    = 50;
		timeouts.ReadTotalTimeoutMultiplier  = 10;
		timeouts.WriteTotalTimeoutConstant   = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (SetCommTimeouts(hComm, &timeouts) == FALSE){
			printf("Error! in Setting Time Outs\n");
			exit(1);
		}

}

__declspec(dllexport) __stdcall void serialWrite(char* write_buff){

	BOOL   Status;
	char   lpBuffer[32];		       // lpBuffer should be  char or byte array, otherwise write wil fail
	DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port
	strcpy(lpBuffer,write_buff);

	//dNoOFBytestoWrite = sizeof(lpBuffer); // Calculating the no of bytes to write into the port
	dNoOFBytestoWrite = strlen(write_buff)+1;

	Status = WriteFile(hComm,               // Handle to the Serialport
					   lpBuffer,            // Data to be written to the port 
					   dNoOFBytestoWrite,   // No of bytes to write into the port
					   &dNoOfBytesWritten,  // No of bytes written to the port
					   NULL);
	
	if (Status == FALSE){
		printf("Error %d in Writing to Serial Port\n",GetLastError());
		exit(1);
	}
		
}

__declspec(dllexport) __stdcall const char* serialRead(){

	static char read_buff[32]="";
	BOOL  Status;                          // Status of the various operations 
	DWORD dwEventMask;                     // Event mask to trigger
	char  TempChar;                        // Temperory Character
	char  SerialBuffer[32];                // Buffer Containing Rxed Data
	DWORD NoBytesRead;                     // Bytes read by ReadFile()
	int i = 0;
	Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception
	
	if (Status == FALSE){
		printf("Error! in Setting CommMask\n");
		exit(1);
	}

	
   /*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

	Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

	/*-------------------------- Program will Wait here till a Character is received ------------------------*/				

	if (Status == FALSE)
	{
		printf("Error! in Setting WaitCommEvent()\n");
	}
	else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
	{
		do
		{
			Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
			SerialBuffer[i] = TempChar;
			i++;
	    }
		while (NoBytesRead > 0);

		

		/*------------Printing the RXed String to Console----------------------*/
		int k=0;
		while(!isdigit(SerialBuffer[k])&&(SerialBuffer[k]!='-')&&(SerialBuffer[k]!='+'))
			k++;
		int j = 0;
		for (j = k; j < i-1; j++){		// j < i-1 to remove the dupliated last character
			read_buff[j-k] = SerialBuffer[j];
		}
		read_buff[j-k] = '\0';

	}	
	return read_buff;
}

__declspec(dllexport) __stdcall int serialStatus(){
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	int res;

	int OK=0;
	res=ClearCommError( hComm, &dwErrorFlags, &ComStat );
	if (res==0) {
		//error
		OK = GetLastError();
	}
	return OK;
}

__declspec(dllexport) __stdcall int serialAvailable(){
	unsigned long find=0;
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	int res;

	int OK=0;
	res=ClearCommError( hComm, &dwErrorFlags, &ComStat );
	if (res==0) {
		//error
		OK = GetLastError();
		return 0;
	}
	//Sleep(1);
	find=ComStat.cbInQue;
	return find; 	
}

__declspec(dllexport) __stdcall void serialFlush(){
	int OK=0;
	int res = PurgeComm(hComm,PURGE_RXABORT|PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR);
	if (res==0){
		//error
		OK = GetLastError();
		_tprintf(TEXT("Error in Serial Flush (%d).\n"),OK);
		exit(1);
	}
	_tprintf(TEXT("Flushed\n"));
}

__declspec(dllexport) __stdcall void serialWait(){
	int OK=0;
	LPDWORD event;
	int res = WaitCommEvent(hComm,event,NULL);
	if (res==0)//error
	{
		OK = GetLastError();
		_tprintf(TEXT("Error in Serial Wait (%d).\n"),OK);
		exit(1);
	}
}

__declspec(dllexport) __stdcall void serialEnd(){
	int OK=0;
	int res = CloseHandle(hComm);
	if (res==0){
		//error
		OK = GetLastError();
		_tprintf(TEXT("Error Closing Serial Port (%d).\n"),OK);
		exit(1);
	}
}

#endif //SerialMI_Win.h
