#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 256
TCHAR szName1[]=TEXT("Global\\MyFileMappingObject1");
TCHAR szName2[]=TEXT("Global\\MyFileMappingObject2");

HANDLE hMapFile1;
HANDLE hMapFile2;

int shmaccess()
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

void shmclose()
{
  CloseHandle(hMapFile1);
  CloseHandle(hMapFile2);
}

int shmwrite(char* szMsg)
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
      return 1;
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

      return 1;
   }


   CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
    _getch();

   UnmapViewOfFile(pBuf);

   //CloseHandle(hMapFile1);
   return 0;
}

int shmread()
{
   LPCTSTR pBuf;

   hMapFile2 = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName2);               // name of mapping object

   if (hMapFile2 == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
      return 1;
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

      CloseHandle(hMapFile2);

      return 1;
   }

   //MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);
   printf("%s\n",pBuf);

   UnmapViewOfFile(pBuf);

   //CloseHandle(hMapFile2);
   return 0;
}


// int _tmain()
// {
//   int started = shmaccess();
//   if(!started)
//   {
//     char msg[32] = "Going 2";
//     int OK = shmwrite(msg);
//     OK= shmread();
//   }
//   shmclose();
//    return 0;
// }