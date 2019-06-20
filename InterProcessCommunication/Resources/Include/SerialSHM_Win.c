#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "SerialMI_Win.h"
// #pragma comment(lib, "user32.lib")

#define BUF_SIZE 32

HANDLE hMapFile1;
HANDLE hMapFile2;

TCHAR szName1[]=TEXT("Global\\MyFileMappingObject1");
TCHAR szName2[]=TEXT("Global\\MyFileMappingObject2");

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

__declspec(dllexport)  __stdcall void shmWrite(int num1,double tagValue)
{

	LPCTSTR pBuf;
	
	hMapFile1 = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName1);               // name of mapping object

	if (hMapFile1 == NULL)
	{
	  _tprintf(TEXT("Could not open file mapping object (%d).\n"),
	         GetLastError());
	  return;
	}

	pBuf = (LPTSTR) MapViewOfFile(hMapFile1,   // handle to map object
	                    FILE_MAP_ALL_ACCESS, // read/write permission
	                    0,
	                    0,
	                    BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile1);
	}

	char szMsg[BUF_SIZE]="";
	sprintf(szMsg,"%d,%g\n", num1, tagValue);

	CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

	UnmapViewOfFile(pBuf);
}



__declspec(dllexport)  __stdcall char* shmRead(int num2)
{
	LPCTSTR pBuf;

	static char returnVal[BUF_SIZE]="";

	hMapFile2 = OpenFileMapping(
						FILE_MAP_ALL_ACCESS,   // read/write access
						FALSE,                 // do not inherit the name
						szName2);               // name of mapping object

	if (hMapFile2 == NULL)
	{
		return returnVal;
	}

	pBuf = (LPTSTR) MapViewOfFile(hMapFile2, // handle to map object
						FILE_MAP_ALL_ACCESS,  // read/write permission
						0,
						0,
						BUF_SIZE);
	
	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
		    GetLastError());
		return returnVal;
	}

	memcpy(returnVal,pBuf,BUF_SIZE);
   
	int adr; 
	float Val;
	if(strcmp(returnVal,"")!=0)
	{
		if(sscanf(returnVal,"%d,%g\n",&adr,&Val)==0)
		{
			_tprintf(TEXT("Error reading shared memory\n"));
			UnmapViewOfFile(pBuf);
			return "";
		}
		if(adr!=num2)
		{
			_tprintf(TEXT("Error accessing shared memory\n"));
			UnmapViewOfFile(pBuf);
			return "";
		}  
	}

	UnmapViewOfFile(pBuf);

	return returnVal;
}

int main()
{
    int S_Port;
	int S_Baud;
    printf("Serial Port (e.g. 5 for COM5) : ");
    scanf("%d", &S_Port);
    printf("Baud Rate (e.g. 115200) : ");
    scanf("%d", &S_Baud);
    serialBegin(&S_Port, &S_Baud);
	serialFlush();
	int OK = shmBegin();
	if(OK!=0)
		return 0;

	char prevData[]="*,0\n";
	int flag = 0;

	while(1)
    {
    	char* outData;
        char someData[32]="";
        const char* inData;
        char val[31]="";
        char addr[3]="";
        int i=0,j=0;

        outData = shmRead(1);
        if(strcmp(outData, "") == 0){
        	if(flag==0){
        		flag = 1;
	        	serialWrite(prevData);
	        	Sleep(10);
        	}
        }
        else{
        	printf("input:%s", outData);
        	serialWrite(outData);
        	Sleep(10);
        	//strcpy(prevData,outData);
            strcpy(outData,"");
        }

        int A = serialAvailable(); 
        if(A>0)
        {
                inData = serialRead();
                strcpy(someData, inData);
                printf("ardOutput:%s", someData);
                for(i=0; i<strlen(someData); i++)
                {
                    if(someData[i]==',')
                    {
                        addr[i]='\0';
                        i++;
                        break;
                    }
                    addr[i] = someData[i];
                }

                for(j=i; j<strlen(someData)-1; j++)
                {
                    val[j-i] = someData[j];
                }
                shmWrite(atoi(addr), atof(val));
        }
    }
	printf("came out of loop..!");
	shmEnd();
	serialEnd();
	return 0;
}
