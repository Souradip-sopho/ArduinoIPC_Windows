#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "SerialMI_Win.h"


struct DataExchange {
    char sendVal[10][32];
    double getVal[10];
};

struct DataExchange *PID;
static HANDLE hMapFile;

//#define BUF_SIZE sizeof(struct DataExchange)
#define BUF_SIZE 256

TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

__declspec(dllexport)  __stdcall double shmWrite(int num1,double tagValue)
{

   LPCTSTR pBuf;

   hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 szName);                 // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not create file mapping object (%d).\n"),
             GetLastError());
	  return 1;
   }
   pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
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

   //char outData[]="";
   //sprintf(outData,"%d,%g\n", num1, tagValue);
   //strcpy(PID-> sendVal[num1], outData);
	TCHAR outData[]="";
	_stprintf(outData,"%d,%g\n",num1,tagValue);
   CopyMemory((PVOID)pBuf, outData, (_tcslen(outData) * sizeof(TCHAR)));
   //CopyMemory((PVOID)pBuf, PID, sizeof(struct DataExchange));
    _getch();

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);
   return 0;
}


__declspec(dllexport)  __stdcall double shmRead(int num2)
{
   LPCTSTR pBuf;
 
   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
	  return 1;
   }

   pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
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
   //PID = (struct DataExchange*)pBuf;
   MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	int adr;	
   
   double returnVal;
   //returnVal = PID->getVal[num2];
	_stscanf(pBuf,"%d,%g\n",&adr,&returnVal);
	
   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);
   
   return returnVal;
}
