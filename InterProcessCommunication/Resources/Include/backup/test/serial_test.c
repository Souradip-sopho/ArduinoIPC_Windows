#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "serial_win.h"

int main()
{
	//char S_Port[32]="";
    int S_Port;
	int S_Baud;
    printf("Serial Port (e.g. 5 for COM5) : ");
    scanf("%d", &S_Port);
    printf("Baud Rate (e.g. 115200) : ");
    scanf("%d", &S_Baud);
    serialBegin(&S_Port, &S_Baud);
	serialFlush();

	char someData[32]="1,0\n";
	while(1)
    {
    	char outData[32];
        //char someData[32]="";
        const char* inData;
        char val[31]="";
        char addr[2]="";
        int i=0,j=0;

        strcpy(outData,someData);
        if(strcmp(outData, "") == 0);
        else{
        	printf("input:%s", outData);
        	serialWrite(outData);
        	Sleep(1000);
            strcpy(outData,"");
        }

        int A = serialAvailable();
        // inData = serialRead();
        // strcpy(someData, inData);
        // printf("ardOutput:%s\n", someData);
        printf("Available:%d\n",A); 
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

                for(j=i; j<strlen(someData); j++)
                {
                    val[j-i] = someData[j];
                }
                printf("ADR: %d, %g\n",atoi(addr),atof(val));
                //shmWrite(1, atof(val));
        }
    }
  printf("came out of loop..!");
  serialEnd();
}
