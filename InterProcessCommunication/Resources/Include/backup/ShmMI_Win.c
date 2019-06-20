#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
//#include "SerialMI_Win.h"
#include "serial_win.h"

// struct DataExchange {
//     char sendVal[10][32];
//     double getVal[10];
// };

//struct DataExchange *PID;

#define BUF_SIZE 256

char* pBuf;
static HANDLE hMapFile;

TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

__declspec(dllexport)  __stdcall double shmWrite(int num1,double tagValue)
{

	printf("Starting Shared Memory Write..\n");
	//LPCTSTR pBuf;
	//char* pBuf;

	hMapFile = CreateFileMappingA(
						INVALID_HANDLE_VALUE,    // use paging file
						NULL,                    // default security
						PAGE_READWRITE,          // read/write access
						0,                       // maximum object size (high-order DWORD)
						BUF_SIZE,                // maximum object size (low-order DWORD)
						szName);                 // name of mapping object

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
	    hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szName);
	}

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
		    GetLastError());
		return 1;
	}

	/*pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
	                    FILE_MAP_ALL_ACCESS, // read/write permission
	                    0,
	                    0,
	                    BUF_SIZE);
	*/
	pBuf = (char*) MapViewOfFile(hMapFile,   // handle to map object
	                    FILE_MAP_ALL_ACCESS, // read/write permission
	                    0,
	                    0,
	                    BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
		    GetLastError());

		CloseHandle(hMapFile);
		return 1;
	}

	char outData[BUF_SIZE]="";
	sprintf(outData,"%d,%g\n", num1, tagValue);
	printf("OutData:%s",outData);

	/*TCHAR outData[]=TEXT("");
	_stprintf(outData,"%d,%g\n",num1,tagValue);
	_tprintf("%s\n",outData);
	*/

	memcpy(pBuf,outData,BUF_SIZE);
	//CopyMemory((PVOID)pBuf, outData, (_tcslen(outData) * sizeof(TCHAR)));
	//CopyMemory((PVOID)pBuf, outData, strlen(outData)*sizeof(char));
	//_getch();
	//FlushViewOfFile (hMapFile, BUF_SIZE);
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	return 0;
}


__declspec(dllexport)  __stdcall double shmRead(int num2)
{
	printf("Starting Shared Memory Read...\n");
	//LPCTSTR pBuf;
	//char* pBuf;

	hMapFile = OpenFileMappingA(
	               FILE_MAP_ALL_ACCESS,   // read/write access
	               FALSE,                 // do not inherit the name
	               szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
		    GetLastError());
		 return 0;
	}

	/*pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						BUF_SIZE);
	*/
	pBuf = (char*) MapViewOfFile(hMapFile, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
		    GetLastError());

		CloseHandle(hMapFile);
		return 0;
	}

	//printf("p:%s",pBuf);
	//MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	char inData[BUF_SIZE]="";
	//memcpy(inData, pBuf, _tcslen(pBuf));
	memcpy(inData,pBuf,BUF_SIZE);
	printf("InData:%s",inData);

	int adr;	
	float returnVal=0;

	if(strcmp(inData,"")!=0)
	{
		if(sscanf(inData,"%d,%g\n",&adr,&returnVal)==0)
		{
		    _tprintf(TEXT("Error reading shared memory\n"));
			UnmapViewOfFile(pBuf);
			CloseHandle(hMapFile);
		}
		if(adr!=num2)
		{
			_tprintf(TEXT("Error accessing shared memory\n"));
			UnmapViewOfFile(pBuf);
			CloseHandle(hMapFile);
		}

		printf("OutputValue: %d:%g\n",adr,returnVal);
	}

	UnmapViewOfFile(pBuf);
	//CloseHandle(hMapFile);

	return returnVal;
}
