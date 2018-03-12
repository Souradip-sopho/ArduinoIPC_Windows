#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "SerialMI_Win.h"


// struct DataExchange {
//     char sendVal[10][32];
//     double getVal[10];
// };

//struct DataExchange *PID;

static HANDLE hMapFile;

#define BUF_SIZE 256

TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
TCHAR szMsg[]=TEXT("Message from first process.");

__declspec(dllexport)  __stdcall double shmWrite(int num1,double tagValue)
{

   printf("Starting Shared Memory Write..\n");
   //LPCTSTR pBuf;
   char* pBuf;

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
   
   char outData[256]="";
   sprintf(outData,"%d,%f\n", num1, tagValue);
   printf("OutData:%s",outData);
   /*TCHAR outData[]=TEXT("");
   _stprintf(outData,"%d,%g\n",num1,tagValue);
   _tprintf("%s\n",outData);
    */

   memcpy(pBuf,outData,BUF_SIZE);
   //CopyMemory((PVOID)pBuf, outData, (_tcslen(outData) * sizeof(TCHAR)));
   //CopyMemory((PVOID)pBuf, outData, strlen(outData)*sizeof(char));
   //_getch();
  
   UnmapViewOfFile(pBuf);

   //CloseHandle(hMapFile);
   return 0;
}


__declspec(dllexport)  __stdcall double shmRead(int num2)
{
   //LPCTSTR pBuf;
   char* pBuf;
   printf("Opening...\n");
   hMapFile = OpenFileMappingA(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
	     return 1;
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
      return 1;
   }

   //printf("p:%s",pBuf);
   //MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

   char inData[256];
   //memcpy(inData, pBuf, _tcslen(pBuf));
   memcpy(inData,pBuf,BUF_SIZE);
   printf("InData:%s",inData);
	 int adr;	
   float returnVal;
   if(sscanf(inData,"%d,%f\n",&adr,&returnVal)==0)
	     printf("Error reading shared memory\n");
	 printf("OutputValue:%f\n",returnVal);
   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);
   
  return returnVal;
}
