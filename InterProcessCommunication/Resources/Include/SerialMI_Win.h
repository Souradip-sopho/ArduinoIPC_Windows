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



//#ifdef _WIN32
#include <Windows.h>
//#else
//#include <unistd.h>
//#endif

#define MAXPORTS 5 //unused : can be used to opened several COM to have several Arduino card

// Static definition to stock HANDLE of Port.
static HANDLE hport;

// Function to open port COM
__declspec(dllexport)  __stdcall int serialBegin(int *port, int *baudrate){
	DCB dcbSerialParams ;
	DWORD dwBytesWrite = 0;
	DWORD dwBytesRead = 10;
	COMMTIMEOUTS timeouts={0};

	int OK;

	char tmp[5]="COM5";
	itoa(*port,&tmp[3],10);

	OK=0;


  	hport = CreateFile(tmp,
							GENERIC_READ | GENERIC_WRITE,
							0,//FILE_SHARE_READ | FILE_SHARE_WRITE //to test : recuperation COM port if simulation crashes
							0,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							0);

	if(hport==INVALID_HANDLE_VALUE){
	  if(GetLastError()==ERROR_FILE_NOT_FOUND){
	     //serial port does not exist. Inform user.
	     OK = GetLastError();
		 return OK;
	  }
	//some other error occurred. Inform user.
	  OK = GetLastError();
	  return OK;
	}



	dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
	if (!GetCommState(hport, &dcbSerialParams)) {
	//error getting state
	     OK = GetLastError();
		 return OK;
	}

	dcbSerialParams.BaudRate=*baudrate;
	dcbSerialParams.ByteSize=8;
	dcbSerialParams.StopBits=ONESTOPBIT;
	dcbSerialParams.Parity=NOPARITY;
	if(!SetCommState(hport, &dcbSerialParams)){
		//error setting serial port state
	    OK = GetLastError();
		return OK;
	}


	timeouts.ReadIntervalTimeout=50;
	timeouts.ReadTotalTimeoutConstant=50;
	timeouts.ReadTotalTimeoutMultiplier=1;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=1;
	if(!SetCommTimeouts(hport, &timeouts)){
	//error occureed. Inform user
		OK = GetLastError();
	    return OK;
	}
	Sleep(1000);
	_tprintf("Serial connection established.\n");
	return 0;
}


__declspec (dllexport) __stdcall void serialWrite(char* write_buff){
	DWORD dwBytesWrite = 0;
	int res;
	int OK=0;
	res = WriteFile(hport,write_buff,strlen(write_buff),&dwBytesWrite,NULL);
	if (res==0) //error
	{
		OK = GetLastError();
		fprintf(stderr, "Error in writing data (%d).",OK);
		exit(1);
	}
}

__declspec (dllexport) __stdcall int serialStatus(){
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	int res;

	int OK=0;
	res=ClearCommError( hport, &dwErrorFlags, &ComStat );
	if (res==0) {//error
		OK = GetLastError();
	}
	//*nbread=ComStat.cbInQue;
	//*nbwrite=ComStat.cbOutQue;
	return OK;
}

__declspec (dllexport) __stdcall const char* serialRead(){

	static char read_buff[32]="";
	DWORD dwBytesRead = 0;
	int byte_r = ReadFile(hport, read_buff, 32, &dwBytesRead, NULL);
	if(byte_r==0)
	{
		_tprintf(TEXT("Error reading Serial Port (%d).n"),GetLastError());
		return "";
	}
	char temp_char='\0';
	int i=0;
	for(i=0; i<32; i++)
	{
		temp_char=read_buff[i];	
		//int byte_r = ReadFile(hport, &temp_char, 1, &dwBytesRead, NULL);
		if(temp_char == '\n') break;	

		//read_buff[i] = temp_char;
	}
	read_buff[i] = '\0';
	printf("%s\n",read_buff);
	return read_buff;
}

__declspec (dllexport) __stdcall int serialAvailable()
{
	unsigned long find;
	//ioctlsocket((SOCKET)hport, FIONREAD, &find);
	//char buffer[32]="";
	//unsigned long returned;
	//int res=DeviceIoControl(hport,FSCTL_CREATE_OR_GET_OBJECT_ID,NULL,0,&find,sizeof(int),&returned,sizeof(int));
	DWORD dwErrorFlags;
	COMSTAT ComStat;
	int res;

	int OK=0;
	res=ClearCommError( hport, &dwErrorFlags, &ComStat );
	if (res==0) {//error
		OK = GetLastError();
		return 0;
	}
	Sleep(1);
	find=ComStat.cbInQue;
	if (find<32)
		return find;
	else
		return 0;
}

__declspec (dllexport) __stdcall void serialFlush()
{
	int OK=0;
	int res = PurgeComm(hport,PURGE_RXABORT|PURGE_TXABORT|PURGE_RXCLEAR|PURGE_TXCLEAR);
	if (res==0)//error
	{
		OK = GetLastError();
		_tprintf(TEXT("Error in Serial Flush (%d).\n"),OK);
		exit(1);
	}
	//return OK;
}

__declspec (dllexport) __stdcall void serialWait()
{
	int OK=0;
	LPDWORD event;
	int res = WaitCommEvent(hport,event,NULL);
	if (res==0)//error
	{
		OK = GetLastError();
		_tprintf(TEXT("Error in Serial Wait (%d).\n"),OK);
		exit(1);
	}
	//return OK;
}

__declspec (dllexport) __stdcall void serialEnd()
{
	int OK=0;
	int res = CloseHandle(hport);
	if (res==0)//error
	{
		OK = GetLastError();
		_tprintf(TEXT("Error Closing Serial Port (%d).\n"),OK);
		exit(1);
	}
	//return OK;
}

#endif //SerialMI_Win.h
