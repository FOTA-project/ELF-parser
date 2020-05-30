#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <conio.h>
#include <windows.h>


HANDLE GetSerialPort(char *);
 

void main(void)
{
    HANDLE h1;
    char h1_buffer[] = "iti :)"; 
    char h2_buffer[7];
    DWORD byteswritten = 0, bytesread = 0;

  
    h1 = GetSerialPort("COM3");

    while(1)
    {
       
        WriteFile(h1, h1_buffer, sizeof(h1_buffer) , &byteswritten, NULL) ;
        for(int i=0; i<100 ; i++);
       
        ReadFile(h1, h2_buffer, sizeof(h2_buffer), &bytesread, NULL) ;
        for(int i=0; i<100 ; i++);
       
        if (bytesread)
            printf("%s\n", h2_buffer);
        else
            printf("Nothing read\n"); 
               
                     
    }
    CloseHandle(h1); 
    //getch();   
}

HANDLE GetSerialPort(char *p)
{
    HANDLE hSerial;
    hSerial = CreateFile(p,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         0);

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
    dcbSerialParams.BaudRate=CBR_9600;//CBR_9600
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.Parity=NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);
    return hSerial;
}