#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
// #pragma comment(lib, "user32.lib")

#define BUF_SIZE 32

TCHAR szName1[]=TEXT("Global\\MyFileMappingObject1");
TCHAR szName2[]=TEXT("Global\\MyFileMappingObject2");

HANDLE hMapFile1;
HANDLE hMapFile2;

int shmBegin()
{
	hMapFile1 = CreateFileMapping(
	           INVALID_HANDLE_VALUE,    // use paging file
	           NULL,                    // default security
	           PAGE_READWRITE,          // read/write access
	           0,                       // maximum object size (high-order DWORD)
	           BUF_SIZE,                // maximum object size (low-order DWORD)
	           szName1);                 // name of mapping object
	if (hMapFile1 == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
		     GetLastError());
		return 1;
	}
	hMapFile2 = CreateFileMapping(
	             INVALID_HANDLE_VALUE,    // use paging file
	             NULL,                    // default security
	             PAGE_READWRITE,          // read/write access
	             0,                       // maximum object size (high-order DWORD)
	             BUF_SIZE,                // maximum object size (low-order DWORD)
	             szName2);                 // name of mapping object
	if (hMapFile2 == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
		     GetLastError());
		return 1;
	}
	return 0;
}

void shmEnd()
{
	CloseHandle(hMapFile1);
	CloseHandle(hMapFile2);
}

__declspec(dllexport)  __stdcall double shmWrite(int num1,double tagValue)
{

	//printf("Starting Shared Memory Write..\n");

	char* pBuf;

	hMapFile2 = CreateFileMappingA(
						INVALID_HANDLE_VALUE,    // use paging file
						NULL,                    // default security
						PAGE_READWRITE,          // read/write access
						0,                       // maximum object size (high-order DWORD)
						BUF_SIZE,                // maximum object size (low-order DWORD)
						szName2);                 // name of mapping object

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
	    hMapFile2 = OpenFileMappingA(
    					FILE_MAP_ALL_ACCESS, 
    					FALSE, 
    					szName2);
	}

	if (hMapFile2 == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
		    GetLastError());
		return 1;
	}

	pBuf = (char*) MapViewOfFile(hMapFile2,   // handle to map object
	                    FILE_MAP_ALL_ACCESS, // read/write permission
	                    0,
	                    0,
	                    BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
		    GetLastError());

		CloseHandle(hMapFile2);
		return 1;
	}

	char outData[BUF_SIZE]="";
	sprintf(outData,"%d,%g\n", num1, tagValue);
	printf("OutData:%s",outData);

	memcpy(pBuf,outData,BUF_SIZE);
	
	UnmapViewOfFile(pBuf);
	return 0;
}


__declspec(dllexport)  __stdcall double shmRead(int num2)
{
	// printf("Starting Shared Memory Read...\n");

	char* pBuf;

	hMapFile1 = OpenFileMappingA(
	               FILE_MAP_ALL_ACCESS,   // read/write access
	               FALSE,                 // do not inherit the name
	               szName1);               // name of mapping object

	if (hMapFile1 == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
		    GetLastError());
		 return 0;
	}

	pBuf = (char*) MapViewOfFile(hMapFile1, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
		    GetLastError());

		CloseHandle(hMapFile1);
		return 0;
	}

	char inData[BUF_SIZE]="";
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
		}
		if(adr!=num2)
		{
			_tprintf(TEXT("Error accessing shared memory\n"));
			UnmapViewOfFile(pBuf);
		}
	}

	UnmapViewOfFile(pBuf);

	return returnVal;
}


