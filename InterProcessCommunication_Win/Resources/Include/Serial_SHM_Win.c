#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "SerialMI_Win.h"
//#pragma comment(lib, "user32.lib")


struct DataExchange {
    char sendVal[10][32];
    double getVal[10];
};

#define BUF_SIZE sizeof(struct DataExchange)

struct DataExchange *PID;
static HANDLE hMapFile;

TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
//TCHAR szMsg[]=TEXT("Message from first process.");

__declspec(dllexport)  __stdcall void shmWrite(int num1,double tagValue)
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
      exit(1);
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
	  exit(1);
   }

   char outData[]="";
   sprintf(outData,"%d,%g\n", num1, tagValue);
   strcpy(PID-> sendVal[num1], outData);

   CopyMemory((PVOID)pBuf, PID, sizeof(struct DataExchange));
    _getch();

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);

}





__declspec(dllexport)  __stdcall char* shmRead(int num2)
{
   LPCTSTR pBuf;
   TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
      exit(1);
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
	  exit(1);
   }
   PID = (struct DataExchange*)pBuf;
   //MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);


   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);

   char* returnVal;
   returnVal = PID->sendVal[num2];
   return returnVal;
}

int _tmain()
{
	//char S_Port[32]="";
    int S_Port;
	  int S_Baud;
    printf("Serial Port (e.g. 5 for COM5) : ");
    scanf("%u", &S_Port);
    printf("Baud Rate (e.g. 115200) : ");
    scanf("%d", &S_Baud);
    serialBegin(&S_Port, &S_Baud);
	  serialFlush();

	 while(1)
    {
    	  char* outData;
        char someData[32]="";
        const char* inData;
        char val[10]="";
        char addr[10]="";
        int i,j;

         for (i=0; i<10; i++)
         {
             outData = shmRead(i);
             if(strcmp(outData, "") == 0);
             else{
             	_tprintf("input:%s", outData);
             	serialWrite(outData);
             	Sleep(1000);
             	strcpy(PID->sendVal[i], "");
             }
         }

        if(serialAvailable()>0)
        {
                inData = serialRead();
                strcpy(someData, inData);
                _tprintf("ardOutput:%s\n", someData);
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

                for(j=i; j<strlen(someData); j++)
                {
                    val[j-i] = someData[j];
                }
                shmWrite(atoi(addr), atof(val));
        }
    }
printf("came out of loop..!");
serialEnd();
}
